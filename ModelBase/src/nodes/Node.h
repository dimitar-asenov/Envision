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
#include "../persistence/PersistentStore.h"
#include "Core/src/InitializationRegistry.h"
#include "Core/src/reflect/Reflect.h"

namespace Model {

class Model;
class UndoCommand;
class NodeReadWriteLock;

/**
 * The Node class is the foundation element in the model tree in Envision. An application in Envision is a collection of
 * objects of type Node (and derived) that are managed by a single Model object. Nodes are combined together in a tree
 * structure. This class defines the minimal interface of each node and implements some service functions.
 *
 * Each class that derives from Node must have at least two constructors which need to be registered before that class
 * can be used. This is achieved using the static methods 'registerNodeType()', 'createNewNode()' and
 * 'createNodeFromPersistence()'.
 *
 * Derived classes must implement the 'save()' and 'load()' methods that specify how the node is stored in a persistent
 * store.
 *
 * A new class that derives from Node must define many standard methods and constructors. Many macros have been defined
 * that greatly simplify this task. See 'nodeMacros.h' for more details.
 *
 * A node in the tree has a revision number. Each time a node is created or loaded from a persistent store this number
 * is reset to 0. Any modification to the node causes the revision to be incremented. This can be then used by
 * visualizations or other plug-ins to track the evolution of node and update it necessary. A node's revision number
 * will also increase when any of its descendants is modified, added or removed.
 *
 * A node that supports partial loading, should reimplement the 'loadFully()' method to enable this functionality.
 *
 * The functionality offered by the Node class includes:
 * - Navigation routines between parents and children.
 * - Navigation routines between different subtrees.
 * - Revision control.
 * - Persistent store interaction.
 * - Meta information.
 */
class MODELBASE_API Node
{
	public:

		/**
		 * This function pointer type is used to register a constructor for nodes.
		 *
		 * @param parent
		 * 				The parent node of the newly created node. This may be 'nullptr'.
		 */
		typedef Node* (*NodeConstructor)(Node* parent);

		/**
		 * This function pointer type is used to register the constructor for nodes that are being loaded from a
		 * persistent store.
		 *
		 * @param parent
		 * 				The parent node of the newly created node. This can be NULL to indicate that this node is the root
		 * 				node for the model. Otherwise it should be non-NULL.
		 * @param id
		 * 				The id of the node from the persistent store.
		 * @param store
		 * 				The store that is contains this node and its subtree. This will be used to load the children of
		 * 				the node.
		 * @param partialLoadHint
		 * 				A flag that hints whether this node should be fully or partially loaded. The constructor of the
		 * 				node is allowed to ignore this flag.
		 */
		typedef Node* (*NodePersistenceConstructor)(Node *parent, PersistentStore &store, bool partialLoadHint);


		/**
		 * Constructs a new node with the specified parent.
		 *
		 * @param parent
		 * 				The parent of this node. This may be 'nullptr'.
		 *
		 * 				If the parent is not null, then the model associated with the parent will also be the model for
		 * 				this node. If the parent is nullptr, then this node will not be associated with a model initially.
		 * 				It can later be added to an existing model.
		 */
		Node(Node* parent = nullptr);

		virtual ~Node();

		/**
		 * Returns the model managing the tree of the current Node.
		 *
		 * Calling this method during the creation of the root Node will return a nullptr.
		 */
		Model* model() const;

		/**
		 * Returns the root node of tree where of this node.
		 */
		Node* root() const;

		/**
		 * Returns the parent of this Node or NULL if this is the root.
		 */
		Node* parent() const;

		/**
		 * Sets the parent of this Node.
		 */
		void setParent(Node* parent);

		/**
		 * Returns a list of all child nodes.
		 *
		 * Reimplement this method in derived classes that have children. The default implementation returns an empty
		 * list.
		 */
		virtual QList<Node*> children();

		/**
		 * Returns true if this node defines a symbol and false otherwise.
		 *
		 * The default implementaion returns false. Reimplement this method and symbolName() in derived classes that
		 * define symbols.
		 */
		virtual bool definesSymbol() const;

		/**
		 * Returns the name of the symbol defined by this node.
		 *
		 * The default implementaion returns a null QString value. Reimplement this method and definesSymbol() in derived
		 * classes that define symbols.
		 */
		virtual const QString& symbolName() const;

		enum FindSymbolMode {
			SEARCH_UP,	/**< Looks for symbols within the specified scope and enclosing scopes. Depending on the source,
									symbols in the current scope which come after the source will not be considered. This is the
			 	 	 	 	 	 	case e.g. with searches for local variable declarations in a method: only variables before
			 	 	 	 	 	 	the source node should be considered. */
			SEARCH_DOWN /**< Looks for symbols inside the specified scope or subscopes. This is used for symbols that are
			 	 	 	 	 	 	requested in a specific context (typically after a '.') e.g. "list.sort()"*/
		};

