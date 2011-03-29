/***********************************************************************************************************************
 * StorageSpecifier.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "elements/StorageSpecifier.h"
#include "ModelBase/headers/commands/FieldSet.h"
#include "OOModelException.h"

namespace OOModel {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(StorageSpecifier)

StorageSpecifier::StorageSpecifier(Model::Node *parent, Model::Model* model) :
	Node(parent, model), specifier(INSTANCE_VARIABLE)
{
}

StorageSpecifier::StorageSpecifier(Model::Node *parent, Model::NodeIdType id, Model::PersistentStore &store, bool) :
	Node(parent, id)
{
	specifier = fromInt( store.loadIntValue() );
}

void StorageSpecifier::set(const StorageSpecifierTypes& value)
{
	execute(new Model::FieldSet<StorageSpecifierTypes> (this, specifier, value));
}

void StorageSpecifier::save(Model::PersistentStore &store) const
{
	store.saveIntValue(specifier);
}

void StorageSpecifier::load(Model::PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw OOModelException("Trying to load a StorageSpecifier node from an incompatible node type " + store.currentNodeType());

	set(fromInt(store.loadIntValue()));
}

StorageSpecifier::StorageSpecifierTypes StorageSpecifier::fromInt(int val)
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
			throw OOModelException("Trying to a load a StorageSpecifier node from an invalid store location");
	}
}

}
