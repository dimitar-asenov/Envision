/***********************************************************************************************************************
 * Boolean.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Boolean.h"
#include "commands/FieldSet.h"
#include "ModelException.h"

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Boolean)

Boolean::Boolean(Node *parent, Model* model) :
	Node(parent, model), value(false)
{
}

Boolean::Boolean(Node *parent, NodeIdType id, PersistentStore &store, bool) :
	Node(parent, id)
{
	value = store.loadIntValue() != 0;
}

void Boolean::set(const bool& newVal)
{
	execute(new FieldSet<bool> (this, value, newVal));
}

void Boolean::save(PersistentStore &store) const
{
	store.saveIntValue( value ? 1 : 0);
}

void Boolean::load(PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw ModelException("Trying to load an Boolean node from an incompatible node type " + store.currentNodeType());

	bool val = store.loadIntValue();
	set(val);
}

}
