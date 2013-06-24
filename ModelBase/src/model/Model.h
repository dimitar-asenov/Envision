/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#pragma once

#include "../modelbase_api.h"
#include "../nodes/Node.h"
#include "../concurrent/NodeReadWriteLock.h"
#include "ModelManager.h"

namespace Model {

class Reference;
class UndoCommand;
class NodeOwningCommand;

/**
 * The Model class is a management and access entity for a program tree.
 *
 * Each program or project in Envision is represented as a tree of nodes called a program model. To manage this tree
 * each project has an associated Model object.
 *
 * The life cycle of a project begins with the creation of a new Model object. This object can then be populated with
 * program nodes by loading an existing tree from a persistent store or by creating a new tree structure.
 *
 * Further access and operations on the tree are performed through the Model object. Here is a list of the functionality
 * of a Model:
 * 	- Root node creation
 * 	- Saving/loading a model to/from a persistent store
 *  - Access control to nodes in a concurrent setting
 *  - Undo/redo functionality
 *  - Notifications of model change
 */
class MODELBASE_API Model: public QObject
{
	Q_OBJECT
	// The moc compiler is used only to support signals and slots.

	public:

		/**
		 * Constructs a new Model with the given \a root.
		 *
		 * The undo history limit is set to 100 operations.
		 */
		Model(Node* root = nullptr);

		/**
		 * Constructs a new Model with the given \a name and \a root.
		 *
		 * The undo history limit is set to 100 operations.
		 */
		Model(const QString& name, Node* root = nullptr);

		/**
		 * Deletes the root node corresponding to the Model.
		 *
		 * Deleting a node normally causes the corresponding subtree to be deleted.
		 */
		virtual ~Model();

		/**
		 * Begins a modification session.
		 *
		 * Before calling a method on a tree node that modifies the node, a writer thread must first begin a modification
		 * session. Write access  to the underlying tree model must always occure in a modification block. Call
		 * Model::beginModification() to begin this block and Model::endModification() to end the block:
		 *
		 * Model m;
		 * ...
		 * Node* someNode;
		 * ...
		 * m.beginModification(someNode, "modification description");
		 * ... Code that makes modification to the subtree rooted at someNode, and within the access unit corresponding
		 * ... to someNode
		 * m.endModification();
		 * ... Other exclusive readers or other writers can now be granted access
		 *
		 * Only one thread can be in a modification session at a time. Furthermore this thread can only modify nodes which
		 * fall within the current access unit.
		 *
		 * Each node has a unique access unit associated with it. An access unit is simply identified by a lock. The
		 * Node::getAccessLock() method can be used to get the access lock associated with a node. By default this is the
		 * lock of the parent node, or if there is no parent node (this is the root node), the lock of the Model. The
		 * user can reimplement the Node::getAccessLock() method to return a new access lock therby defining a new access
		 * unit.
		 *
		 * If there is a reader that has requested exclusive access via Model::beginExclusiveRead() this method will block
		 * until the reader calls Model::endExclusiveRead()
		 *
		 * @param modificationTarget
		 * 				the top-most ancestor node of all nodes that will be modified.
		 *
		 * @param text
		 * 				the text that will be associated with this modification and put on the undo stack. This parameter
		 * 				has no effect if this modification is used to call undo/redo and not to execute new commands.
		 */
		void beginModification(Node* modificationTarget, const QString &text = QString());

		/**
		 * Ends a modification session.
		 *
		 * This method must always be called after a call Model::beginModification(). It concludes the modification
		 * session by releasing all acquired locks and emits the Mode::nodesModified signal to notify listeners
		 * about the changes.
		 *
		 * This method must be called before the any other thread can gain exclusive access to the tree model.
		 */
		void endModification();

		bool isBeingModified() const;

		/**
		 * Changes the modification target and possibly the access unit that the current writer is allowed to access.
		 *
		 * This method can be used within a modification block in order to change the current modification target. This
		 * will release the access lock of the current target and acquire the lock of the new target.
		 *
		 * @param modificationTarget
		 * 				The target that should become the current modification target. The writer thread will be allowed
		 * 				to modify all nodes belonging to the subtree of this target and falling within the same access unit
		 *
		 * @exception ModelException
		 * 				if this method is called without calling Model::beginModification() first.
		 */
		void changeModificationTarget(Node* modificationTarget);

		Node* modificationTarget() const;

		/**
		 * Begins an exclusive read.
		 *
		 * During an exclusive read, the application model can not be modified by any other thread. Other readers,
		 * including exclusive readers will be allowed to access the model.
		 *
		 * Call Model::endExclusiveRead() to end the exclusive reading block.
		 */
		void beginExclusiveRead();

		/**
		 * Ends an exclusive read.
		 *
		 * This function must always be called after a call to Model::beginExclusiveRead()
		 */
		void endExclusiveRead();

		/**
		 * Returns the lock corresponding to the top-most (root) access unit.
		 */
		NodeReadWriteLock* rootLock();

