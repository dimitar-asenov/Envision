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

#include "GenericNode.h"
#include "GenericPersistentUnit.h"
#include "GenericTree.h"
#include "Parser.h"
#include "../FilePersistenceException.h"
#include "../utils/FileUtil.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/ModelException.h"
#include "ModelBase/src/persistence/NodeIdMap.h"

namespace FilePersistence {

const QString SimpleTextFileStore::NULL_STRING{"____NULL____"};
bool SimpleTextFileStore::ForceSinglePersistentUnit{false};

SimpleTextFileStore::SimpleTextFileStore(const QString& baseDir) :
	baseFolder_{baseDir.isNull() ? QDir::home().path() +
											 QDir::toNativeSeparators("/Envision/projects")
										  : baseDir}
{}

SimpleTextFileStore::SimpleTextFileStore(FileGetterFunction fileGetter) : fileGetter_{fileGetter} {}
SimpleTextFileStore::SimpleTextFileStore(GenericTree* externalTree): externalTree_{externalTree} {}

SimpleTextFileStore::~SimpleTextFileStore()
{
	Q_ASSERT(genericNode_ == nullptr);
}

SimpleTextFileStore* SimpleTextFileStore::clone() const
{
	auto ss = new SimpleTextFileStore{};
	ss->baseFolder_ = baseFolder_;
	ss->externalTree_ = externalTree_;
	return ss;
}

void SimpleTextFileStore::setBaseFolder(const QString& baseFolder)
{
	Q_ASSERT(!fileGetter_);
	Q_ASSERT(!externalTree_);
	baseFolder_.setPath(baseFolder);
}

QString SimpleTextFileStore::getPersistenceUnitName(const Model::Node *node)
{
	Model::NodeIdType persistenceUnitId = treeManager_->nodeIdMap().id( node->persistentUnitNode() );

	QString name;
	if (persistenceUnitId.isNull()) name = node->manager()->name();
	else name = persistenceUnitId.toString();

	return name;
}

QString SimpleTextFileStore::relativeDirectoryPathForPersistenceUnit(const Model::Node* node)
{
	 auto ancestor = node->parent();
	 while (ancestor && !(ancestor->isNewPersistenceUnit() && ancestor->definesSymbol()))
		 ancestor = ancestor->parent();

	 if (ancestor && ancestor->parent())
		 return ancestor->symbolName() + "/";

	 return "";
}

void SimpleTextFileStore::saveTree(Model::TreeManager* manager, const QString &name)
{
	storeAccess_.lock();
	working_ = true;
	manager->beginExclusiveRead();
	treeManager_ = manager;

	try
	{
		Q_ASSERT(!fileGetter_);
		Q_ASSERT(!externalTree_);
		if ( !baseFolder_.exists(name) )
			if ( !baseFolder_.mkpath(name) )
				throw FilePersistenceException{"Could not create folder " + baseFolder_.path() + " for tree."};

		treeDirs_.clear();
		treeDirs_.push(baseFolder_.path() + QDir::toNativeSeparators("/" + name));

		if ( !treeDirs_.top().exists() )
			throw FilePersistenceException{"Error opening tree folder " + treeDirs_.top().path()};

		genericTree_ = new GenericTree{name};
		saveNewPersistenceUnit(manager->root(), name);

		FileUtil::deleteUnnecessaryFiles(oldFiles_, newFiles_);

		oldFiles_ = std::set<QString>{newFiles_};
		newFiles_ = std::set<QString>{};

		SAFE_DELETE(genericTree_);
	}
	catch (Model::ModelException& e)
	{
		SAFE_DELETE(genericTree_);
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
	genericNode_->setValue(value);
}

void SimpleTextFileStore::saveIntValue(int value)
{
	checkIsWorking();
	genericNode_->setValue((long)value);
}

void SimpleTextFileStore::saveDoubleValue(double value)
{
	checkIsWorking();
	genericNode_->setValue(value);
}

void SimpleTextFileStore::saveReferenceValue(const QString &name, const Model::Node* target)
{
	checkIsWorking();
	QString targetString = target ? treeManager_->nodeIdMap().id(target).toString() : NULL_STRING;
	QString nameString = name.isNull() ? NULL_STRING : name;
	genericNode_->setValue(targetString + ":" + nameString);
}

void SimpleTextFileStore::saveNewPersistenceUnit(const Model::Node *node, const QString &name)
{
	checkIsWorking();

	GenericNode* oldPersisted = genericNode_;

	// For the root, use its name, otherwise use the ID.
	QString puName = oldPersisted == nullptr ? name : treeManager_->nodeIdMap().id(node).toString();
	QString fileName = oldPersisted == nullptr ? puName : node->symbolName() + " " + puName;
	QString relativeDirectory = relativeDirectoryPathForPersistenceUnit(node);
	QString relativeFileName = relativeDirectory + fileName;

	// If this is not the root node, then we should put a reference to this node
	if ( node->parent() )
	{
		auto child = genericNode_->attachChild(genericNode_->persistentUnit()->newNode());
		child->setLabel(name);
		child->setType(GenericNode::PERSISTENT_UNIT_TYPE);
		child->setId(treeManager_->nodeIdMap().id(node));
		child->setValue(relativeFileName);
		if (node->parent()) child->setParentId(treeManager_->nodeIdMap().id(node->parent()));
	}

	QString parentAbsoluteDirectory = treeDirs_.top().absolutePath();
	QString absoluteDirectory = treeDirs_.top().absolutePath() + '/' + relativeDirectory;
	treeDirs_.push(absoluteDirectory);
	treeDirs_.top().mkpath(".");

	genericNode_ = genericTree_->newPersistentUnit(puName).newNode();
	saveNodeDirectly(node, name);
	writeGenericNodeToFile(genericNode_, parentAbsoluteDirectory, relativeFileName, {});

	QString path = QString{parentAbsoluteDirectory + '/' + relativeFileName};
	newFiles_.insert(path);
	newFiles_.insert(QFileInfo{path}.absoluteDir().absolutePath());

	treeDirs_.pop();

	genericTree_->removePersistentUnit(name);
	genericNode_ = oldPersisted;
}

void SimpleTextFileStore::saveGenericTree(std::shared_ptr<GenericTree> tree, const QString& name,
														const QString& destDir, const QStringList& persistentUnitTypes)
{
	// Put all existing PersistentUnits (except for the root) on the stack
	auto rootNode = tree->root();
	QList<GenericNode*> stack{};
	for ( auto pu : tree->persistentUnits() )
	{
		auto puRoot = pu->unitRootNode();
		if (puRoot != rootNode) stack << rootNode;
	}

	// Write the root and generate possibly other PUs, that were not in the original structure
	stack << writeGenericNodeToFile(rootNode, destDir, name,
											  ForceSinglePersistentUnit ? QStringList{} : persistentUnitTypes);

	// For each pu write and clean up
	while (!stack.isEmpty())
	{
		auto pu = stack.takeLast();
		// Make sure to remove all occurences of PUs as soon as we see one of them.
		stack.removeAll(pu);
		stack << writeGenericNodeToFile(pu, destDir, pu->id().toString(), persistentUnitTypes);
	}
}

QList<GenericNode*> SimpleTextFileStore::writeGenericNodeToFile(GenericNode* node, const QString& destDir,
	const QString& fileName, const QStringList& persistentUnitTypes)
{
	QFile file{destDir + '/' + fileName};
	if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate) )
		throw FilePersistenceException{"Could not open file " + file.fileName() + ". " + file.errorString()};

