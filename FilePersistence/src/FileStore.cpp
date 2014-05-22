/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

FileStore* FileStore::clone() const
{
	auto fs = new FileStore();
	fs->baseFolder = baseFolder;
	return fs;
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
			if ( !baseFolder.mkpath(name) )
				throw FilePersistenceException("Could not create folder " + baseFolder.path() + " for model.");
		}

		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + name);

		if ( !modelDir.exists() ) throw FilePersistenceException("Error opening model folder " + modelDir.path());

		saveNewPersistenceUnit(model->root(), name);
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

void FileStore::saveNewPersistenceUnit(const Model::Node *node, const QString &name)
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
		saveNodeDirectly(node, name);
		xml->setNextId(ids.getNextId());
		xml->endSaveChildNode();
	}
	else saveNodeDirectly(node, name);

	QString filename;
	if ( oldXML == nullptr ) filename = name; // This is the root of the model, save the file name
	else
		filename = QString::number( ids.getId(node)); // This is not the root, so save by id

	QFile file(modelDir.absoluteFilePath(filename));
	if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) )
		throw FilePersistenceException("Could not open file " + file.fileName() + ". " +file.errorString());

	QTextStream ts(&file);
	ts << xml->documentText();
	file.close();

	SAFE_DELETE(xml);
	xml = oldXML;
}

void FileStore::saveNode(const Model::Node *node, const QString &name)
{
	checkIsWorking();

	if ( node->isNewPersistenceUnit() ) saveNewPersistenceUnit(node, name);
	else
		saveNodeDirectly(node, name);
}

void FileStore::saveNodeDirectly(const Model::Node *node, const QString &name)
{
	Q_ASSERT(!node->isPartiallyLoaded());

	xml->beginSaveChildNode(node->typeName());
	xml->setName(name);
	xml->setId(ids.getId(node));

	node->save(*this);
	xml->endSaveChildNode();
}

Model::Node* FileStore::loadModel(Model::Model*, const QString &name, bool loadPartially)
{
	storeAccess.lock();
	working = true;
	partiallyLoadingAModel_ = loadPartially;
	Model::LoadedNode ln;

	try
	{
		modelDir = baseFolder.path() + QDir::toNativeSeparators("/" + name);
		if ( !modelDir.exists() ) throw FilePersistenceException("Can not find root node folder " + modelDir.path());

		xml = new XMLModel(modelDir.absoluteFilePath(name));
		xml->goToFirstChild();
		ids.setNextId(xml->getNextId());
		xml->goToFirstChild();
		ln =  loadNode(nullptr, loadPartially);

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

Model::LoadedNode FileStore::loadNewPersistenceUnit(const QString& name, Model::Node* parent, bool loadPartially)
{
	XMLModel* oldXML = xml;
	xml = new XMLModel(modelDir.absoluteFilePath(name));
	xml->goToFirstChild();
	Model::LoadedNode ln =  loadNode(parent, loadPartially);

	SAFE_DELETE(xml);
	xml = oldXML;

	return ln;
}


QList<Model::LoadedNode> FileStore::loadAllSubNodes(Model::Node* parent, const QSet<QString>& loadPartially)
{
	checkIsWorking();

	QList < Model::LoadedNode > result;

	if (xml->hasChildren())
	{
		xml->goToFirstChild();
		while ( true )
		{
			Model::LoadedNode ln;
			if ( xml->getType() == XML_NEWUNIT_NODE_TAG )
				ln = loadNewPersistenceUnit(xml->loadStringValue(), parent, loadPartially.contains(xml->getName()));
			else ln = loadNode(parent, loadPartially.contains(xml->getName()));

			result.append(ln);

			if (xml->hasNext()) xml->loadNext();
			else break;
		}
		xml->goToParent();
	}

	return result;
}

Model::LoadedNode FileStore::loadNode(Model::Node* parent, bool loadPartially)
{
	Model::LoadedNode node;
	node.name = xml->getName();
	node.node = Model::Node::createNewNode(xml->getType(), parent, *this, partiallyLoadingAModel_ && loadPartially);
	ids.setId( node.node, xml->getId() ); // Record id
	return node;
}

Model::Node* FileStore::loadSubNode(Model::Node* parent, const QString& name, bool loadPartially)
{
	checkIsWorking();

	if (!xml->hasChild(name)) return nullptr;
	xml->beginLoadChildNode(name);

	Model::LoadedNode ln;
	if ( xml->getType() == XML_NEWUNIT_NODE_TAG )
		ln = loadNewPersistenceUnit(xml->loadStringValue(), parent, loadPartially);
	else ln = loadNode(parent, loadPartially);

	xml->endLoadChildNode();

	return ln.node;
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
			throw FilePersistenceException("Could not find the persisted data for partial node with id "
					+ QString::number(nodeId));

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

bool FileStore::isLoadingPartially() const
{
	return partiallyLoadingAModel_;
}

}
