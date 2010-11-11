/***********************************************************************************************************************
 * PersistentStoreMock.cpp
 *
 *  Created on: Nov 11, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "PersistentStoreMock.h"
#include "Node.h"

namespace Model {

PersistentStoreMock::PersistentStoreMock()
{
}

PersistentStoreMock::~PersistentStoreMock()
{
}

void PersistentStoreMock::saveStringValue(const QString &value)
{
	savedData.append(value + ",");
}

void PersistentStoreMock::saveIntValue(int value)
{
	savedData.append(QString::number(value) + ",");
}

void PersistentStoreMock::saveFloatValue(double value)
{
	savedData.append(QString::number(value) + ",");
}

void PersistentStoreMock::saveNode(const Node *node, const QString &name, bool partialLoadHint)
{
	savedData.append(node->getTypeName() + "," + name + "," + (partialLoadHint ? "partial," : "full,"));
	node->save(*this);
}

Node* PersistentStoreMock::loadRootNode(const QString &)
{
	return NULL;
}

QList<LoadedNode> PersistentStoreMock::loadAllSubNodes(Node* )
{
	return QList<LoadedNode> ();
}

int PersistentStoreMock::loadIntValue()
{
	return 0;
}

QString PersistentStoreMock::loadStringValue()
{
	return QString();
}

double PersistentStoreMock::loadFloatValue()
{
	return 0.0;
}

const QString& PersistentStoreMock::getSaved() const
{
	return savedData;
}

void PersistentStoreMock::clear()
{
	savedData.clear();
}

}
