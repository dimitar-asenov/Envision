/***********************************************************************************************************************
 * Visibility.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "common/Visibility.h"
#include "ModelBase/headers/commands/FieldSet.h"
#include "OOModelException.h"

namespace OOModel {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Visibility)

Visibility::Visibility(Model::Node *parent, Model::Model* model) :
	Node(parent, model), vis(DEFAULT)
{
}

Visibility::Visibility(Model::Node *parent, Model::NodeIdType id, Model::PersistentStore &store, bool) :
	Node(parent, id)
{
	vis = fromInt( store.loadIntValue() );
}

void Visibility::set(const VisibilityType& value)
{
	execute(new Model::FieldSet<VisibilityType> (this, vis, value));
}

void Visibility::save(Model::PersistentStore &store) const
{
	store.saveIntValue(vis);
}

void Visibility::load(Model::PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw OOModelException("Trying to load a Visibility node from an incompatible node type " + store.currentNodeType());

	set(fromInt(store.loadIntValue()));
}

Visibility::VisibilityType Visibility::fromInt(int val)
{
	switch (val)
	{
		case DEFAULT:
			return DEFAULT;
			break;
		case PUBLIC:
			return PUBLIC;
			break;
		case PRIVATE:
			return PRIVATE;
			break;
		case PROTECTED:
			return PROTECTED;
			break;
		default:
			throw OOModelException("Trying to a load a visibility node from an invalid store location");
	}
}

}
