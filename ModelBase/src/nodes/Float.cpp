/***********************************************************************************************************************
 * Float.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Float.h"
#include "commands/FieldSet.h"
#include "ModelException.h"
#include "nodes/TypedListDefinition.h"

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Float)

Float::Float(Node *parent, Model* model) :
	Node(parent, model), value(0.0)
{
}

Float::Float(Node *parent, NodeIdType id, PersistentStore &store, bool) :
	Node(parent, id)
{
	value = store.loadDoubleValue();
}

void Float::set(const double& newValue)
{
	execute(new FieldSet<double> (this, value, newValue));
}

void Float::save(PersistentStore &store) const
{
	store.saveDoubleValue(value);
}

void Float::load(PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw ModelException("Trying to load a Float node from an incompatible node type " + store.currentNodeType());

	set(store.loadDoubleValue());
}

template class TypedList<Float>;

}