		/**
		 * Checks if a node can be modified.
		 *
		 * A node can be modified if the following three conditions are met:
		 * 	- The Model is in a modification session, started with Model::beginModification().
		 * 	- It is, or has as a parent, the current modification target.
		 * 	- It belongs to the same access unit as the current modification target.
		 *
		 * This method returns true if all conditions are met and false otherwise.
		 *
		 * @param node
		 * 				The node to check.
		 */
		bool canBeModified(const Node* node) const;

		/**
		 * Returns the root node for this model.
		 */
		Node* root();

		/**
		 * Returns the name of the model. This is the name under which this model can be found in the persistent store.
		 */
		QString name();

		/**
		 * Sets the name of the model. This is the name under which this model will be save in the persistent store.
		 */
		void setName(const QString& name);

		/**
		 * Sets the root node of this model to \a node.
		 *
		 * The model should not have a root node set. This method must be called outside of a modification block.
		 * This action can not be undone.
		 */
		void setRoot(Node* node);

		/**
		 * Pushes the specified command on the undo stack and executes it.
		 *
		 * This method can only be called inside a modification block initiated with Model::beginModification().
		 * Furthermore no calls to Model::undo or Model::redo can be made in the current modification block.
		 *
		 * @param command
		 * 				The command to add to the stack.
		 *
		 * @exception ModelException
		 * 				if called without calling Model::beginModification() first or if calls to Model::undo() or
		 * 				Model::redo() have already been made.
		 */
		void pushCommandOnUndoStack(UndoCommand* command);

		/**
		 * Undoes the commands executed in the previous modification block.
		 *
		 * @exception ModelException
		 * 				if called without calling Model::beginModification() first or if a call to
		 * 				Model::pushCommandOnUndoStack() has already been made within this modification block.
		 */
		void undo();

		/**
		 * Redoes the commands executed in the next modification block.
		 *
		 * @exception ModelException
		 * 				if called without calling Model::beginModification() first or if a call to
		 * 				Model::pushCommandOnUndoStack() has already been made within this modification block.
		 */
		void redo();

		/**
		 * Returns whether \a node is owned by a command in the undo stack.
		 *
		 * If \a node is only owned by \a excludeCommand this method returns false.
		 */
		bool isOwnedByUndoStack(const Node* node, const NodeOwningCommand* excludeCommand) const;

		/**
		 * Saves the model tree in persistent store of the model.
		 *
		 * If this model does not have a current store, the store argument must not be NULL. In that case the provided
		 * store will become the current store of the model.
		 *
		 * @param store
		 * 				The persistent store where the model should be saved. If this is NULL the current store will be
		 * 				used. The current store is the one from which the model was loaded. If this is not NULL it will
		 * 				be used instead of the current store.
		 */
		void save(PersistentStore* store = nullptr);

		/**
		 * Loads the current model tree from the specified persistent store. The provided store will become the current
		 * store of the model.
		 *
		 * If this model already has a root node, this method does nothing.
		 *
		 * @param store
		 * 				The persistent store where the model should be loaded from.
		 *
		 * @param name
		 * 				The model name in the persistent store which contains the model tree.
		 */
		void load(PersistentStore* store, const QString& name);

		/**
		 * Returns the current store for this model.
		 *
		 * The current store is the store used to load the model. If this model was created rather than loaded and has
		 * not been saved to a persistent store yet this method will return NULL.
		 */
		PersistentStore* store();

		/**
		 * Causes a nameModified signal to be emitted.
		 *
		 * @param node
		 * 				Which node's name has changed
		 * @param oldNmae
		 * 				What was the old name of the node
		 */
		void emitNameModified(Node* node, const QString &oldName);

		/**
		 * Causes a nodeFullyLoaded signal to be emitted.
		 *
		 * @param node
		 * 				The node which is now fully loaded.
		 */
		void emitNodeFullyLoaded(Node* node);

		/**
		 * Causes a nodePartiallyLoaded signal to be emitted.
		 *
		 * @param node
		 * 				The node which is now partially loaded.
		 */
		void emitNodePartiallyLoaded(Node* node);

		/**
		 * \brief Returns a list of all loaded unresolved references in the model.
		 *
		 * Unresolved references which are persisted but not loaded are will not be in the returned list.
		 */
		const QList<Reference*>& unresolvedReferences() const;

		/**
		 * \brief Adds the specified reference to the unresolved references list.
		 */
		void addUnresolvedReference(Reference* ref);

		/**
		 * \brief Removes the specified reference to the unresolved references list.
		 *
		 * Call this method when a reference has been resolved.
		 */
		void removeUnresolvedReference(Reference* ref);

		/**
		 * \brief Attempt to resolve all unresolved references.
		 *
		 * This method must be called within a modification session.
		 */
		void tryResolvingReferences();

		ModelManager& manager() const;

	signals:
		/**
		 * Emitted when a new root node was set.
		 *
		 * @param root
		 * 				A pointer to the new root node
		 */
		void rootNodeSet(Node* root);

