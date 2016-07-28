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

#pragma once

#include "../filepersistence_api.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace Model {
	class Node;
	class Reference;
	class TreeManager;
}

namespace FilePersistence {

class GenericNode;
class GenericTree;

class FILEPERSISTENCE_API SimpleTextFileStore : public Model::PersistentStore
{
	public:
		using FileGetterFunction = std::function<bool (QString filename, const char*& data, int& dataLength)>;

		/**
		 * TODO the Envision folder should be taken from the environment not hardcoded.
		 */
		SimpleTextFileStore(const QString& baseDir = QString::null);
		SimpleTextFileStore(FileGetterFunction fileGetter);
		SimpleTextFileStore(GenericTree* externalTree);

		virtual ~SimpleTextFileStore();

		virtual SimpleTextFileStore* clone() const override;

		void setBaseFolder(const QString& baseFolder);

		/**
		 * Methods from Persistent Store
		 */
		virtual void saveStringValue(const QString &value) override;
		virtual void saveIntValue(int value) override;
		virtual void saveDoubleValue(double value) override;
		virtual void saveReferenceValue(const QString &name, const Model::Node* target) override;
		virtual void saveNode(const Model::Node *node, const QString &name) override;

		virtual QList<Model::LoadedNode> loadAllSubNodes(Model::Node* parent, const QSet<QString>& loadPartially)override;
		virtual Model::Node* loadSubNode(Model::Node* parent, const QString& name, bool loadPartially) override;
		virtual QString currentNodeType() const override;

		virtual int loadIntValue() override;
		virtual QString loadStringValue() override;
		virtual double loadDoubleValue() override;
		virtual QString loadReferenceValue(Model::Reference* r) override;

		virtual bool isLoadingPartially() const override;

		static void saveGenericTree(std::shared_ptr<GenericTree> tree, const QString& name, const QString& destDir,
											 const QStringList& persistentUnitTypes);

		static void setForceSinglePersistentUnit();

	protected:
		virtual void saveTree(Model::TreeManager* manager, const QString &name) override;
		virtual Model::Node* loadTree(Model::TreeManager* manager, const QString &name, bool loadPartially) override;

	private:

		/**
		 * If specified, this function will be used to get the data of the files. Only used for loading.
		 */
		FileGetterFunction fileGetter_{};

		/**
		 * The folder where all trees are stored. Each tree is a separate sub folder in the base folder.
		 */
		QDir baseFolder_;

		/**
		 * A mutex that assures exclusive tree saving and loading operations.
		 */
		QMutex storeAccess_;

		/**
		 * The manager that requested the last load/save operation.
		 */
		Model::TreeManager* treeManager_{};

		/**
		 * A flag that indicates if the store is currently in the middle of saving or loading a tree.
		 */
		bool working_{};
		bool partiallyLoadingATree_{};

		/**
		 * This is a stack of the the folder where the current tree is being saved to or loaded from.
		 * This is only valid if working is true.
		 */
		QStack<QDir> treeDirs_;

		static QList<GenericNode*> writeGenericNodeToFile(GenericNode* node, const QString& destDir,
																			const QString& fileName,
																			const QStringList& persistentUnitTypes);
		void saveNewPersistenceUnit(const Model::Node *node, const QString &name);
		Model::LoadedNode loadNewPersistenceUnit(const QString& name, Model::Node* parent,
															  QString relativeFilePath, bool loadPartially);
		Model::LoadedNode loadNode(Model::Node* parent, bool loadPartially);
		void saveNodeDirectly(const Model::Node *node, const QString &name);

		/**
		 * Returns the name of the persistent unit that corresponds to the specified node.
		 */
		QString getPersistenceUnitName(const Model::Node *node);

		/**
		 * Returns the relative directory path to a persistent unit
		 */
		static QString relativeDirectoryPathForPersistenceUnit(const Model::Node* node);

		void checkIsWorking() const;

		GenericTree* genericTree_{};
		GenericNode* genericNode_{};

		/**
		 * If the tree is external, only loading is supported and the tree will not be deleted.
		 */
		GenericTree* externalTree_{};

		/**
		 * This is a list of all references which have been constructed and which have a target that is not yet set.
		 * The second part of the pair is the target id.
		 */
		QList<QPair<Model::Reference*, QString>> uninitializedReferences_;

		static const QString NULL_STRING;

		/**
		 * If true, only a single persistent unit will be generated.
		 */
		static bool ForceSinglePersistentUnit;

		std::set<QString> oldFiles_;
		std::set<QString> newFiles_;
};

inline void SimpleTextFileStore::setForceSinglePersistentUnit() { ForceSinglePersistentUnit = true; }

}
