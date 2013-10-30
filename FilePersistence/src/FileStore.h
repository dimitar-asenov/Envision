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

#pragma once

#include "filepersistence_api.h"
#include "XMLModel.h"
#include "NodeIdMap.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace Model {
	class Reference;
}

namespace FilePersistence {

class FILEPERSISTENCE_API FileStore: public Model::PersistentStore
{
	public:
		FileStore();
		virtual ~FileStore();

		void setBaseFolder(const QString& baseFolder);

		// Methods from Persistent Store
		virtual void saveStringValue(const QString &value) override;
		virtual void saveIntValue(int value) override;
		virtual void saveDoubleValue(double value) override;
		virtual void saveReferenceValue(const QString &name, const Model::Node* target) override;
		virtual void saveNode(const Model::Node *node, const QString &name) override;

		virtual QList<Model::LoadedNode> loadAllSubNodes(Model::Node* parent, const QSet<QString>& loadPartially)override;
		virtual Model::Node* loadSubNode(Model::Node* parent, const QString& name, bool loadPartially) override;
		virtual QString currentNodeType() const override;
		virtual Model::PersistedNode* loadCompleteNodeSubtree(const QString& modelName, const Model::Node* node) override;

		virtual int loadIntValue() override;
		virtual QString loadStringValue() override;
		virtual double loadDoubleValue() override;
		virtual QString loadReferenceValue(Model::Reference* r) override;

		virtual bool isLoadingPartially() const override;

	protected:
		virtual void saveModel(Model::Model* model, const QString &name) override;
		virtual Model::Node* loadModel(Model::Model* model, const QString &name, bool loadPartially) override;

	private:

		/** The folder where all models are stored. Each model is a separate sub folder in the base folder. */
		QDir baseFolder;

		/** A mutex that assures exclusive model saving and loading operations. */
		QMutex storeAccess;

		/** A flag that indicates if the store is currently in the middle of saving or loading a model. */
		bool working;

		bool partiallyLoadingAModel_{};

		/**
		 * This is the folder where the current model is being saved to or loaded from. This is only valid if working is
		 * true.
		 */
		QDir modelDir;

		void saveNewPersistenceUnit(const Model::Node *node, const QString &name);
		Model::LoadedNode loadNewPersistenceUnit(const QString& name, Model::Node* parent, bool loadPartially);
		Model::LoadedNode loadNode(Model::Node* parent, bool loadPartially);
		void saveNodeDirectly(const Model::Node *node, const QString &name);

		Model::PersistedNode* loadNodeData();
		Model::PersistedNode* loadPersistentUnitData();

		/**
		 * Returns the name of the persistent unit that corresponds to the specified node.
		 */
		QString getPersistenceUnitName(const Model::Node *node);

		void checkIsWorking() const;

		XMLModel* xml;
		NodeIdMap ids;

		/**
		 * \brief This is a list of all references which have been constructed and which have a target that is not yet
		 *        set. The second part of the pair is the target id.
		 */
		QList<QPair<Model::Reference*, QString>> uninitializedReferences;

		static const QString NULL_STRING;
};

}
