/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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
#include "../FilePersistenceException.h"
#include "GenericNodeAllocator.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/ModelException.h"
#include "ModelBase/src/persistence/NodeIdMap.h"
#include "ModelBase/src/persistence/PersistedNode.h"
#include "ModelBase/src/persistence/PersistedValue.h"

namespace FilePersistence {

const char* PERSISTENT_UNIT_NODE_TYPE = "persistencenewunit";
const QString SimpleTextFileStore::NULL_STRING = "____NULL____";

// TODO the Envision folder should be taken from the environment not hardcoded.
SimpleTextFileStore::SimpleTextFileStore(const QString& baseDir) :
		baseFolder_{baseDir.isNull() ? QDir::home().path() + QDir::toNativeSeparators("/Envision/projects") : baseDir}
{}

SimpleTextFileStore::~SimpleTextFileStore()
{
	Q_ASSERT(persisted_ == nullptr);
}

SimpleTextFileStore* SimpleTextFileStore::clone() const
{
	auto ss = new SimpleTextFileStore();
	ss->baseFolder_ = baseFolder_;
	return ss;
}

void SimpleTextFileStore::setBaseFolder(const QString& path)
{
	baseFolder_ = path;
}

QString SimpleTextFileStore::getPersistenceUnitName(const Model::Node *node)
{
	Model::NodeIdType persistenceUnitId = Model::NodeIdMap::id( node->persistentUnitNode() );

	QString name;
	if (persistenceUnitId.isNull()) name = node->manager()->name();
	else name = persistenceUnitId.toString();

	return name;
}

//**********************************************************************************************************************
// Methods from Persistent Store
//**********************************************************************************************************************

void SimpleTextFileStore::saveTree(Model::TreeManager* manager, const QString &name)
{
	storeAccess_.lock();
	working_ = true;
	manager->beginExclusiveRead();

	try
	{
		if ( !baseFolder_.exists(name) )
			if ( !baseFolder_.mkpath(name) )
				throw FilePersistenceException("Could not create folder " + baseFolder_.path() + " for tree.");

		treeDir_ = baseFolder_.path() + QDir::toNativeSeparators("/" + name);

		if ( !treeDir_.exists() ) throw FilePersistenceException("Error opening tree folder " + treeDir_.path());

		saveNewPersistenceUnit(manager->root(), name);
	}
	catch (Model::ModelException& e)
	{
		manager->endExclusiveRead();
		working_ = false;
		storeAccess_.unlock();
		throw;
	}
	manager->endExclusiveRead();
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
	QString targetString = target ? Model::NodeIdMap::id(target).toString() : NULL_STRING;
	QString nameString = name.isNull() ? NULL_STRING : name;
	persisted_->setValue(targetString + ":" + nameString);
}

void SimpleTextFileStore::saveNewPersistenceUnit(const Model::Node *node, const QString &name)
{
	checkIsWorking();

	GenericNode* oldPersisted = persisted_;

	if ( node->parent() ) // If this is not the root node, then we should put a reference to this node
	{
		auto child = persisted_->addChild(new GenericNode());
		child->setName(name);
		child->setType(PERSISTENT_UNIT_NODE_TYPE);
		child->setId(Model::NodeIdMap::id(node));
	}

	persisted_ = new GenericNode();
	saveNodeDirectly(node, name);

	QString filename;
	if ( oldPersisted == nullptr ) filename = name; // This is the root of the tree, save the file name
	else filename = Model::NodeIdMap::id(node).toString(); // This is not the root, so save by id

	QFile file(treeDir_.absoluteFilePath(filename));
	if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) )
		throw FilePersistenceException("Could not open file " + file.fileName() + ". " + file.errorString());

	QTextStream ts(&file);
	ts.setCodec("UTF-8");
	persisted_->save(ts);
	file.close();

	SAFE_DELETE(persisted_);
	persisted_ = oldPersisted;
}

void SimpleTextFileStore::saveNode(const Model::Node *node, const QString &name)
{
	checkIsWorking();

	if ( node->isNewPersistenceUnit() ) saveNewPersistenceUnit(node, name);
	else
	{
		auto parent = persisted_;
		persisted_ = persisted_->addChild(new GenericNode());
		saveNodeDirectly(node, name);
		persisted_ = parent;
	}
}

void SimpleTextFileStore::saveNodeDirectly(const Model::Node *node, const QString &name)
{
	Q_ASSERT(!node->isPartiallyLoaded());
	persisted_->setName(name);
	persisted_->setType(node->typeName());
	persisted_->setId(Model::NodeIdMap::id(node));

	node->save(*this);
}

Model::Node* SimpleTextFileStore::loadTree(Model::TreeManager*, const QString &name, bool loadPartially)
{
	storeAccess_.lock();
	working_ = true;
	partiallyLoadingATree_ = loadPartially;
	Model::LoadedNode ln;

	try
	{
		treeDir_ = baseFolder_.path() + QDir::toNativeSeparators("/" + name);
		if ( !treeDir_.exists() ) throw FilePersistenceException("Can not find root node folder " + treeDir_.path());
		allocator_ = new GenericNodeAllocator();
		persisted_ = GenericNode::load(treeDir_.absoluteFilePath(name), true, allocator_);

		ln =  loadNode(nullptr, loadPartially);

		// Initialize all references
		for (auto p : uninitializedReferences_)
		{
			Model::NodeIdType id = p.second;
			if (id.isNull()) throw FilePersistenceException("Incorrect id format for reference target " + p.second);

			Model::Node* target = const_cast<Model::Node*> (Model::NodeIdMap::node(id));
			if (!target) throw FilePersistenceException("A reference is pointing to an unloaded node " + p.second);

			setReferenceTargetr(p.first, target);
		}

		allocator_->endThisLoad();
		persisted_ = nullptr;
		SAFE_DELETE(allocator_);
	}
	catch (Model::ModelException& e)
	{
		allocator_->endThisLoad();
		persisted_ = nullptr;
		SAFE_DELETE(allocator_);
		working_ = false;
		storeAccess_.unlock();
		throw;
	}

	working_ = false;
	storeAccess_.unlock();

	return ln.node;
}

