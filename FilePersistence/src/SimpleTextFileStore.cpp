/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "SimpleTextFileStore.h"
#include "FilePersistenceException.h"

#include "ModelBase/src/model/Model.h"
#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/ModelException.h"
#include "ModelBase/src/persistence/PersistedNode.h"
#include "ModelBase/src/persistence/PersistedValue.h"

namespace FilePersistence {

const char* PERSISTENT_UNIT_NODE_TYPE = "persistencenewunit";
const QString SimpleTextFileStore::NULL_STRING = "____NULL____";

// TODO the Envision folder should be taken from the environment not hardcoded.
SimpleTextFileStore::SimpleTextFileStore() :
		baseFolder_(QDir::home().path() + QDir::toNativeSeparators("/Envision/projects"))
{}

SimpleTextFileStore::~SimpleTextFileStore()
{
	SAFE_DELETE(persisted_);
}

void SimpleTextFileStore::setBaseFolder(const QString& path)
{
	baseFolder_ = path;
}

QString SimpleTextFileStore::getPersistenceUnitName(const Model::Node *node)
{
	NodeIdMap::NodeIdType persistenceUnitId = ids_.getId( node->persistentUnitNode() );

	QString name;
	if (persistenceUnitId == 0) name = node->model()->name();
	else name = QString::number(persistenceUnitId);

	return name;
}

//**********************************************************************************************************************
// Methods from Persistent Store
//**********************************************************************************************************************

void SimpleTextFileStore::saveModel(Model::Model* model, const QString &name)
{
	storeAccess_.lock();
	working_ = true;
	model->beginExclusiveRead();

	try
	{
		if ( !baseFolder_.exists(name) )
			if ( !baseFolder_.mkpath(name) )
				throw FilePersistenceException("Could not create folder " + baseFolder_.path() + " for model.");

		modelDir_ = baseFolder_.path() + QDir::toNativeSeparators("/" + name);

		if ( !modelDir_.exists() ) throw FilePersistenceException("Error opening model folder " + modelDir_.path());

		saveNewPersistenceUnit(model->root(), name, false);
	}
	catch (Model::ModelException& e)
	{
		model->endExclusiveRead();
		working_ = false;
		storeAccess_.unlock();
		throw;
	}
	model->endExclusiveRead();
	working_ = false;
	storeAccess_.unlock();
}

void SimpleTextFileStore::saveStringValue(const QString &value)
{
	checkIsWorking();
	persisted_->setValue(value);
}

void SimpleTextFileStore::saveIntValue(int value)
{
	checkIsWorking();
	persisted_->setValue((long)value);
}

void SimpleTextFileStore::saveDoubleValue(double value)
{
	checkIsWorking();
	persisted_->setValue(value);
}

void SimpleTextFileStore::saveReferenceValue(const QString &name, const Model::Node* target)
{
	checkIsWorking();
	QString targetString = target ? QString::number(ids_.getId(target)) : NULL_STRING;
	QString nameString = name.isNull() ? NULL_STRING : name;
	persisted_->setValue(targetString + ":" + nameString);
}

void SimpleTextFileStore::saveNewPersistenceUnit(const Model::Node *node, const QString &name, bool partialLoadHint)
{
	checkIsWorking();

	GenericNode* oldPersisted = persisted_;

	if ( node->parent() ) // If this is not the root node, then we should put a reference to this node
	{
		auto child = persisted_->addChild(new GenericNode());
		child->setName(name);
		child->setType(PERSISTENT_UNIT_NODE_TYPE);
		child->setId(ids_.getId(node));
	}

	persisted_ = new GenericNode();

	if (oldPersisted == nullptr)
	{
		// If this is the root node save the model information
		persisted_->setType("model");
		persisted_->setName("model");
		auto parent = persisted_;
		persisted_ = persisted_->addChild(new GenericNode());
		saveNodeDirectly(node, name, partialLoadHint);
		persisted_ = parent;
		persisted_->setId(ids_.getNextId());
	}
	else saveNodeDirectly(node, name, partialLoadHint);

	QString filename;
	if ( oldPersisted == nullptr ) filename = name; // This is the root of the model, save the file name
	else
		filename = QString::number( ids_.getId(node)); // This is not the root, so save by id

	QFile file(modelDir_.absoluteFilePath(filename));
	if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) )
		throw FilePersistenceException("Could not open file " + file.fileName() + ". " + file.errorString());

	QTextStream ts(&file);
	ts.setCodec("UTF-8");
	persisted_->save(ts);
	file.close();

	SAFE_DELETE(persisted_);
	persisted_ = oldPersisted;
}

