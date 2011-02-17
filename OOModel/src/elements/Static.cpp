/***********************************************************************************************************************
 * Static.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/Static.h"
#include "ModelBase/headers/commands/FieldSet.h"
#include "OOModelException.h"

namespace OOModel {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Static)

Static::Static(Model::Node *parent, Model::Model* model) :
	Node(parent, model), stat(INSTANCE_VARIABLE)
{
}

Static::Static(Model::Node *parent, Model::NodeIdType id, Model::PersistentStore &store, bool) :
	Node(parent, id)
{
	stat = fromInt( store.loadIntValue() );
}

void Static::set(const StaticType& value)
{
	execute(new Model::FieldSet<StaticType> (this, stat, value));
}

void Static::save(Model::PersistentStore &store) const
{
	store.saveIntValue(stat);
}

void Static::load(Model::PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw OOModelException("Trying to load a Static node from an incompatible node type " + store.currentNodeType());

	set(fromInt(store.loadIntValue()));
}

Static::StaticType Static::fromInt(int val)
{
	switch (val)
	{
		case INSTANCE_VARIABLE:
			return INSTANCE_VARIABLE;
			break;
		case CLASS_VARIABLE:
			return CLASS_VARIABLE;
			break;
		default:
			throw OOModelException("Trying to a load a static node from an invalid store location");
	}
}

}
