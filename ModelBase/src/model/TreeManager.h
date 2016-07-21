/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "../concurrent/NodeReadWriteLock.h"
#include "../persistence/NodeIdMap.h"

namespace Model {

class Node;
class UndoCommand;
class NodeOwningCommand;
class NameText;
class PersistentStore;

/**
 * The TreeManager class is a management and access entity for a program tree.
 *
 * Each program or project in Envision is represented as a tree of nodes. To manage this tree each project has an
 * associated TreeManager object.
 *
 * The life cycle of a project begins with the creation of a new TreeManager object. This object can then be populated
 * with program nodes by loading an existing tree from a persistent store or by creating a new tree structure.
 *
 * Further access and operations on the tree are performed through the TreeManager object. Here is a list of the
 * functionality of a TreeManager:
 * - Root node creation
 * - Saving/loading a project to/from a persistent store
 * - Access control to nodes in a concurrent setting
 * - Undo/redo functionality
 * - Notifications of project change
 */
class MODELBASE_API TreeManager: public QObject
{
	Q_OBJECT

	public:

		/**
		 * Constructs a new TreeManager with the given \a root.
		 *
		 * The undo history limit is set to 100 operations.
		 */
		TreeManager(Node* root = nullptr);

		/**
		 * Constructs a new TreeManager with the given \a name and \a root.
		 *
		 * The undo history limit is set to 100 operations.
		 */
		TreeManager(const QString& name, Node* root = nullptr);

		/**
		 * Deletes the root node corresponding to the TreeManager.
		 *
		 * Deleting a node normally causes the corresponding subtree to be deleted.
		 */
		virtual ~TreeManager();

		/**
		 * Begins a modification session.
		 *
		 * Before calling a method on a tree node that modifies the node, a writer thread must first begin a modification
		 * session. Write access to the underlying tree must always occur in a modification block. Call
		 * TreeManager::beginModification() to begin this block and TreeManager::endModification() to end the block:
		 *
		 * TreeManager m;
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
		 * lock of the parent node, or if there is no parent node (this is the root node), the lock of the TreeManager.
		 * The user can reimplement the Node::getAccessLock() method to return a new access lock therby defining a new access
		 * unit.
		 *
		 * If there is a reader that has requested exclusive access via TreeManager::beginExclusiveRead() this method
		 * will block until the reader calls TreeManager::endExclusiveRead()
		 *
		 * @param modificationTarget
		 * 				the top-most ancestor node of all nodes that will be modified.
		 *
		 * @param text
		 * 				the text that will be associated with this modification and put on the undo stack. This parameter
		 * 				has no effect if this modification is used to call undo/redo and not to execute new commands.
		 */
		void beginModification(Node* modificationTarget, const QString &text = {});

		/**
		 * Ends a modification session.
		 *
		 * This method must always be called after a call TreeManager::beginModification(). It concludes the modification
		 * session by releasing all acquired locks and emits the Mode::nodesModified signal to notify listeners
		 * about the changes.
		 *
		 * This method must be called before the any other thread can gain exclusive access to the tree.
		 *
		 * If \a tryResolvingReferences is true, then an attempt will be made to resolve all unresolved references.
		 */
		void endModification(bool tryResolvingReferences = true);

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
		 * 				if this method is called without calling TreeManager::beginModification() first.
		 */
		void changeModificationTarget(Node* modificationTarget);

		Node* modificationTarget() const;

		/**
		 * Begins an exclusive read.
		 *
		 * During an exclusive read, the tree can not be modified by any other thread. Other readers,
		 * including exclusive readers will be allowed to access the tree.
		 *
		 * Call TreeManager::endExclusiveRead() to end the exclusive reading block.
		 */
		void beginExclusiveRead();

		/**
		 * Ends an exclusive read.
		 *
		 * This function must always be called after a call to TreeManager::beginExclusiveRead()
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
		 * 	- The manager is in a modification session, started with TreeManager::beginModification().
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
		 * Returns the root node for this manager.
		 */
		Node* root();

		/**
		 * Returns the name of the manged tree. This is the name under which this tree can be found in the persistent
		 * store.
		 */
		QString name();

		/**
		 * Sets the name of the tree. This is the name under which this tree will be save in the persistent store.
		 */
		void setName(const QString& name);

		/**
		 * Returns the name of the revision.
		 */
		QString revisionName();

		/**
		 * Sets the name of the revision.
		 */
		void setRevisionName(const QString& revisionName);

		/**
		 * Sets the root node of this manager to \a node.
		 *
		 * The TreeManager should not have a root node set. This method must be called outside of a modification block.
		 * This action can not be undone.
		 */
		void setRoot(Node* node);

		/**
		 * Pushes the specified command on the undo stack and executes it.
		 *
		 * This method can only be called inside a modification block initiated with TreeManager::beginModification().
		 * Furthermore no calls to TreeManager::undo or TreeManager::redo can be made in the current modification block.
		 *
		 * @param command
		 * 				The command to add to the stack.
		 *
		 * @exception ModelException
		 * 				if called without calling TreeManager::beginModification() first or if calls to
		 * 				TreeManager::undo() or TreeManager::redo() have already been made.
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
		 * 				if called without calling TreeManager::beginModification() first or if a call to
		 * 				TreeManager::pushCommandOnUndoStack() has already been made within this modification block.
		 */
		void redo();

		/**
		 * Returns whether \a node is owned by a command in the undo stack.
		 *
		 * If \a node is only owned by \a excludeCommand this method returns false.
		 */
		bool isOwnedByUndoStack(const Node* node, const NodeOwningCommand* excludeCommand) const;