void SimpleTextFileStore::saveNode(const Model::Node *node, const QString &name, bool partialLoadHint)
{
	checkIsWorking();

	if ( node->isNewPersistenceUnit() ) saveNewPersistenceUnit(node, name, partialLoadHint);
	else
	{
		auto parent = persisted_;
		persisted_ = persisted_->addChild(new GenericNode());
		saveNodeDirectly(node, name, partialLoadHint);
		persisted_ = parent;
	}
}

void SimpleTextFileStore::saveNodeDirectly(const Model::Node *node, const QString &name, bool partialLoadHint)
{
	persisted_->setName(name);
	persisted_->setType(node->typeName());
	persisted_->setId(ids_.getId(node));
	if (partialLoadHint) persisted_->setPartial(true);

	node->save(*this);

	// TODO this should rely on the loadCompleteNodeSubtree method of the previous store of node instead.
	if ( !node->isFullyLoaded() )
	{
		// Do not allow the modification of models which are not fully loaded
		Q_ASSERT(false);
	}
}

Model::Node* SimpleTextFileStore::loadModel(Model::Model*, const QString &name)
{
	storeAccess_.lock();
	working_ = true;
	Model::LoadedNode ln;

	try
	{
		modelDir_ = baseFolder_.path() + QDir::toNativeSeparators("/" + name);
		if ( !modelDir_.exists() ) throw FilePersistenceException("Can not find root node folder " + modelDir_.path());

		QFile file(modelDir_.absoluteFilePath(name));
		if ( !file.open(QIODevice::ReadOnly) )
			throw FilePersistenceException("Could not open file " + file.fileName() + ". " + file.errorString());

		QTextStream ts(&file);
		ts.setCodec("UTF-8");
		persisted_ = GenericNode::load(ts);

		Q_ASSERT(persisted_->name() == "model");
		Q_ASSERT(persisted_->type() == "model");
		auto nextId = persisted_->id();
		Q_ASSERT(nextId >= 0);
		ids_.setNextId(nextId);

		auto parent = persisted_;
		persisted_ = persisted_->children().first();
		ln =  loadNode(nullptr);
		persisted_ = parent;

		// Initialize all references
		for (auto p : uninitializedReferences_)
		{
			bool ok = false;
			NodeIdMap::NodeIdType id = p.second.toInt(&ok);
			if (!ok) throw FilePersistenceException("Incorrect id format for reference target " + p.second);


			Model::Node* target = const_cast<Model::Node*> (ids_.getNodeForId(id));
			if (!target) throw FilePersistenceException("A reference is pointing to an unloaded node " + p.second);

			setReferenceTargetr(p.first, target);
		}

		SAFE_DELETE(persisted_);
	}
	catch (Model::ModelException& e)
	{
		SAFE_DELETE(persisted_);
		working_ = false;
		storeAccess_.unlock();
		throw;
	}

	working_ = false;
	storeAccess_.unlock();

	return ln.node;
}

Model::LoadedNode SimpleTextFileStore::loadNewPersistenceUnit(const QString& name, Model::Node* parent)
{
	GenericNode* oldPersisted = persisted_;

	QFile file(modelDir_.absoluteFilePath(name));
	if ( !file.open(QIODevice::ReadOnly) )
		throw FilePersistenceException("Could not open file " + file.fileName() + ". " + file.errorString());

	QTextStream ts(&file);
	ts.setCodec("UTF-8");
	persisted_ = GenericNode::load(ts);

	Model::LoadedNode ln =  loadNode(parent);

	SAFE_DELETE(persisted_);
	persisted_ = oldPersisted;

	return ln;
}