		/**
		 * Emitted at the end of a modification block.
		 *
		 * @param nodes
		 * 				A list of all nodes modified within this block.
		 */
		void nodesModified(QList<Node*> nodes);

		/**
		 * Emitted when the name of a node has changed
		 *
		 * This signal can be used to trigger symbolic reference updates, so that references are kept consistent when the
		 * name of a symbol changes.
		 *
		 * @param node
		 * 				The node that has a new name
		 * @param oldName
		 * 				The old name of the node
		 */
		void nameModified(Node* node, const QString &oldName);

		/**
		 * Emitted when a partially loaded node becomes fully loaded.
		 *
		 * @param node
		 * 				The node which is now fully loaded.
		 */
		void nodeFullyLoaded(Node* node);

		/**
		 * Emitted when a fully loaded node becomes partially loaded.
		 *
		 * @param node
		 * 				The node which is now partially loaded.
		 */
		void nodePartiallyLoaded(Node* node);

	private:

		/**
		 * Returns whether \a node is transitively owned by \a cmd.
		 *
		 * If \a node is only owned by \a excludeCommand this method returns false.
		 */
		static bool isOwnedByCommand(const Node* node, const UndoCommand* cmd, const NodeOwningCommand* excludeCommand);

		/** The name of this model. This name will be used to save the model in the persistent store. */
		QString name_;

		/** The root node for this model */
		Node* root_{};

		/** The command stack that holds the undo history */
		QUndoStack commands;

		/**
		 * A ReadWrite lock used for exclusive writer or reader access. Writers always acquire this lock first and then
		 * proceed to acquire exactly one other lock corresponding to the access unit they want to write to.
		 *
		 * A reader that requires exclusive access (other readers can still read, but writers can not) can also acquire
		 * this lock. Typically however readers do not acquire this lock.
		 */
		QReadWriteLock exclusiveAccess;

		/**
		 * The lock corresponding to the root access unit. This is used by synchronized readers and by writers to control
		 * access to the root unit.
		 */
		NodeReadWriteLock rootLock_;

		/**
		 * The test message associated with the current modification operation. This text is only used to describe the
		 * undo operation that is currently performed.
		 */
		QString modificationText;

		/**
		 * The node that is the top-most ancestor of all other nodes that are currently being modification. Only nodes in
		 * the same access unit and below this node (including the node itself) may be modified.
		 */
		Node* currentModificationTarget{};

		/**
		 * The lock of the access unit that is responsible for the current modification target node. Each node has exactly
		 * one access unit corresponding to it. A writer must acquire this lock before any modification can occur. The
		 * currently acquired lock is stored here so that each write operation can be checked to assure that no write
		 * occurs outside of the access unit.
		 */
		NodeReadWriteLock* currentModificationLock{};

		/**
		 * A list of all top-level nodes which were modified as part of the last modification operation. This is only used
		 * to signal to anyone who is interested in monitoring changes.
		 */
		QList<Node*> modifiedTargets;

		/**
		 * Indicates if the last modification operation pushed commands on stack.
		 *
		 * A modification operation can do two things:
		 * 	- Add new commands on the stack
		 * 	- Call undo/redo
		 *
		 * The two can not be mixed in the same operation and this flag is used to control this.
		 */
		bool pushedNewCommandsOnTheStack{};

		/**
		 * Indicates if undo() or redo() were called during the last modification operation.
		 *
		 * A modification operation can do two things:
		 * 	- Add new commands on the stack
		 * 	- Call undo/redo
		 *
		 * The two can not be mixed in the same operation and this flag is used to control this.
		 */
		bool performedUndoRedo{};

		/**
		 * This flag indicates if a modification is currently in progress. Commands can be pushed on the undo stack and
		 * executed only if this is true.
		 */
		bool modificationInProgress{};

		/**
		 * The persistent store where the model is currently stored.
		 *
		 * This is used in calls to Node::loadFully when a partially loaded node needs to load its entire contents.
		 * It can also be used by other stores when the model needs to be saved to a different location.
		 */
		PersistentStore* store_{};

		/**
		 * A list of all unresolved references which are currently loaded nodes.
		 */
		QList<Reference*> unresolvedReferences_;
};

inline bool Model::isBeingModified() const { return modificationInProgress; }
inline Node* Model::modificationTarget() const { return currentModificationTarget; }
inline NodeReadWriteLock* Model::rootLock() { return &rootLock_; }

inline Node* Model::root(){ return root_; }
inline QString Model::name() { return name_; }
inline void Model::setName(const QString& name) { name_ = name; }

inline PersistentStore* Model::store() { return store_; }
inline void Model::emitNameModified(Node* node, const QString &oldName) { emit nameModified(node, oldName); }
inline void Model::emitNodeFullyLoaded(Node* node) { emit nodeFullyLoaded(node); }
inline void Model::emitNodePartiallyLoaded(Node* node) { emit nodePartiallyLoaded(node); }

inline const QList<Reference*>& Model::unresolvedReferences() const { return unresolvedReferences_; }

inline ModelManager& Model::manager() const { return ModelManager::instance(); }

}
