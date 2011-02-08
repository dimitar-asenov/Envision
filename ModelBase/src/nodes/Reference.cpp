/***********************************************************************************************************************
 * Reference.cpp
 *
 *  Created on: Nov 17, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Reference.h"
#include "commands/FieldSet.h"
#include "Model.h"
#include "ModelException.h"

#include <QtCore/QStringList>

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Reference)

Reference::Reference(Node *parent, Model* model) :
	Node(parent, model)
{
}

Reference::Reference(Node *parent, NodeIdType id, PersistentStore &store, bool) :
	Node(parent, id)
{
	path_ = store.loadStringValue();
}

void Reference::set(const QString &new_path)
{
	execute(new FieldSet<QString> (this, path_, new_path));
}

void Reference::save(PersistentStore &store) const
{
	store.saveStringValue(path_);
}

void Reference::load(PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw ModelException("Trying to load a Reference node from an incompatible node type " + store.currentNodeType());

	set(store.loadStringValue());
}

}
