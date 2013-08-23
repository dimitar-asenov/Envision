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

#pragma once

#include "filepersistence_api.h"
#include "GenericNode.h"
#include "NodeIdMap.h"

#include "ModelBase/src/persistence/PersistentStore.h"

namespace FilePersistence {

class FILEPERSISTENCE_API SimpleTextFileStore : public Model::PersistentStore
{
	public:
		SimpleTextFileStore();
		virtual ~SimpleTextFileStore();

		void setBaseFolder(const QString& baseFolder);

		// Methods from Persistent Store
		virtual void saveStringValue(const QString &value);
		virtual void saveIntValue(int value);
		virtual void saveDoubleValue(double value);
		virtual void saveReferenceValue(const QString &name, const Model::Node* target);
		virtual void saveNode(const Model::Node *node, const QString &name, bool partialLoadHint);

		virtual QList<Model::LoadedNode> loadAllSubNodes(Model::Node* parent);
		virtual Model::Node* loadSubNode(Model::Node* parent, const QString& name);
		virtual QString currentNodeType() const;
		virtual QList<Model::LoadedNode> loadPartialNode(Model::Node* partialNode);
		virtual Model::PersistedNode* loadCompleteNodeSubtree(const QString& modelName, const Model::Node* node);

		virtual int loadIntValue();
		virtual QString loadStringValue();
		virtual double loadDoubleValue();
		virtual QString loadReferenceValue(Model::Reference* r);

	protected:
		virtual void saveModel(Model::Model* model, const QString &name);
		virtual Model::Node* loadModel(Model::Model* model, const QString &name);

	private:

		/** The folder where all models are stored. Each model is a separate sub folder in the base folder. */
		QDir baseFolder_;

		/** A mutex that assures exclusive model saving and loading operations. */
		QMutex storeAccess_;

		/** A flag that indicates if the store is currently in the middle of saving or loading a model. */
		bool working_{};

		/**
		 * This is the folder where the current model is being saved to or loaded from. This is only valid if working is
		 * true.
		 */
		QDir modelDir_;

		void saveNewPersistenceUnit(const Model::Node *node, const QString &name, bool partialLoadHint);
		Model::LoadedNode loadNewPersistenceUnit(const QString& name, Model::Node* parent);
		Model::LoadedNode loadNode(Model::Node* parent);
		void saveNodeDirectly(const Model::Node *node, const QString &name, bool partialLoadHint);

		Model::PersistedNode* loadNodeData();
		Model::PersistedNode* loadPersistentUnitData();

		/**
		 * Returns the name of the persistent unit that corresponds to the specified node.
		 */
		QString getPersistenceUnitName(const Model::Node *node);

		void checkIsWorking() const;

		GenericNode* persisted_{};
		NodeIdMap ids_;

		/**
		 * This is a list of all references which have been constructed and which have a target that is not yet set.
		 * The second part of the pair is the target id.
		 */
		QList<QPair<Model::Reference*, QString>> uninitializedReferences_;

		static const QString NULL_STRING;
};

} /* namespace FilePersistence */
