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

namespace Model {

class MODELBASE_API Node
{

	public:
		typedef unsigned long IdType;

	private:
		Node* parent;
		IdType id;
		int revision;

		static IdType nextId;
		static QMutex nextIdAccess;

	public:
		Node(Node* parent);
		Node(Node* parent, IdType id);
		virtual ~Node();

		IdType getId();
		int getRevision();
		void incrementRevision();
		void addToRevision(int valueToAdd);

		/**
		 * Saves the current node to a persistent store.
		 *
		 *
		 * @param store
		 * 				The persistent store to use
		 * @param name
		 * 				The name under which to save this node. This can be QString::null to indicate that the node has no
		 * 				name. If the node has no name it is assumed that is a part of an ordered list.
		 *
		 */
		virtual void save(PersistentStore &store, QString &name) = 0;
		virtual void load(PersistentStore &store) = 0;

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
};

}

#endif /* NODE_H_ */