		/**
		 * Returns a list of all nodes which define a symbol with a name matching \a symbolExp in the scope of this node.
		 *
		 * The \a source Node specifies what node should be used as a reference when determining what symbols are visible.
		 *
		 * The \a mode specifies what search to perform.
		 *
		 * If \a exhaustAllScopes is false, the search will halt as soon as symbols are found within a scope. This is
		 * useful when resolving links and it is important to find the "nearest" symbols that match with respect to scope.
		 * If \a exhaustAllScopes is true, the search will explore all scopes in corresponding to the provided mode. This
		 * is useful when \a symbolExp can match multiple symbols with different names (for example during auto completion
		 * list build up).
		 *
		 * The default implementation returns a list with only the current node in it, in case the node defines the
		 * requested symbol. Otherwise if \a mode is FindSymbolMode::SEARCH_UP, the implementation of the parent node is
		 * called.
		 *
		 * Reimplement this method in derived classes to specify fine grained behavior and operation for search modes
		 * other than FindSymbolMode::SEARCH_UP
		 */
		virtual QList<Node*> findSymbols(const QRegExp& symbolExp, Node* source, FindSymbolMode mode,
				bool exhaustAllScopes);

		/**
		 * \overload
		 *
		 * This is equivalent to:
		 * \code
		 * findSymbols(QRegExp(symbol, Qt::CaseSensitive, QRegExp::FixedString), source, mode)
		 * \endcode
		 */
		QList<Node*> findSymbols(const QString& symbol, Node* source, FindSymbolMode mode,
				bool exhaustAllScopes);

		/**
		 * Returns the revision of this node.
		 */
		int revision() const;

		/**
		 * Increments the revision of this node by 1.
		 */
		void incrementRevision();

		/**
		 * Increments the revision of this node by the specified amount.
		 */
		void addToRevision(int valueToAdd);

		/**
		 * Returns true if this node is fully loaded and false if it is only partially loaded. The default implementation
		 * always returns true.
		 *
		 * Reimplement this method and loadFully() in derived classes that support partial loading.
		 */
		bool isFullyLoaded() const;

		/**
		 * Returns the lock used to control access to this node.
		 *
		 * If a node should represent a new access unit, this method should be overriden to return a lock associated with
		 * the node.
		 *
		 * The default implementation just asks for the lock of the parent, or if the parent is NULL for the root lock of
		 * the model.
		 */
		virtual NodeReadWriteLock* accessLock() const;

		/**
		 * Returns the lowest common ancestor of this node and other.
		 */
		Node* lowestCommonAncestor(Node* other);

		/**
		 * Returns true of this node is an Ancestor of other and false otherwise.
		 */
		bool isAncestorOf(const Node* other) const;

		/**
		 * Executes the specified command and pushes it on the undo stack.
		 *
		 * This method will fail with an exception if the current thread does not hold the lock for this node's access
		 * unit.
		 *
		 * This method also increments the revision of the node by 1.
		 */
		void execute(UndoCommand *command);

		/**
		 * Saves the current node to a persistent store.
		 *
		 * This method should contain calls to the store object that persist the subtree of this node.
		 *
		 *
		 * @param store
		 * 				The persistent store to use
		 *
		 */
		virtual void save(PersistentStore &store) const = 0;

		/**
		 * Reloads this node from a persistent store.
		 *
		 * This method is called at any point after this node has been created in order to reinitialize its contents from
		 * the persistent store. Any existing subtree should be gracefully detached. This means that subtrees should not
		 * be deleted, but simply detached from the current node in a reversible way using commands that can be undone.
		 */
		virtual void load(PersistentStore &store) = 0;

		/**
		 * Fully loads a partially loaded node from the specified persistent store.
		 *
		 * The default implementation does nothing. Reimplement this method and isFullyLoaded() in derived classes to
		 * enable support for partial loading.
		 */
		virtual void loadFully(PersistentStore &store);

		//TODO In the comment below the part that explains things about the revision is incorrect. The persistence store
		//does not care about this currently. Either change the comment or fix this.
		/**
		 * Returns true if this node should be persisted in a new persistence unit. This is typically a per class value.
		 *
		 * When the user saves the project normally the entire corresponding application tree is saved. To optimize the
		 * amount of data written to disk, it is possible to indicate new persistence units. Whenever a node in the
		 * application is modified only the persistent unit it belongs to, together with all persistence units of its
		 * parents are saved.
		 *
		 * For example, to simulate the typical scenario of saving classes in different files, a node which represents a
		 * class should return true as the result of this method.
		 *
		 * NOTE: The persistence engine will save the ID and last revision of all objects that which are marked as a new
		 * persistence unit. Therefore this option should be used with care. Only node types closer to the root are
		 * suitable for being new persistence units. Nodes closer to the leafs, such as expressions and text values should
		 * not be new persistence units, as this will greatly increase the memory required by the persistence engine.
		 */
		virtual bool isNewPersistenceUnit() const;

