/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "SystemClipboard.h"
#include "FilePersistenceException.h"

#include "ModelBase/src/model/Model.h"
#include "ModelBase/src/nodes/Reference.h"
#include "ModelBase/src/persistence/PersistedNode.h"
#include "ModelBase/src/persistence/PersistedValue.h"

using namespace Model;

namespace FilePersistence {

static const QString CLIPBOARD_TAG = "clipboard";
const QString SystemClipboard::NULL_STRING = "____NULL____";

SystemClipboard::SystemClipboard() :
	xml(nullptr), numNodes_(0)
{
	readClipboard();
}

SystemClipboard::~SystemClipboard()
{
	SAFE_DELETE(xml);
}

// Methods from Persistent Store
void SystemClipboard::saveModel(::Model::Model* model, const QString &name)
{
	if (model->root())
	{
		SAFE_DELETE(xml);
		xml = new XMLModel();

		xml->beginSaveChildNode(CLIPBOARD_TAG);
		saveNode(model->root(), name, false);
		xml->endSaveChildNode();

		QApplication::clipboard()->setText(xml->documentText());

		SAFE_DELETE(xml);
		numNodes_ = 0;
	}
}

void SystemClipboard::saveStringValue(const QString &value)
{
	xml->saveStringValue(value);
}

void SystemClipboard::saveIntValue(int value)
{
	xml->saveIntValue(value);
}

void SystemClipboard::saveDoubleValue(double value)
{
	xml->saveDoubleValue(value);
}

void SystemClipboard::saveReferenceValue(const QString &name, const Node* target)
{
	QString nameString = name.isNull() ? (target && target->definesSymbol() ? target->symbolName() : NULL_STRING) : name;
	xml->saveStringValue(nameString);
}

void SystemClipboard::saveNode(const Node *node, const QString &name, bool)
{
	xml->beginSaveChildNode(node->typeName());
	xml->setName(name);
	xml->setPartialHint(false);

	node->save(*this);

	if ( !node->isFullyLoaded() )
	{
		// Persist sub nodes which have not been loaded. Each subnode has a unique name. If a node was previously
		// persisted and was not saved so far, we will read it back from the persistent store and store it again.

		// Get a list of sub nodes which have already been persisted.
		QStringList persistedChildren = xml->getChildrenNames();

		PersistedNode* persisted = node->model()->store()->loadCompleteNodeSubtree(node->model()->name(), node);

		if (!persisted) throw FilePersistenceException("Could not load node subtree from old persistent store.");
		auto composite = dynamic_cast<PersistedValue< QList<PersistedNode*> >* > (persisted);
		if (!composite)
			throw FilePersistenceException("Partial loading of Value-type nodes (string, int, double) is not supported.");

		for(int i = 0; i<composite->value().size(); ++i)
		{
			if (!persistedChildren.contains(composite->value()[i]->name())) saveNodeFromOldStore(composite->value()[i]);
		}

		SAFE_DELETE(persisted);
	}

	xml->endSaveChildNode();
}

void SystemClipboard::saveNodeFromOldStore(PersistedNode* node)
{
	xml->beginSaveChildNode( node->type() );
	xml->setName(node->name());
	xml->setPartialHint( false );

	PersistedValue< QString >* string =  dynamic_cast< PersistedValue< QString >* > (node);
	if (string) xml->saveStringValue(string->value());
	else
	{
		PersistedValue< int >* integer =  dynamic_cast< PersistedValue< int >* > (node);
		if (integer) xml->saveIntValue( integer->value() );
		else
		{
			PersistedValue< double >* dbl =  dynamic_cast< PersistedValue< double >* > (node);
			if (dbl) xml->saveDoubleValue(dbl->value());
			else
			{
				auto composite = dynamic_cast< PersistedValue< QList<PersistedNode*> >* > (node);
				if (composite)
				{
					for (int i = 0; i< composite->value().size(); ++i) saveNodeFromOldStore(composite->value()[i]);
				}
				else throw FilePersistenceException("Unknown PersistedNode type.");
			}
		}
	}

	xml->endSaveChildNode();
}

Node* SystemClipboard::loadModel(::Model::Model*, const QString &)
{
	throw FilePersistenceException("The clipboard does not support the loadModel() method.");
}

QList<LoadedNode> SystemClipboard::loadAllSubNodes(Node*)
{
	QList<LoadedNode> result;

	if ( xml->hasChildren() )
	{
		xml->goToFirstChild();
		while ( true )
		{
			LoadedNode ln = loadNode(nullptr);
			result.append(ln);
			if ( xml->hasNext() ) xml->loadNext();
			else break;
		}
		xml->goToParent();
	}

	return result;
}

Node* SystemClipboard::loadSubNode(Node* parent, const QString& name)
{
	if (!xml->hasChild(name)) return nullptr;

	xml->beginLoadChildNode(name);
	LoadedNode ln = loadNode(parent);
	xml->endLoadChildNode();

	return ln.node;
}

LoadedNode SystemClipboard::loadNode(Node* parent)
{
	LoadedNode node;
	node.name = xml->getName();
	node.node = Node::createNewNode(xml->getType(), parent, *this, false);

	return node;
}

QString SystemClipboard::currentNodeType() const
{
	return xml->getType();
}

QList<LoadedNode> SystemClipboard::loadPartialNode(Node*)
{
	throw FilePersistenceException("The loadPartialNode(...) method is not supported in the SystemClipboard store."
			" This might indicate that an object only partially loaded itself, ignoring the provided partial hint.");
}

PersistedNode* SystemClipboard::loadCompleteNodeSubtree(const QString&, const Node*)
{
	throw FilePersistenceException("The loadCompleteNodeSubtree(...) method is not supported in the SystemClipboard"
			" store. This might indicate that an object only partially loaded itself,"
			" ignoring the provided partial hint.");
}

int SystemClipboard::loadIntValue()
{
	return xml->loadIntValue();
}

QString SystemClipboard::loadStringValue()
{
	return xml->loadStringValue();
}

double SystemClipboard::loadDoubleValue()
{
	return xml->loadDoubleValue();
}

QString SystemClipboard::loadReferenceValue(Reference*)
{
	QString name = xml->loadStringValue();
	if (name == NULL_STRING) name = QString();

	return name;
}

// Methods from ClipboardStore
void SystemClipboard::putNode(const Node* node)
{
	QList<const Node*> nodes;
	nodes.append(node);
	putNodes(nodes);
}

void SystemClipboard::putNodes(const QList<const Node*>& nodes)
{
	if (nodes.size() > 0)
	{
		SAFE_DELETE(xml);
		xml = new XMLModel();
		xml->beginSaveChildNode(CLIPBOARD_TAG);

		for(int i = 0; i<nodes.size(); ++i)
		{
			xml->beginSaveChildNode(nodes[i]->typeName());
			xml->setName(QString::number(i));
			xml->setPartialHint(false);

			nodes[i]->save(*this);

			xml->endSaveChildNode();
		}

		xml->endSaveChildNode();
		QApplication::clipboard()->setText(xml->documentText());

		SAFE_DELETE(xml);
		numNodes_ = 0;
	}
}

bool SystemClipboard::readClipboard()
{
	numNodes_ = 0;
	SAFE_DELETE(xml);

	bool clipboardContainsEnvisionData = false;
	if (!QApplication::clipboard()->text().isEmpty())
	{
		bool oldAssertOnThrow = FilePersistenceException::assertOnThrow();
		FilePersistenceException::assertOnThrow() = false;
		try
		{
			xml = new XMLModel();
			xml->setDocumentText(QApplication::clipboard()->text());
			xml->goToRoot();
			numNodes_ = xml->getChildrenNames().size();
			xml->goToFirstChild();
			clipboardContainsEnvisionData = true;
		}
		catch (FilePersistenceException&)
		{
			SAFE_DELETE(xml);
			clipboardContainsEnvisionData = false;
		}
		FilePersistenceException::assertOnThrow() = oldAssertOnThrow;
	}

	return clipboardContainsEnvisionData && numNodes_ > 0;
}

int SystemClipboard::numNodes() const
{
	return numNodes_;
}

bool SystemClipboard::hasNext() const
{
	return xml->hasNext();
}

void SystemClipboard::next()
{
	if (xml->hasNext()) xml->loadNext();
	else throw FilePersistenceException("Could not find next clipboard element.");
}

Node* SystemClipboard::create(::Model::Model*, Node* parent)
{
	Node* node = Node::createNewNode(xml->getType(), parent, *this, false);
	return node;
}

}
