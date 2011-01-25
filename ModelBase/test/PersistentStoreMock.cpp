/***********************************************************************************************************************
 * PersistentStoreMock.cpp
 *
 *  Created on: Nov 11, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "PersistentStoreMock.h"
#include "nodes/Node.h"
#include "Model.h"

namespace Model {

PersistentStoreMock::PersistentStoreMock()
{
}

PersistentStoreMock::~PersistentStoreMock()
{
}

void PersistentStoreMock::saveModel(Model* model, const QString &name)
{
	saveNode(model->root(), name, false);
}

void PersistentStoreMock::saveStringValue(const QString &value)
{
	savedData.append(value + ",");
}

void PersistentStoreMock::saveIntValue(int value)
{
	savedData.append(QString::number(value) + ",");
}

void PersistentStoreMock::saveDoubleValue(double value)
{
	savedData.append(QString::number(value) + ",");
}

void PersistentStoreMock::saveNode(const Node *node, const QString &name, bool partialLoadHint)
{
	savedData.append(node->getTypeName() + "," + name + "," + (partialLoadHint ? "partial," : "full,"));
	node->save(*this);
}

Node* PersistentStoreMock::loadModel(Model*, const QString &)
{
	return NULL;
}

QList<LoadedNode> PersistentStoreMock::loadAllSubNodes(Node* )
{
	return QList<LoadedNode> ();
}

Node* PersistentStoreMock::loadSubNode(Node*, const QString&)
{
	return NULL;
}

QList<LoadedNode> PersistentStoreMock::loadPartialNode(Node*)
{
	return QList<LoadedNode>();
}

PersistedNode* PersistentStoreMock::loadCompleteNodeSubtree(const QString&, NodeIdType, NodeIdType)
{
	return NULL;
}

int PersistentStoreMock::loadIntValue()
{
	return 0;
}

QString PersistentStoreMock::loadStringValue()
{
	return QString();
}

double PersistentStoreMock::loadDoubleValue()
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