		/**
		 * Returns the node that defines the persistence unit for this node.
		 */
		Node* persistentUnitNode() const;

		/**
		 * Returns the name of the type of this Node. This is typically a string identical to the class name.
		 *
		 * This value is used when persisting the node.
		 */
		virtual const QString& typeName() const;

		/**
		 * Returns an integer id of the type of this Node. When a new node class is registered it receives a unique id.
		 * This id can be used later to quickly recognize objects of a specific type.
		 */
		virtual int typeId() const;

		static int typeIdStatic();

		/**
		 * Returns a list of all ids in the type hierarchy of this node (excluding abstract base classes).
		 * \see typeId().
		 *
		 * The most derived id appears at the front of the list.
		 *
		 * The default implementation returns a list containing only the id of Node.
		 */
		virtual QList<int> hierarchyTypeIds() const;

		/**
		 * Registers the constructors of a class derived from Node.
		 *
		 * Each class derived from Node must be registered before it can be used.
		 *
		 * @param type
		 * 				The string name of the class.
		 * @param constructor
		 * 				A function that can construct a new instance of the class at run-time.
		 * @param persistenceconstructor
		 * 				A function that can construct a new instance of the class from a persistent store.
		 */
		static int registerNodeType(const QString &type, const NodeConstructor constructor,
				const NodePersistenceConstructor persistenceconstructor);

		/**
		 * Creates a new node of the specified type.
		 *
		 * @param type
		 * 			The type of the node to create. This must be a type that has been registered before by calling
		 * 			registerNodeType.
		 *
		 * @param parent
		 * 			The parent of the node. This may be 'nullptr'.
		 *
		 */
		static Node* createNewNode(const QString &type, Node* parent = nullptr);

		/**
		 * Creates a new node of the specified type by loading it from a persistent store.
		 *
		 * @param type
		 * 			The type of the node to create. This must be a type that has been registered before by calling
		 * 			registerNodeType.
		 *
		 * @param parent
		 * 			The parent of the node. This can be NULL if this is the root node.
		 *
		 * @param store
		 * 			The persistent store to use to load the subtree of this node.
		 *
		 * @param partialLoadHint
		 * 			Whether this node should only be partially loaded. This is only a hint and a node implementation can
		 * 			ignore this flag.
		 */
		static Node* createNewNode(const QString &type, Node* parent, PersistentStore &store, bool partialLoadHint);

		/**
		 * Returns true if there are already registered constructs for a type with the specified name and false otherwise.
		 */
		static bool isTypeRegistered(const QString &type);


		/**
		 * Returns true is the this node can be modified or false otherwise.
		 *
		 * A node is modifiable if it is part of an access unit which is currently acquired by a writer thread. This is
		 * managed by the 'Model' associated with the node.
		 *
		 * If a node does not have an associated model it is always modifiable.
		 */
		bool isModifyable() const;

		/**
		 * \brief Replaces the child item \a child with the item \a replacement and returns true on success.
		 *
		 * The default implementation always returns false and is suitable for nodes without children. Reimplement this
		 * method if you are implementing a composite node that has replaceable child nodes.
		 */
		virtual bool replaceChild(Node* child, Node* replacement);

		/**
		 * Begins a modification session with the current node as a modification target.
		 *
		 * \a text is the description that will be associated with this modification.
		 *
		 * \sa Model::beginModification()
		 */
		void beginModification(const QString &text = QString());

		/**
		 * Ends a modification session.
		 *
		 * \sa Model::endModification()
		 */
		void endModification();

	protected:

		/**
		 * This flag indicates if the current node is fullyLoaded.
		 *
		 * Derived classes which support this functionality should take care to properly initialize and modify this value
		 * whener the state of the object changes.
		 */
		bool fullyLoaded;

	private:
		Node* parent_;
		int revision_;

		static int numRegisteredTypes_;
		static QMap<QString, NodeConstructor> nodeConstructorRegister;
		static QMap<QString, NodePersistenceConstructor> nodePersistenceConstructorRegister;
};

inline int Node::typeIdStatic()
{
	return 0;
}

/**
 * This is a convenience function that can be used when registering classes derived from Node using registerNodeType().
 */
template<class T> Node* createNewNode(Node* parent)
{
	return new T(parent);
}

/**
 * This is a convenience function that can be used when registering classes derived from Node using registerNodeType().
 */
template<class T> Node* createNodeFromPersistence(Node *parent, PersistentStore &store, bool partialLoadHint)
{
	return new T(parent, store, partialLoadHint);
}

inline QList<Node*> Node::findSymbols(const QString& symbol, Node* source, FindSymbolMode mode, bool exhaustAllScopes)
{
	return findSymbols(QRegExp(symbol, Qt::CaseSensitive, QRegExp::FixedString), source, mode, exhaustAllScopes);
}

}
