/***********************************************************************************************************************
 * SystemClipboard.cpp
 *
 *  Created on: Jan 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "SystemClipboard.h"
#include "FilePersistenceException.h"

#include "ModelBase/headers/Model.h"
#include "ModelBase/headers/persistence/PersistedNode.h"
#include "ModelBase/headers/persistence/PersistedValue.h"

#include "Core/headers/global.h"

#include <QtGui/QClipboard>
#include <QtGui/QApplication>

using namespace Model;

namespace FilePersistence {

static const QString CLIPBOARD_TAG = "clipboard";

SystemClipboard::SystemClipboard() :
	xml(NULL), numNodes_(0)
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

void SystemClipboard::saveNode(const Node *node, const QString &name, bool)
{
	xml->beginSaveChildNode(node->getTypeName());
	xml->setName(name);
	xml->setPartialHint(false);

	node->save(*this);

	if ( !node->isFullyLoaded() )
	{
		// Persist sub nodes which have not been loaded. Each subnode has a unique name. If a node was previously
		// persisted and was not saved so far, we will read it back from the persistent store and store it again.

		// Get a list of sub nodes which have already been persisted.
		QStringList persistedChildren = xml->getChildrenNames();

		PersistedNode* persisted = node->getModel()->store()->loadCompleteNodeSubtree(node->getModel()->name(), node->persistentUnitId(), node->getId());

		if (!persisted) throw FilePersistenceException("Could not load node subtree from old persistent store.");
		PersistedValue< QList<PersistedNode*> >* composite = dynamic_cast<PersistedValue< QList<PersistedNode*> >* > (persisted);
		if (!composite) throw FilePersistenceException("Partial loading of Value-type nodes (string, int, double) is not supported.");

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
				PersistedValue< QList<PersistedNode*> >* composite =  dynamic_cast< PersistedValue< QList<PersistedNode*> >* > (node);
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

QList<LoadedNode> SystemClipboard::loadAllSubNodes(Node* parent)
{
	QList<LoadedNode> result;

	if ( xml->hasChildren() )
	{
		xml->goToFirstChild();
		while ( true )
		{
			LoadedNode ln = loadNode(parent);
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
	if (!xml->hasChild(name)) return NULL;

	xml->beginLoadChildNode(name);
	LoadedNode ln = loadNode(parent);
	xml->endLoadChildNode();

	return ln.node;
}

LoadedNode SystemClipboard::loadNode(Node* parent)
{
	if (!parent) throw FilePersistenceException("Can not load a not from clipboard without a parent.");

	LoadedNode node;
	node.name = xml->getName();
	node.node = Node::createNewNode(xml->getType(), parent, parent->getModel()->generateNextId(), *this, false);

	return node;
}

QList<LoadedNode> SystemClipboard::loadPartialNode(Node*)
{
	throw FilePersistenceException("The loadPartialNode(...) method is not supported in the SystemClipboard store. This might indicate that an object only partially loaded itself, ignoring the provided partial hint.");
}

PersistedNode* SystemClipboard::loadCompleteNodeSubtree(const QString&, NodeIdType, NodeIdType)
{
	throw FilePersistenceException("The loadCompleteNodeSubtree(...) method is not supported in the SystemClipboard store. This might indicate that an object only partially loaded itself, ignoring the provided partial hint.");
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
			xml->beginSaveChildNode(nodes[i]->getTypeName());
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

QString SystemClipboard::type() const
{
	return xml->getType();
}

Node* SystemClipboard::create(::Model::Model* model, Node* parent)
{
	Node* node = Node::createNewNode(xml->getType(), parent, model->generateNextId(), *this, false);
	return node;
}

}
