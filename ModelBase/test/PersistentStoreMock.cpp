/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

/***********************************************************************************************************************
 * PersistentStoreMock.cpp
 *
 *  Created on: Nov 11, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "PersistentStoreMock.h"
#include "nodes/Node.h"
#include "model/Model.h"
#include "ModelException.h"

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
	savedData.append(node->typeName() + "," + name + "," + (partialLoadHint ? "partial," : "full,"));
	node->save(*this);
}

QString PersistentStoreMock::loadReferenceValue(Reference*)
{
	throw ModelException("The Persistent store mock does not support references");
}

void PersistentStoreMock::saveReferenceValue(const QString &, const Node*)
{
	throw ModelException("The Persistent store mock does not support references");
}

Node* PersistentStoreMock::loadModel(Model*, const QString &)
{
	return nullptr;
}

QList<LoadedNode> PersistentStoreMock::loadAllSubNodes(Node* )
{
	return QList<LoadedNode> ();
}

Node* PersistentStoreMock::loadSubNode(Node*, const QString&)
{
	return nullptr;
}

QList<LoadedNode> PersistentStoreMock::loadPartialNode(Node*)
{
	return QList<LoadedNode>();
}

QString PersistentStoreMock::currentNodeType() const
{
	return QString();
}

PersistedNode* PersistentStoreMock::loadCompleteNodeSubtree(const QString&, const Node*)
{
	return nullptr;
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
