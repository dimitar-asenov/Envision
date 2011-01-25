/***********************************************************************************************************************
 * Text.cpp
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Text.h"
#include "commands/FieldSet.h"
#include "ModelException.h"

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Text)

Text::Text(Node *parent, Model* model) :
	Node(parent, model)
{
}

Text::Text(Node *parent, NodeIdType id, PersistentStore &store, bool) :
	Node(parent, id)
{
	text = store.loadStringValue();
}

const QString& Text::get() const
{
	return text;
}

Text::operator const QString&() const
{
	return text;
}

void Text::set(const QString &newText)
{
	execute(getSetCommand(newText));
}

void Text::save(PersistentStore &store) const
{
	store.saveStringValue(text);
}

void Text::load(PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw ModelException("Trying to load a Text node from an incompatible node type " + store.currentNodeType());

	set(store.loadStringValue());
}

FieldSet<QString>* Text::getSetCommand(const QString& newText)
{
	return new FieldSet<QString> (this, text, newText);
}

}