	QTextStream ts{&file};
	ts.setCodec("UTF-8");
	auto persistentUnits = Parser::save(ts, node, persistentUnitTypes);
	file.close();

	return persistentUnits;
}

void SimpleTextFileStore::saveNode(const Model::Node *node, const QString &name)
{
	checkIsWorking();

	if ( node->isNewPersistenceUnit() ) saveNewPersistenceUnit(node, name);
	else
	{
		auto parent = genericNode_;
		genericNode_ = genericNode_->attachChild(genericNode_->persistentUnit()->newNode());
		saveNodeDirectly(node, name);
		genericNode_ = parent;
	}
}

void SimpleTextFileStore::saveNodeDirectly(const Model::Node *node, const QString &name)
{
	Q_ASSERT(!node->isPartiallyLoaded());
	genericNode_->setLabel(name);
	genericNode_->setType(node->typeName());
	genericNode_->setId(treeManager_->nodeIdMap().id(node));
	if (node->parent()) genericNode_->setParentId(treeManager_->nodeIdMap().id(node->parent()));

	node->save(*this);
}

Model::Node* SimpleTextFileStore::loadTree(Model::TreeManager* manager, const QString &name, bool loadPartially)
{
	storeAccess_.lock();
	working_ = true;
	treeManager_ = manager;
	partiallyLoadingATree_ = loadPartially;
	Model::LoadedNode ln;

	try
	{
		if (!externalTree_)
		{
				if (!fileGetter_)
				{
					treeDirs_.clear();
					treeDirs_.push(baseFolder_.path() + QDir::toNativeSeparators("/" + name));
					if ( !treeDirs_.top().exists() )
						throw FilePersistenceException{"Can not find root node folder " + treeDirs_.top().path()};
				}

				genericTree_ = new GenericTree{name};
		}

		ln = loadNewPersistenceUnit(name, nullptr, name, loadPartially);

		// Initialize all references
		for (auto p : uninitializedReferences_)
		{
			Model::NodeIdType id = p.second;
			if (id.isNull()) throw FilePersistenceException{"Incorrect id format for reference target " + p.second};

			Model::Node* target = const_cast<Model::Node*> (treeManager_->nodeIdMap().node(id));
			if (target)
			{
				// A reference might have pointed to a temporary node when saved and will now be invalid
				// TODO: shouldn't we rather export such references as invlaid?
				setReferenceTargetr(p.first, target);
			}
		}

		SAFE_DELETE(genericTree_);
	}
	catch (Model::ModelException& e)
	{
		SAFE_DELETE(genericTree_);
		working_ = false;
		storeAccess_.unlock();
		throw;
	}

	working_ = false;
	storeAccess_.unlock();

	return ln.node;
}