QList<Model::LoadedNode> SimpleTextFileStore::loadAllSubNodes(Model::Node* parent)
{
	checkIsWorking();

	QList < Model::LoadedNode > result;

	auto previousPersisted = persisted_;
	for (auto c : persisted_->children())
	{
		persisted_ = c;

		Model::LoadedNode ln;
		if ( persisted_->type() == PERSISTENT_UNIT_NODE_TYPE )
			ln = loadNewPersistenceUnit(QString::number(persisted_->id()), parent);
		else ln = loadNode(parent);

		result.append(ln);
	}
	persisted_ = previousPersisted;

	return result;
}

Model::LoadedNode SimpleTextFileStore::loadNode(Model::Node* parent)
{
	Model::LoadedNode node;
	node.name = persisted_->name();
	node.node = Model::Node::createNewNode(persisted_->type(), parent, *this, persisted_->partial());
	ids_.setId( node.node, persisted_->id() ); // Record id
	return node;
}

Model::Node* SimpleTextFileStore::loadSubNode(Model::Node* parent, const QString& name)
{
	checkIsWorking();

	auto child = persisted_->child(name);
	if (!child) return nullptr;

	auto previousPersisted = persisted_;
	persisted_ = child;

	Model::LoadedNode ln;
	if ( child->type() == PERSISTENT_UNIT_NODE_TYPE )
		ln = loadNewPersistenceUnit(QString::number(persisted_->id()), parent);
	else ln = loadNode(parent);

	persisted_ = previousPersisted;
	return ln.node;
}

QList<Model::LoadedNode> SimpleTextFileStore::loadPartialNode(Model::Node* partialNode)
{
	storeAccess_.lock();
	working_ = true;
	Model::LoadedNode ln;

	QList < Model::LoadedNode > result;

	try
	{
		modelDir_ = baseFolder_.path() + QDir::toNativeSeparators("/" + partialNode->model()->name());
		if ( !modelDir_.exists() ) throw FilePersistenceException("Can not find root node folder " + modelDir_.path());

		QString filename = getPersistenceUnitName(partialNode);

		QFile file(modelDir_.absoluteFilePath(filename));
		if ( !file.open(QIODevice::ReadOnly) )
			throw FilePersistenceException("Could not open file " + file.fileName() + ". " + file.errorString());

		QTextStream ts(&file);
		ts.setCodec("UTF-8");
		persisted_ = GenericNode::load(ts);
		auto top = persisted_;

		// Search through the content in order to find the requested node id.
		persisted_ = persisted_->find(ids_.getId(partialNode));

		if (!persisted_)
			throw FilePersistenceException("Could not find the persisted data for partial node with id "
					+ QString::number( ids_.getId(partialNode)));

		// Load all subnodes and return them.
		// Pass a null parent as we do not want these nodes to be immediately assigned as children to the parent node.
		result = loadAllSubNodes(nullptr);

		SAFE_DELETE(top);
		persisted_ = nullptr;
	}
	catch (Model::ModelException& e)
	{
		working_ = false;
		storeAccess_.unlock();
		throw;
	}

	working_ = false;
	storeAccess_.unlock();

	return result;
}

QString SimpleTextFileStore::currentNodeType() const
{
	checkIsWorking();

	return persisted_->type();
}

Model::PersistedNode* SimpleTextFileStore::loadCompleteNodeSubtree(const QString& modelName, const Model::Node* node)
{
	storeAccess_.lock();
	working_ = true;

	Model::PersistedNode* result = nullptr;

	NodeIdMap::NodeIdType nodeId = 0;
	NodeIdMap::NodeIdType persistenceUnitId = 0;
	if (node)
	{
		nodeId = ids_.getId(node);
		persistenceUnitId = ids_.getId( node->persistentUnitNode() );
	}

	try
	{
		modelDir_ = baseFolder_.path() + QDir::toNativeSeparators("/" + modelName);
		if ( !modelDir_.exists() ) throw FilePersistenceException("Can not find root node folder " + modelDir_.path());

		QString filename;
		if (persistenceUnitId > 0) filename = QString::number(persistenceUnitId);
		else filename = modelName;

		QFile file(modelDir_.absoluteFilePath(filename));
		if ( !file.open(QIODevice::ReadOnly) )
			throw FilePersistenceException("Could not open file " + file.fileName() + ". " + file.errorString());

		QTextStream ts(&file);
		ts.setCodec("UTF-8");
		persisted_ = GenericNode::load(ts);
		auto top = persisted_;

		// Search through the content in order to find the requested node id.
		persisted_ = persisted_->find(nodeId);

		if (!persisted_)
			throw FilePersistenceException("Could not find the persisted data for partial node with id "
					+ QString::number(nodeId));

		// Load the node and return it.
		result = loadNodeData();

		SAFE_DELETE(top);
		persisted_ = nullptr;
	}
	catch (Model::ModelException& e)
	{
		working_ = false;
		storeAccess_.unlock();
		throw;
	}

	working_ = false;
	storeAccess_.unlock();

	return result;
}

