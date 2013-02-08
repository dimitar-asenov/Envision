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

#include "FileStore.h"
#include "FilePersistenceException.h"

#include "ModelBase/src/model/Model.h"
#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/ModelException.h"
#include "ModelBase/src/persistence/PersistedNode.h"
#include "ModelBase/src/persistence/PersistedValue.h"

namespace FilePersistence {

const char* XML_NEWUNIT_NODE_TAG = "persistencenewunit";
const QString FileStore::NULL_STRING = "____NULL____";

FileStore::FileStore() : // TODO the Envision folder should be taken from the environment not hardcoded.
	baseFolder(QDir::home().path() + QDir::toNativeSeparators("/Envision/projects")), working(false), xml(nullptr)
{
}

FileStore::~FileStore()
{
}

void FileStore::setBaseFolder(const QString& path)
{
	baseFolder = path;
}

QString FileStore::getPersistenceUnitName(const Model::Node *node)
{
	NodeIdMap::NodeIdType persistenceUnitId = ids.getId( node->persistentUnitNode() );

	QString name;
	if (persistenceUnitId == 0) name = node->model()->name();
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

		saveNewPersistenceUnit(model->root(), name, false);
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

void FileStore::saveReferenceValue(const QString &name, const Model::Node* target)
{
	checkIsWorking();
	QString targetString = target ? QString::number(ids.getId(target)) : NULL_STRING;
	QString nameString = name.isNull() ? NULL_STRING : name;
	xml->saveStringValue(targetString + ":" + nameString);
}

void FileStore::saveNewPersistenceUnit(const Model::Node *node, const QString &name, bool partialLoadHint)
{
	checkIsWorking();

	XMLModel* oldXML = xml;

	if ( node->parent() ) // If this is not the root node, then we should put a reference to this node
	{
		xml->beginSaveChildNode(XML_NEWUNIT_NODE_TAG);
		xml->setName(name);
		xml->saveStringValue(QString::number(ids.getId(node)));
		xml->endSaveChildNode();
	}

	xml = new XMLModel();

	if (oldXML == nullptr)
	{
		// If this is the root node save the model information
		xml->beginSaveChildNode("model");
		saveNodeDirectly(node, name, partialLoadHint);
		xml->setNextId(ids.getNextId());
		xml->endSaveChildNode();
	}
	else saveNodeDirectly(node, name, partialLoadHint);

	QString filename;
	if ( oldXML == nullptr ) filename = name; // This is the root of the model, save the file name
	else
		filename = QString::number( ids.getId(node)); // This is not the root, so save by id

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
	xml->beginSaveChildNode(node->typeName());
	xml->setName(name);
	xml->setId(ids.getId(node));
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

		if ( persisted->goToElement(ids.getId(node)) == false )
			throw FilePersistenceException("Could not find the persistent unit for partial node with id " + QString::number( ids.getId(node)));
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

Model::Node* FileStore::loadModel(Model::Model*, const QString &name)
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
		ids.setNextId(xml->getNextId());
		xml->goToFirstChild();
		ln =  loadNode(nullptr);

		// Initialize all references
		for (auto p : uninitializedReferences)
		{
			bool ok = false;
			NodeIdMap::NodeIdType id = p.second.toInt(&ok);
			if (!ok) throw FilePersistenceException("Incorrect id format for reference target " + p.second);


			Model::Node* target = const_cast<Model::Node*> (ids.getNodeForId(id));
			if (!target) throw FilePersistenceException("A reference is pointing to an unloaded node " + p.second);

			setReferenceTargetr(p.first, target);
		}

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
	node.node = Model::Node::createNewNode(xml->getType(), parent, *this, xml->getPartialHint());
	ids.setId( node.node, xml->getId() ); // Record id
	return node;
}

Model::Node* FileStore::loadSubNode(Model::Node* parent, const QString& name)
{
	checkIsWorking();

	if (!xml->hasChild(name)) return nullptr;
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
		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + partialNode->model()->name());
		if ( !modelDir.exists() ) throw FilePersistenceException("Can not find root node folder " + modelDir.path());

		QString filename = getPersistenceUnitName(partialNode);
		xml = new XMLModel(modelDir.absoluteFilePath(filename));

		// Search through the content in order to find the requested node id.
		if (!xml->goToElement(ids.getId(partialNode)) )
			throw FilePersistenceException("Could not find the persisted data for partial node with id " + QString::number( ids.getId(partialNode)));

		// Load all subnodes and return them
		result = loadAllSubNodes(nullptr); // Pass a null parent as we do not want these nodes to be immediately assigned as children to the parent node.

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

QString FileStore::currentNodeType() const
{
	checkIsWorking();

	return xml->getType();
}

Model::PersistedNode* FileStore::loadCompleteNodeSubtree(const QString& modelName, const Model::Node* node)
{
	storeAccess.lock();
	working = true;

	Model::PersistedNode* result = nullptr;

	NodeIdMap::NodeIdType nodeId = 0;
	NodeIdMap::NodeIdType persistenceUnitId = 0;
	if (node)
	{
		nodeId = ids.getId(node);
		persistenceUnitId = ids.getId( node->persistentUnitNode() );
	}

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

	Model::PersistedNode* result = nullptr;

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

QString FileStore::loadReferenceValue(Model::Reference* r)
{
	checkIsWorking();
	QStringList ref = xml->loadStringValue().split(":");
	QString target = ref.first() == NULL_STRING ? QString() : ref.first();
	QString name = ref.last() == NULL_STRING ? QString() : ref.last();

	if (!target.isNull()) uninitializedReferences.append(qMakePair(r, target));

	return name;
}

void FileStore::checkIsWorking() const
{
	if ( !working ) throw FilePersistenceException("Performing an illegal file persistence operation.");
}
}