Model::LoadedNode SimpleTextFileStore::loadNewPersistenceUnit(const QString& name, Model::Node* parent,
		bool loadPartially)
{
	GenericNode* oldPersisted = persisted_;

	persisted_ = GenericNode::load(treeDir_.absoluteFilePath(name), true, allocator_);

	Model::LoadedNode ln =  loadNode(parent, loadPartially);

	allocator_->endThisLoad();
	persisted_ = oldPersisted;

	return ln;
}

QList<Model::LoadedNode> SimpleTextFileStore::loadAllSubNodes(Model::Node* parent, const QSet<QString>& loadPartially)
{
	checkIsWorking();

	QList < Model::LoadedNode > result;

	auto previousPersisted = persisted_;
	for (auto c : persisted_->children())
	{
		persisted_ = c;

		Model::LoadedNode ln;
		if ( persisted_->type() == PERSISTENT_UNIT_NODE_TYPE )
			ln = loadNewPersistenceUnit(
					persisted_->id().toString(), parent, loadPartially.contains(persisted_->name()));
		else ln = loadNode(parent, loadPartially.contains(persisted_->name()));

		result.append(ln);
	}
	persisted_ = previousPersisted;

	return result;
}

Model::LoadedNode SimpleTextFileStore::loadNode(Model::Node* parent, bool loadPartially)
{
	Model::LoadedNode node;
	node.name = persisted_->name();
	node.node = Model::Node::createNewNode(persisted_->type(), parent, *this, partiallyLoadingATree_ && loadPartially);
	Model::NodeIdMap::setId( node.node, persisted_->id() ); // Record id
	return node;
}

Model::Node* SimpleTextFileStore::loadSubNode(Model::Node* parent, const QString& name, bool loadPartially)
{
	checkIsWorking();

	auto child = persisted_->child(name);
	if (!child) return nullptr;

	auto previousPersisted = persisted_;
	persisted_ = child;

	Model::LoadedNode ln;
	if ( child->type() == PERSISTENT_UNIT_NODE_TYPE )
		ln = loadNewPersistenceUnit(persisted_->id().toString(), parent, loadPartially);
	else ln = loadNode(parent, loadPartially);

	persisted_ = previousPersisted;
	return ln.node;
}

QString SimpleTextFileStore::currentNodeType() const
{
	checkIsWorking();

	return persisted_->type();
}

Model::PersistedNode* SimpleTextFileStore::loadCompleteNodeSubtree(const QString& treeName, const Model::Node* node)
{
	storeAccess_.lock();
	working_ = true;

	Model::PersistedNode* result = nullptr;

	Model::NodeIdType nodeId;
	Model::NodeIdType persistenceUnitId;
	if (node)
	{
		nodeId = Model::NodeIdMap::id(node);
		persistenceUnitId = Model::NodeIdMap::id( node->persistentUnitNode() );
	}

	try
	{
		treeDir_ = baseFolder_.path() + QDir::toNativeSeparators("/" + treeName);
		if ( !treeDir_.exists() ) throw FilePersistenceException("Can not find root node folder " + treeDir_.path());

		QString filename;
		if (!persistenceUnitId.isNull()) filename = persistenceUnitId.toString();
		else filename = treeName;

		allocator_= new GenericNodeAllocator();
		persisted_ = GenericNode::load(treeDir_.absoluteFilePath(filename), true, allocator_);

		// Search through the content in order to find the requested node id.
		if (!nodeId.isNull()) persisted_ = persisted_->find(nodeId);

		if (!persisted_)
			throw FilePersistenceException("Could not find the persisted data for partial node with id "
					+ nodeId.toString());

		// Load the node and return it.
		result = loadNodeData();

		allocator_->endThisLoad();
		persisted_ = nullptr;
		SAFE_DELETE(allocator_);
	}
	catch (Model::ModelException& e)
	{
		allocator_->endThisLoad();
		persisted_ = nullptr;
		SAFE_DELETE(allocator_);
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
		for (auto c : persisted_->children())
		{
			persisted_ = c;

			Model::PersistedNode* child;
			child = loadNodeData();
			val->value().append(child);
		}
		persisted_ = previousPersisted;
	}

	if (!persisted_->id().isNull()) result->setId(persisted_->id());
	result->setType(persisted_->type());
	result->setName(persisted_->name());
	result->setNewPersistenceUnit(false);

	return result;
}

Model::PersistedNode* SimpleTextFileStore::loadPersistentUnitData( )
{
	checkIsWorking();

	GenericNode* previousPersisted = persisted_;
	persisted_ = GenericNode::load(treeDir_.absoluteFilePath(persisted_->id().toString()), true, allocator_);

	Model::PersistedNode* result = loadNodeData();
	result->setNewPersistenceUnit(true);

	allocator_->endThisLoad();
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

bool SimpleTextFileStore::isLoadingPartially() const
{
	return partiallyLoadingATree_;
}

} /* namespace FilePersistence */
