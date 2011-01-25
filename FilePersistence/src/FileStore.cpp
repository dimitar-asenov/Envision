/***********************************************************************************************************************
 * FileStore.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "FileStore.h"
#include "FilePersistenceException.h"

#include "ModelBase/headers/Model.h"
#include "ModelBase/headers/nodes/Node.h"
#include "ModelBase/headers/ModelException.h"
#include "ModelBase/headers/persistence/PersistedNode.h"
#include "ModelBase/headers/persistence/PersistedValue.h"

namespace FilePersistence {

const char* XML_NEWUNIT_NODE_TAG = "persistencenewunit";

FileStore::FileStore() : // TODO the Envision folder should be taken from the environment not hardcoded.
	baseFolder(QDir::home().path() + QDir::toNativeSeparators("/Envision/projects")), working(false), xml(NULL)
{
}

FileStore::~FileStore()
{
}

void FileStore::setBaseFolder(const QString& path)
{
	baseFolder = path;
}

QString FileStore::getPersistenceUnitName(const Model::Node *node) const
{
	Model::NodeIdType persistenceUnitId = node->persistentUnitId();

	QString name;
	if (persistenceUnitId == 0) name = node->getModel()->getName();
	else name = QString::number(persistenceUnitId);

	return name;
}

//**********************************************************************************************************************
// Methods from Persistent Store
//**********************************************************************************************************************

void FileStore::saveModel(Model::Model* model, const QString &name)
{
	storeAccess.lock();
	working = true;
	model->beginExclusiveRead();

	try
	{
		if ( !baseFolder.exists(name) )
		{
			if ( !baseFolder.mkpath(name) ) throw FilePersistenceException("Could not create folder " + baseFolder.path() + " for model.");
		}

		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + name);

		if ( !modelDir.exists() ) throw FilePersistenceException("Error opening model folder " + modelDir.path());

		saveNewPersistenceUnit(model->getRoot(), name, false);
	}
	catch (Model::ModelException& e)
	{
		model->endExclusiveRead();
		working = false;
		storeAccess.unlock();
		throw;
	}
	model->endExclusiveRead();
	working = false;
	storeAccess.unlock();
}

void FileStore::saveStringValue(const QString &value)
{
	checkIsWorking();
	xml->saveStringValue(value);
}

void FileStore::saveIntValue(int value)
{
	checkIsWorking();
	xml->saveIntValue(value);
}

void FileStore::saveDoubleValue(double value)
{
	checkIsWorking();
	xml->saveDoubleValue(value);
}

void FileStore::saveNewPersistenceUnit(const Model::Node *node, const QString &name, bool partialLoadHint)
{
	checkIsWorking();

	XMLModel* oldXML = xml;

	if ( node->getParent() ) // If this is not the root node, then we should put a reference to this node
	{
		xml->beginSaveChildNode(XML_NEWUNIT_NODE_TAG);
		xml->setName(name);
		xml->saveStringValue(QString::number(node->getId()));
		xml->endSaveChildNode();
	}

	xml = new XMLModel();

	if (oldXML == NULL)
	{
		// If this is the root node save the model information
		xml->beginSaveChildNode("model");
		xml->setNextId(node->getModel()->maxId());

		saveNodeDirectly(node, name, partialLoadHint);

		xml->endSaveChildNode();
	}
	else saveNodeDirectly(node, name, partialLoadHint);

	QString filename;
	if ( oldXML == NULL ) filename = name; // This is the root of the model, save the file name
	else
		filename = QString::number(node->getId()); // This is not the root, so save by id

	QFile file(modelDir.absoluteFilePath(filename));
	if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) ) throw FilePersistenceException("Could not open file " + file.fileName() + ". " +file.errorString());

	QTextStream ts(&file);
	ts << xml->documentText();
	file.close();

	SAFE_DELETE(xml);
	xml = oldXML;
}

void FileStore::saveNode(const Model::Node *node, const QString &name, bool partialLoadHint)
{
	checkIsWorking();

	if ( node->isNewPersistenceUnit() ) saveNewPersistenceUnit(node, name, partialLoadHint);
	else
		saveNodeDirectly(node, name, partialLoadHint);
}

void FileStore::saveNodeDirectly(const Model::Node *node, const QString &name, bool partialLoadHint)
{
	xml->beginSaveChildNode(node->getTypeName());
	xml->setName(name);
	xml->setId(node->getId());
	xml->setPartialHint(partialLoadHint);

	node->save(*this);

	// TODO this should rely on the loadCompleteNodeSubtree method of the previous store of node instead.
	if ( !node->isFullyLoaded() )
	{
		// Persist sub nodes which have not been loaded. Each subnode has a unique name. If a node was previously
		// persisted and was not saved so far, we will read it back from the persistent store and store it again.

		// Get a list of sub nodes which have already been persisted.
		QStringList persistedChildren = xml->getChildrenNames();

		// Load the old persisted version of this node
		QString filename = getPersistenceUnitName(node);

		// TODO this will fail if the rootDir of the FileStore has changed in the mean time. E.g when saving in a new
		// location.
		XMLModel* persisted = new XMLModel(modelDir.absoluteFilePath(filename));

		if ( persisted->goToElement(node->getId()) == false )
			throw FilePersistenceException("Could not find the persistent unit for partial node with id " + QString::number(node->getId()));
		persisted->goToFirstChild();

		while (true)
		{
			if (!persistedChildren.contains(persisted->getName())) xml->importChildFromXML(persisted->getCurrentElement());

			if (persisted->hasNext()) persisted->loadNext();
			else break;
		}

		SAFE_DELETE(persisted);
	}

	xml->endSaveChildNode();
}

Model::Node* FileStore::loadModel(Model::Model* model, const QString &name)
{
	storeAccess.lock();
	working = true;
	Model::LoadedNode ln;

	try
	{
		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + name);
		if ( !modelDir.exists() ) throw FilePersistenceException("Can not find root node folder " + modelDir.path());

		xml = new XMLModel(modelDir.absoluteFilePath(name));
		xml->goToFirstChild();
		model->setNextId(xml->getNextId());
		xml->goToFirstChild();
		ln =  loadNode(NULL);

		SAFE_DELETE(xml);
	}
	catch (Model::ModelException& e)
	{
		SAFE_DELETE(xml);
		working = false;
		storeAccess.unlock();
		throw;
	}

	working = false;
	storeAccess.unlock();

	return ln.node;
}

Model::LoadedNode FileStore::loadNewPersistenceUnit(const QString& name, Model::Node* parent)
{
	XMLModel* oldXML = xml;
	xml = new XMLModel(modelDir.absoluteFilePath(name));
	xml->goToFirstChild();
	Model::LoadedNode ln =  loadNode(parent);

	SAFE_DELETE(xml);
	xml = oldXML;

	return ln;
}


QList<Model::LoadedNode> FileStore::loadAllSubNodes(Model::Node* parent)
{
	checkIsWorking();

	QList < Model::LoadedNode > result;

	if (xml->hasChildren())
	{
		xml->goToFirstChild();
		while ( true )
		{
			Model::LoadedNode ln;
			if ( xml->getType() == XML_NEWUNIT_NODE_TAG ) ln = loadNewPersistenceUnit(xml->loadStringValue(), parent);
			else ln = loadNode(parent);

			result.append(ln);

			if (xml->hasNext()) xml->loadNext();
			else break;
		}
		xml->goToParent();
	}

	return result;
}

Model::LoadedNode FileStore::loadNode(Model::Node* parent)
{
	Model::LoadedNode node;
	node.name = xml->getName();
	node.node = Model::Node::createNewNode(xml->getType(), parent, xml->getId(), *this, xml->getPartialHint());

	return node;
}

Model::Node* FileStore::loadSubNode(Model::Node* parent, const QString& name)
{
	checkIsWorking();

	if (!xml->hasChild(name)) return NULL;
	xml->beginLoadChildNode(name);

	Model::LoadedNode ln;
	if ( xml->getType() == XML_NEWUNIT_NODE_TAG ) ln = loadNewPersistenceUnit(xml->loadStringValue(), parent);
	else ln = loadNode(parent);

	xml->endLoadChildNode();

	return ln.node;
}

QList<Model::LoadedNode> FileStore::loadPartialNode(Model::Node* partialNode)
{
	storeAccess.lock();
	working = true;
	Model::LoadedNode ln;

	QList < Model::LoadedNode > result;

	try
	{
		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + partialNode->getModel()->getName());
		if ( !modelDir.exists() ) throw FilePersistenceException("Can not find root node folder " + modelDir.path());

		QString filename = getPersistenceUnitName(partialNode);
		xml = new XMLModel(modelDir.absoluteFilePath(filename));

		// Search through the content in order to find the requested node id.
		if (!xml->goToElement(partialNode->getId()) )
			throw FilePersistenceException("Could not find the persisted data for partial node with id " + QString::number(partialNode->getId()));

		// Load all subnodes and return them
		result = loadAllSubNodes(partialNode);

		SAFE_DELETE(xml);
	}
	catch (Model::ModelException& e)
	{
		working = false;
		storeAccess.unlock();
		throw;
	}

	working = false;
	storeAccess.unlock();

	return result;
}

Model::PersistedNode* FileStore::loadCompleteNodeSubtree(const QString& modelName, Model::NodeIdType persistenceUnitId, Model::NodeIdType nodeId)
{
	storeAccess.lock();
	working = true;

	Model::PersistedNode* result = NULL;

	try
	{
		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + modelName);
		if ( !modelDir.exists() ) throw FilePersistenceException("Can not find root node folder " + modelDir.path());

		QString filename;
		if (persistenceUnitId > 0) filename = QString::number(persistenceUnitId);
		else filename = modelName;

		xml = new XMLModel(modelDir.absoluteFilePath(filename));

		// Search through the content in order to find the requested node id.
		if (!xml->goToElement(nodeId) )
			throw FilePersistenceException("Could not find the persisted data for partial node with id " + QString::number(nodeId));

		// Load the node and return it.
		result = loadNodeData();

		SAFE_DELETE(xml);
	}
	catch (Model::ModelException& e)
	{
		working = false;
		storeAccess.unlock();
		throw;
	}

	working = false;
	storeAccess.unlock();

	return result;
}

Model::PersistedNode* FileStore::loadNodeData()
{
	checkIsWorking();

	if (xml->getType() == XML_NEWUNIT_NODE_TAG) return loadPersistentUnitData();

	Model::PersistedNode* result = NULL;

	// Determine the type of the node
	if ( xml->isString() )
	{
		Model::PersistedValue<QString> *val = new Model::PersistedValue<QString>();
		val->set( loadStringValue() );
		result = val;
	}
	else if ( xml->isInteger() )
	{
		Model::PersistedValue<int> *val = new Model::PersistedValue<int>();
		val->set( loadIntValue() );
		result = val;
	}
	else if ( xml->isDouble() )
	{
		Model::PersistedValue<double> *val = new Model::PersistedValue<double>();
		val->set( loadDoubleValue() );
		result = val;
	}
	else
	{
		Model::PersistedValue< QList<Model::PersistedNode*> > *val = new Model::PersistedValue< QList<Model::PersistedNode*> >();
		result = val;

		if (xml->hasChildren())
		{
			xml->goToFirstChild();
			while(true)
			{
				Model::PersistedNode* child;
				child = loadNodeData();

				val->value().append(child);

				if (xml->hasNext()) xml->loadNext();
				else break;
			}
			xml->goToParent();
		}
	}

	result->setId(xml->getId());
	result->setType(xml->getType());
	result->setName(xml->getName());
	result->setPartialHint(xml->getPartialHint());
	result->setNewPersistenceUnit(false);

	return result;
}

Model::PersistedNode* FileStore::loadPersistentUnitData( )
{
	checkIsWorking();

	XMLModel* oldXML = xml;
	xml = new XMLModel(modelDir.absoluteFilePath(oldXML->loadStringValue()));
	xml->goToFirstChild();

	Model::PersistedNode* result = loadNodeData();
	result->setNewPersistenceUnit(true);

	SAFE_DELETE(xml);
	xml = oldXML;

	return result;
}


int FileStore::loadIntValue()
{
	checkIsWorking();
	return xml->loadIntValue();
}

QString FileStore::loadStringValue()
{
	checkIsWorking();
	return xml->loadStringValue();
}

double FileStore::loadDoubleValue()
{
	checkIsWorking();
	return xml->loadDoubleValue();
}

void FileStore::checkIsWorking()
{
	if ( !working ) throw FilePersistenceException("Performing an illegal file persistence operation.");
}
}