Model::PersistedNode* SimpleTextFileStore::loadNodeData()
{
	checkIsWorking();

	if (persisted_->type() == PERSISTENT_UNIT_NODE_TYPE) return loadPersistentUnitData();

	Model::PersistedNode* result = nullptr;

	// Determine the type of the node
	if (persisted_->hasValue())
	{
		if ( persisted_->hasStringValue() )
		{
			Model::PersistedValue<QString> *val = new Model::PersistedValue<QString>();
			val->set( persisted_->valueAsString() );
			result = val;
		}
		else if ( persisted_->hasIntValue() )
		{
			Model::PersistedValue<int> *val = new Model::PersistedValue<int>();
			val->set( persisted_->valueAsLong() );
			result = val;
		}
		else if ( persisted_->hasDoubleValue() )
		{
			Model::PersistedValue<double> *val = new Model::PersistedValue<double>();
			val->set( persisted_->valueAsDouble() );
			result = val;
		}
	}
	else
	{
		auto val = new Model::PersistedValue< QList<Model::PersistedNode*> >();
		result = val;

		auto previousPersisted = persisted_;
		for(auto c : persisted_->children())
		{
			persisted_ = c;

			Model::PersistedNode* child;
			child = loadNodeData();
			val->value().append(child);
		}
		persisted_ = previousPersisted;
	}

	if (persisted_->id() >=0) result->setId(persisted_->id());
	result->setType(persisted_->type());
	result->setName(persisted_->name());
	result->setPartialHint(persisted_->partial());
	result->setNewPersistenceUnit(false);

	return result;
}

Model::PersistedNode* SimpleTextFileStore::loadPersistentUnitData( )
{
	checkIsWorking();

	GenericNode* previousPersisted = persisted_;

	QFile file(modelDir_.absoluteFilePath(QString::number(persisted_->id())));
	if ( !file.open(QIODevice::ReadOnly) )
		throw FilePersistenceException("Could not open file " + file.fileName() + ". " + file.errorString());

	QTextStream ts(&file);
	ts.setCodec("UTF-8");
	persisted_ = GenericNode::load(ts);

	Model::PersistedNode* result = loadNodeData();
	result->setNewPersistenceUnit(true);

	SAFE_DELETE(persisted_);
	persisted_ = previousPersisted;

	return result;
}

int SimpleTextFileStore::loadIntValue()
{
	checkIsWorking();
	return persisted_->valueAsLong();
}

QString SimpleTextFileStore::loadStringValue()
{
	checkIsWorking();
	return persisted_->valueAsString();
}

double SimpleTextFileStore::loadDoubleValue()
{
	checkIsWorking();
	return persisted_->valueAsDouble();
}

QString SimpleTextFileStore::loadReferenceValue(Model::Reference* r)
{
	checkIsWorking();
	QStringList ref = persisted_->valueAsString().split(":");
	QString target = ref.first() == NULL_STRING ? QString() : ref.first();
	QString name = ref.last() == NULL_STRING ? QString() : ref.last();

	if (!target.isNull()) uninitializedReferences_.append(qMakePair(r, target));

	return name;
}

void SimpleTextFileStore::checkIsWorking() const
{
	if ( !working_ ) throw FilePersistenceException("Performing an illegal file persistence operation.");
}

} /* namespace FilePersistence */
