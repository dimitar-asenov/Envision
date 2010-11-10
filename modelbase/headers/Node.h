/***********************************************************************************************************************
 * Node.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef NODE_H_
#define NODE_H_

#include "modelbase_api.h"
#include "PersistentStore.h"
#include <QString>
#include <QMutex>
#include <QMap>

namespace Model {

class Model;
class UndoCommand;

class MODELBASE_API Node
{

	public:
		typedef Node* (*NodeConstructor)(Node* parent, Model* model);
		typedef Node* (*NodePersistenceConstructor)(Node *parent, NodeIdType id, PersistentStore &store, bool partialLoadHint);

	private:
		Node* parent;
		NodeIdType id;
		int revision;

		static QMap<QString, NodeConstructor> nodeConstructorRegister;
		static QMap<QString, NodePersistenceConstructor> nodePersistenceConstructorRegister;

	protected:
		bool fullyLoaded;

	public:
		Node(Node* parent, Model* model);
		Node(Node* parent, NodeIdType id);
		virtual ~Node();

		Model* getModel();
		Node* getRoot();
		Node* getParent();

		virtual Node* getChild(NodeIdType id);
		NodeIdType getId();
		int getRevision();
		void incrementRevision();
		void addToRevision(int valueToAdd);
		bool isFullyLoaded();

		/**
		 * Executes the specified command and pushes it on the undo stack.
		 */
		void execute(UndoCommand *command);

		/**
		 * Returns the name by which this node can be referenced.
		 *
		 * This name should be unique between all siblings. If the returned value is a null string ( QString.isNull()
		 * returns true ) this means that this node can not be referenced.
		 */
		virtual QString getReferenceName();

		/**
		 * Saves the current node to a persistent store.
		 *
		 *
		 * @param store
		 * 				The persistent store to use
		 *
		 */
		virtual void save(PersistentStore &store) = 0;
		virtual void loadFully(PersistentStore &store);

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
		 * persistence unit. Therefore this options should be used with care. Only node types closer to the root are
		 * suitable for being new persistence units. Nodes closer to the leafs, such as expressions and text values should
		 * not be new persistence units, as this will greatly increase the memory required by the persistence engine.
		 */
		virtual bool isNewPersistenceUnit();

		virtual QString getTypeName() = 0;

		static bool registerNodeType(const QString &type, const NodeConstructor constructor, const NodePersistenceConstructor persistenceconstructor);

		/**
		 * Creates a new node of the specified type.
		 *
		 * @param type
		 * 			The type of the node to create. This must be a type that has been registered before by calling
		 * 			registerNodeType.
		 *
		 * @param parent
		 * 			The parent of the node. This can be NULL if this is the root node.
		 *
		 * @param model
		 * 			The model that this node belongs to. If this value is NULL the model of the parent will be used. The
		 * 			default value is NULL. Note that this should only be set to a value different than NULL in case the
		 * 			parent node does not yet belong to a model. This arises only when the root node is constructed.
		 */
		static Node* createNewNode(const QString &type, Node* parent, Model* model = NULL);
		static Node* createNewNode(const QString &type, Node* parent, NodeIdType id, PersistentStore &store, bool partialLoadHint);
};

}

#endif /* NODE_H_ */
