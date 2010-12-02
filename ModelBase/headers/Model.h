/***********************************************************************************************************************
 * Model.h
 *
 *  Created on: Nov 8, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODEL_H_
#define MODEL_H_

#include "modelbase_api.h"
#include "nodes/Node.h"
#include "NodeReadWriteLock.h"

#include <QtGui/QUndoStack>
#include <QtCore/QString>
#include <QtCore/QReadWriteLock>
#include <QtCore/QVector>

namespace Model {

class UndoCommand;

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
 *    - Access control to nodes in a concurrent setting
 *    - Undo/redo functionality
 *    - Notifications of model change
 */
class MODELBASE_API Model: public QObject
{
	Q_OBJECT
		// The moc compiler is used only to support signals and slots.

	private:

		/** The root node for this model */
		Node* root;

		/** The command stack that hold the undo history */
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
		NodeReadWriteLock rootLock;

		/**
		 * The test message associated with the current modification operation. This text is only used to describe the
		 * undo operation that is currently performed.
		 */
		QString modificationText;

		/**
		 * The node that is the top-most ancestor of all other nodes that are currently being modification. Only nodes in
		 * the same access unit and below this node (including the node itself) may be modified.
		 */
		Node* currentModificationTarget;

		/**
		 * The lock of the access unit that is responsible for the current modification target node. Each node has exactly
		 * one access unit corresponding to it. A writer must acquire this lock before any modification can occur. The
		 * currently acquired lock is stored here so that each write operation can be checked to assure that no write
		 * occurs outside of the access unit.
		 */
		NodeReadWriteLock* currentModificationLock;

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
		bool pushedNewCommandsOnTheStack;

		/**
		 * Indicates if undo() or redo() were called during the last modification operation.
		 *
		 * A modification operation can do two things:
		 * 	- Add new commands on the stack
		 * 	- Call undo/redo
		 *
		 * The two can not be mixed in the same operation and this flag is used to control this.
		 */
		bool performedUndoRedo;

		/**
		 * This flag indicates if a modification is currently in progress. Commands can be pushed on the undo stack and
		 * executed only if this is true.
		 */
		bool modificationInProgress;

		/**
		 * The id of the next node that will be created. Each node in the tree must have a unique id. When a new node is
		 * constructed its id takes this value and nextId is incremented.
		 */
		NodeIdType nextId;

		/**
		 * A list of all Model objects that are currently instantiated. This is used to find the Model corresponding to a
		 * particular root object.
		 */
		static QList<Model*> loadedModels;

	public:
		/**
		 * Constructs a new Model.
		 *
		 * The undo history limit is set to 100 operations.
		 */
		Model();

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
		NodeReadWriteLock* getRootLock();

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
		 *
		 * If the root node has not been created yet, this method will return NULL.
		 */
		Node* getRoot();

		/**
		 * Generates a new unique Node id.
		 *
		 * This is used by the Node class to initialize newly created nodes.
		 *
		 * @exception ModelException
		 * 				if called after the root node has been constructed without calling Model::beginModification() first
		 */
		NodeIdType generateNextId();

		/**
		 * Creates a new root node with the specified type.
		 *
		 * If a root node already exists this method will not do anything. If a new root is created the rootCreated signal
		 * will be emitted.
		 *
		 * This method must be called outside of a modification block. This action can not be undone.
		 *
		 * @param typeName
		 * 				A string that specifies what type should the root node have.
		 *
		 */
		Node* createRoot(const QString &typeName);

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
		 * Saves the current model tree in the specified persistent store.
		 *
		 * @param store
		 * 				The persistent store where the model should be saved.
		 */
		void save(PersistentStore& store);

		/**
		 * Returns the model object that has as its root node the node indicated.
		 */
		static Model* getModel(Node* root);

		/**
		 * Causes a nameModified signal to be emitted.
		 *
		 * @param node
		 * 				Which node's name has changed
		 * @param oldNmae
		 * 				What was the old name of the node
		 */
		void emitNameModified(Node* node, const QString &oldName);

	signals:
		/**
		 * Emitted when a new rootNode was created or loaded
		 *
		 * @param root
		 * 				A pointer to the new root node
		 */
		void rootCreated(Node* root);

		/**
		 * Emitted at the end of a modification block.
		 *
		 * @param nodes
		 * 				A list of all modification targets for this operation.
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

};

}

#endif /* MODEL_H_ */
