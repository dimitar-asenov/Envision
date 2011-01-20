/***********************************************************************************************************************
 * PersistedNode.h
 *
 *  Created on: Jan 20, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PERSISTEDNODE_H_
#define PERSISTEDNODE_H_

#include "../modelbase_api.h"
#include "PersistentStore.h"

namespace Model {

class MODELBASE_API PersistedNode
{
	public:
		PersistedNode();
		virtual ~PersistedNode();

		void setId(NodeIdType id);
		void setName(const QString& name);
		void setPartialHint(bool partialHint);
		void setNewPersistenceUnit(bool newPersistenceUnit);

		NodeIdType id() const;
		const QString& name() const;
		bool partialHint() const;
		bool isNewPersistenceUnit() const;

	private:
		NodeIdType id_;
		QString name_;
		bool partialHint_;
		bool isNewPersistenceUnit_;
};

inline void PersistedNode::setId(NodeIdType id) { id_ = id; }
inline void PersistedNode::setName(const QString& name) { name_ = name; }
inline void PersistedNode::setPartialHint(bool partialHint) { partialHint_ = partialHint; }
inline void PersistedNode::setNewPersistenceUnit(bool newPersistenceUnit) { isNewPersistenceUnit_ = newPersistenceUnit; }

inline NodeIdType PersistedNode::id() const { return id_; }
inline const QString& PersistedNode::name() const { return name_; }
inline bool PersistedNode::partialHint() const { return partialHint_; }
inline bool PersistedNode::isNewPersistenceUnit() const { return isNewPersistenceUnit_; }

}

#endif /* PERSISTEDNODE_H_ */
