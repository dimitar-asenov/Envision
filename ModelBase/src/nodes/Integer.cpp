/***********************************************************************************************************************
 * Integer.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Integer.h"
#include "commands/FieldSet.h"
#include "ModelException.h"
#include "nodes/TypedListDefinition.h"

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Integer)

Integer::Integer(Node *parent, Model* model) :
	Node(parent, model), integer(0)
{
}

Integer::Integer(Node *parent, NodeIdType id, PersistentStore &store, bool) :
	Node(parent, id)
{
	integer = store.loadIntValue();
}

void Integer::set(const int& value)
{
	execute(new FieldSet<int> (this, integer, value));
}

void Integer::save(PersistentStore &store) const
{
	store.saveIntValue(integer);
}

void Integer::load(PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw ModelException("Trying to load an Integer node from an incompatible node type " + store.currentNodeType());

	set(store.loadIntValue());
}

template class TypedList<Integer>;
}