		/**
		 * Saves the tree in persistent store of the tree manager.
		 *
		 * If this manager does not have a current store, the store argument must not be NULL. In that case the provided
		 * store will become the current store of the manager.
		 *
		 * @param store
		 * 				The persistent store where the tree should be saved. If this is NULL the current store will be
		 * 				used. The current store is the one from which the tree was loaded. If this is not NULL it will
		 * 				be used instead of the current store.
		 */
		void save(PersistentStore* store = nullptr);

		/**
		 * Loads the manager's tree from the specified persistent store. The provided store will become the current
		 * store of the manager.
		 *
		 * If this manager already has a root node, this method does nothing.
		 *
		 * @param store
		 * 				The persistent store where the manager's tree should be loaded from.
		 *
		 * @param name
		 * 				The tree name in the persistent store.
		 *
		 * @param loadPartially
		 * 				Whether the tree should only be partially loaded. The intention is that a partially loaded tree
		 * 				is read only and should only contain the publicly visible part of the tree. Typically this means
		 * 				excluding method bodies from loading.
		 */
		void load(PersistentStore* store, const QString& name, bool loadPartially);

		/**
		 * Returns the current store for this manager.
		 *
		 * The current store is the store used to load the manager's tree. If the tree was created rather than loaded and
		 * has not been saved to a persistent store yet this method will return NULL.
		 */
		PersistentStore* store();

		/**
		 * Adds \a node to the list of modified nodes during an edit operation or immediately emits a nodesModified
		 * signal with the specified node if no modification to the tree is ongoing.
		 *
		 * For usual edit operations that alter the tree, you do not need to explicitly call this method. It is useful
		 * for cases where only a caching property of a node has changed which might lead to changes in its visualization.
		 *
		 * For example this method can be used when resolving references to notify visualizations of changes to the
		 * reference targets.
		 */
		void notifyNodeChange(Node* node);

		/**
		 * Causes a nameModified signal to be emitted.
		 *
		 * @param node
		 * 				The node that represents the name that has changed
		 * @param oldName
		 * 				What was the old name of the node
		 */
		void emitNameModified(NameText* node, const QString &oldName);

		/**
		 * Returns whether this tree has been partially loaded (as a library).
		 */
		bool isPartiallyLoaded() const;

		const NodeIdMap& nodeIdMap() const;
		NodeIdMap& nodeIdMap();
		const NodeIdMap& nodeIdMapForUndoStack() const;
		NodeIdMap& nodeIdMapForUndoStack();

		void cleanupDestroyedNode(Node* node);

	Q_SIGNALS:
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
		 * @param modifiedNodes
		 * 				A set of all nodes modified within this block. These nodes might include nodes in \a removedNodes
		 *
		 * @param removedNodes
		 * 				A set of all nodes that have been removed within this block. These nodes might include nodes in
		 * 				\a modifiedNodes. The set of reported nodes is all nodes that have been removed, including children
		 * 				of removed nodes.
		 */
		void nodesModified(QSet<Node*> modifiedNodes, QSet<Node*> removedNodes);

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

	private:

		/**
		 * Returns whether \a node is transitively owned by \a cmd.
		 *
		 * If \a node is only owned by \a excludeCommand this method returns false.
		 */
		static bool isOwnedByCommand(const Node* node, const UndoCommand* cmd, const NodeOwningCommand* excludeCommand);

		/**
		 * The name of this manager's tree. This name will be used to save the tree in the persistent store.
		 */
		QString name_;

		/**
		 * Can be used to name the revision the manager handles. This name will be used in the diff to
		 * differentiate between the old and new version managers;
		 */
		QString revisionName_;

		/**
		 * The root node for this manager's tree
		 */
		Node* root_{};

		/**
		 * The command stack that holds the undo history
		 */
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
		 * A set of all nodes which might have been modified as part of the last modification operation. This is only
		 * used to signal to anyone who is interested in monitoring changes.
		 */
		QSet<Node*> modifiedTargets;

		/**
		 * A set of all nodes which have been removed from the tree as part of the last modification operation. This is
		 * only used to signal to anyone who is interested in monitoring changes.
		 */
		QSet<Node*> removedTargets_;

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
		 * The persistent store where the manager's tree is currently stored.
		 *
		 * This is used in calls to Node::loadFully when a partially loaded node needs to load its entire contents.
		 * It can also be used by other stores when the tree needs to be saved to a different location.
		 */
		PersistentStore* store_{};

		/**
		 * Indicates whether this tree has been partially loaded (as a library) from a store.
		 */
		bool partiallyLoaded_{};

		NodeIdMap nodeIdMap_;
		/**
		 * We need a separate map for the undo stack in order to support cut and move operations.
		 */
		NodeIdMap nodeIdMapForUndoStack_;
};

inline bool TreeManager::isBeingModified() const { return modificationInProgress; }
inline Node* TreeManager::modificationTarget() const { return currentModificationTarget; }
inline NodeReadWriteLock* TreeManager::rootLock() { return &rootLock_; }

inline Node* TreeManager::root(){ return root_; }
inline QString TreeManager::name() { return name_; }
inline void TreeManager::setName(const QString& name) { name_ = name; }

inline QString TreeManager::revisionName() { return revisionName_; }
inline void TreeManager::setRevisionName(const QString& revisionName) { revisionName_ = revisionName; }

inline PersistentStore* TreeManager::store() { return store_; }

inline bool TreeManager::isPartiallyLoaded() const { return partiallyLoaded_; }
inline const NodeIdMap& TreeManager::nodeIdMap() const { return nodeIdMap_; }
inline NodeIdMap& TreeManager::nodeIdMap() { return nodeIdMap_; }
inline const NodeIdMap& TreeManager::nodeIdMapForUndoStack() const { return nodeIdMapForUndoStack_; }
inline NodeIdMap& TreeManager::nodeIdMapForUndoStack() { return nodeIdMapForUndoStack_; }
}
