/***********************************************************************************************************************
 * Character.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Character.h"
#include "commands/FieldSet.h"
#include "ModelException.h"

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Character)

Character::Character(Node *parent, Model* model) :
	Node(parent, model), value('\0')
{
}

Character::Character(Node *parent, NodeIdType id, PersistentStore &store, bool) :
	Node(parent, id)
{
	QString t = store.loadStringValue();
	if (t.size() != 1) throw ModelException("Creating character node failed. Invalid persistent store data: " + t);

	value = t[0];
}

void Character::set(const QChar& newValue)
{
	execute(new FieldSet<QChar> (this, value, newValue));
}

void Character::save(PersistentStore &store) const
{
	store.saveStringValue(QString(value));
}

void Character::load(PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw ModelException("Trying to load a Character node from an incompatible node type " + store.currentNodeType());

	QString t = store.loadStringValue();
	if (t.size() != 1) throw ModelException("Loading character node failed. Invalid persistent store data: " + t);

	set(t[0]);
}

}
