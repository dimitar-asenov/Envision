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

void Reference::set(Node* target)
{
	set( getSymbolicPathToTarget(target) );
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

Node* Reference::getTargetFromSymbolicPath(const QString &path)
{
	return getTargetFromLocalPath(path);
}

QString Reference::getSymbolicPathToTarget(Node *target)
{
	return getLocalPathToTarget(target);
}

Node* Reference::getTargetFromLocalPath(const QString &path)
{
	QString p = path;
	Node* n = this;

	if ( !p.startsWith("local:") ) return NULL;
	p.remove(0, 6);

	QStringList parts = p.split("/");
	for (QStringList::iterator step = parts.begin(); step != parts.end(); step++)
	{
		if ( step == parts.begin() && step->isEmpty() )
		{
			//This means that the string begins with "/" so start from root
			n = root();
			continue;
		}

		if ( *step == ".." ) n = n->parent();
		else
			n = n->child(*step);

		if ( n == NULL ) return NULL;
	}

	return n;
}

QString Reference::getLocalPathToTarget(Node *target)
{
	if ( target == NULL ) return QString();

	Node* parent = lowestCommonAncestor(target);

	if ( parent == NULL ) return QString();

	// Get the first part of the string which defines how many nodes we should go up the parent chain until reaching the
	// common ancestor
	QString up = "";
	Node* n = this;
	while ( n != parent )
	{
		if ( up.size() > 0 ) up += "/";
		up += "..";
		n = n->parent();
	}

	// Get the names under which each node starting from parent can be used to navigate to the target
	QString down = "";
	n = target;
	while ( n != parent )
	{
		if ( down.size() > 0 ) down.prepend("/");
		down.prepend(n->parent()->childReferenceName(n));
		n = n->parent();
	}

	QString completePath = "local:";
	if ( up.size() > 0 && down.size() > 0 ) completePath += up + "/" + down;
	else
		completePath += up + down;

	return completePath;
}

}
