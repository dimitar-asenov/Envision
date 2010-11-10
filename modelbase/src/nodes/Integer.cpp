/***********************************************************************************************************************
 * Integer.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Integer.h"
#include "commands/IntegerSet.h"

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Integer)

Integer::Integer(Node *parent, Model* model) :
	Node(parent, model)
{
}

Integer::Integer(Node *parent, NodeIdType id, PersistentStore &store, bool) :
	Node(parent, id)
{
	integer = store.loadIntValue();
}

int Integer::get() const
{
	return integer;
}

void Integer::set(int value)
{
	execute(new IntegerSet(this, &integer, value));
}

void Integer::save(PersistentStore &store)
{
	store.saveIntValue(integer);
}

}