Model::LoadedNode SimpleTextFileStore::loadNewPersistenceUnit(const QString& name, Model::Node* parent,
																				  QString relativeFilePath, bool loadPartially)
{
	GenericNode* oldPersisted = genericNode_;

	if (externalTree_)
	{
		Q_ASSERT(relativeFilePath.isEmpty());
		auto pu = externalTree_->persistentUnit(name);
		Q_ASSERT(pu);
		genericNode_ = pu->unitRootNode();
	}
	else if (fileGetter_)
	{
		const char* data = nullptr;
		int dataLength = 0;
		bool success = fileGetter_(name, data, dataLength);
		Q_ASSERT(success);
		Q_ASSERT(data);
		Q_ASSERT(dataLength > 0);

		genericNode_ = Parser::load(data, dataLength, true, genericTree_->newPersistentUnit(name));
	}
	else
	{
		QString absoluteFilePath = treeDirs_.top().absoluteFilePath(relativeFilePath);
		oldFiles_.insert(absoluteFilePath);
		genericNode_ = Parser::load(absoluteFilePath, true, genericTree_->newPersistentUnit(name));
		QDir dir = QFileInfo{absoluteFilePath}.absoluteDir();
		// also add directory in case it becomes empty and can be deleted
		oldFiles_.insert(dir.absolutePath());
		treeDirs_.push(dir);
	}

	Q_ASSERT(genericNode_);

	Model::LoadedNode ln =  loadNode(parent, loadPartially);

	if (!externalTree_) genericTree_->removePersistentUnit(name);
	if (!externalTree_ && !fileGetter_) treeDirs_.pop();
	genericNode_ = oldPersisted;

	return ln;
}

QList<Model::LoadedNode> SimpleTextFileStore::loadAllSubNodes(Model::Node* parent, const QSet<QString>& loadPartially)
{
	checkIsWorking();

	QList < Model::LoadedNode > result;
	result.reserve(genericNode_->children().size());

	auto previousPersisted = genericNode_;
	for (auto c : genericNode_->children())
	{
		genericNode_ = c;

		Model::LoadedNode ln;
		if ( genericNode_->type() == GenericNode::PERSISTENT_UNIT_TYPE )
			ln = loadNewPersistenceUnit( genericNode_->id().toString(), parent, genericNode_->valueAsString(),
												  loadPartially.contains(genericNode_->label()));
		else ln = loadNode(parent, loadPartially.contains(genericNode_->label()));

		result.append(ln);
	}
	genericNode_ = previousPersisted;

	return result;
}

Model::LoadedNode SimpleTextFileStore::loadNode(Model::Node* parent, bool loadPartially)
{
	Model::LoadedNode node;
	node.name = genericNode_->label();
	node.node = Model::Node::createNewNode(genericNode_->type(), parent, *this, partiallyLoadingATree_ && loadPartially);
	// Record id
	treeManager_->nodeIdMap().setId( node.node, genericNode_->id() );
	return node;
}

Model::Node* SimpleTextFileStore::loadSubNode(Model::Node* parent, const QString& name, bool loadPartially)
{
	checkIsWorking();

	auto child = genericNode_->child(name);
	if (!child) return nullptr;

	auto previousPersisted = genericNode_;
	genericNode_ = child;

	Model::LoadedNode ln;
	if ( child->type() == GenericNode::PERSISTENT_UNIT_TYPE )
		ln = loadNewPersistenceUnit(genericNode_->id().toString(), parent, genericNode_->valueAsString(), loadPartially);
	else ln = loadNode(parent, loadPartially);

	genericNode_ = previousPersisted;
	return ln.node;
}

QString SimpleTextFileStore::currentNodeType() const
{
	checkIsWorking();

	return genericNode_->type();
}

int SimpleTextFileStore::loadIntValue()
{
	checkIsWorking();
	return genericNode_->valueAsLong();
}

QString SimpleTextFileStore::loadStringValue()
{
	checkIsWorking();
	return genericNode_->valueAsString();
}

double SimpleTextFileStore::loadDoubleValue()
{
	checkIsWorking();
	return genericNode_->valueAsDouble();
}

QString SimpleTextFileStore::loadReferenceValue(Model::Reference* r)
{
	checkIsWorking();
	QString string =  genericNode_->valueAsString();
	int midIndex = string.indexOf(':');
	Q_ASSERT(midIndex >= 0);

	// id
	auto part = string.leftRef(midIndex);
	QString target = part == NULL_STRING ? QString{} : part.toString();

	// name
	part = string.midRef(midIndex+1);
	QString name = part == NULL_STRING ? QString{} : part.toString();

	if (!target.isNull()) uninitializedReferences_.append(qMakePair(r, target));

	return name;
}

void SimpleTextFileStore::checkIsWorking() const
{
	if ( !working_ ) throw FilePersistenceException{"Performing an illegal file persistence operation."};
}

bool SimpleTextFileStore::isLoadingPartially() const
{
	return partiallyLoadingATree_;
}

}
