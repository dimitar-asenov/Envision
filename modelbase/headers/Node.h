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

class MODELBASE_API Node
{

	public:
		typedef long IdType;
		typedef Node* (*NodeConstructor)(Node* parent);
		//typedef Node* (*NodeConstructorWithId)(Node* parent, IdType id);

	private:
		Node* parent;
		IdType id;
		int revision;

		static IdType nextId;
		static QMutex nextIdAccess;

		static QMap<QString, NodeConstructor> nodeTypeRegister;

	protected:
		bool fullyLoaded;

	public:
		Node(Node* parent);
		Node(Node* parent, IdType id);
		virtual ~Node();

		Node* getParent();
		IdType getId();
		int getRevision();
		void incrementRevision();
		void addToRevision(int valueToAdd);
		bool isFullyLoaded();

		/**
		 * Saves the current node to a persistent store.
		 *
		 *
		 * @param store
		 * 				The persistent store to use
		 *
		 */
		virtual void save(PersistentStore &store) = 0;
		virtual void loadFully(PersistentStore &store) = 0;
		virtual void loadPartially(PersistentStore &store);

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

		virtual const QString& getTypeName() = 0;

		static bool registerNodeType(const QString &type, const NodeConstructor constructor);
		static Node* createNewNode(const QString &type, Node* parent);
};

}

#endif /* NODE_H_ */
