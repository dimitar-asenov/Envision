/***********************************************************************************************************************
 * SystemClipboard.h
 *
 *  Created on: Jan 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SYSTEMCLIPBOARD_H_
#define SYSTEMCLIPBOARD_H_

#include "filepersistence_api.h"
#include "XMLModel.h"

#include "ModelBase/headers/persistence/ClipboardStore.h"

namespace Model { class PersistedNode; }

namespace FilePersistence {

class FILEPERSISTENCE_API SystemClipboard : public Model::ClipboardStore
{
	public:
		SystemClipboard();
		virtual ~SystemClipboard();

		// Methods from Persistent Store
		virtual void saveModel(Model::Model& model, const QString &name);

		virtual void saveStringValue(const QString &value);
		virtual void saveIntValue(int value);
		virtual void saveDoubleValue(double value);
		virtual void saveNode(const Model::Node *node, const QString &name, bool partialLoadHint);

		virtual Model::Node* loadRootNode(const QString &name);
		virtual QList<Model::LoadedNode> loadAllSubNodes(Model::Node* parent);
		virtual Model::Node* loadSubNode(Model::Node* parent, const QString& name);
		virtual QList<Model::LoadedNode> loadPartialNode(Model::Node* partialNode);
		virtual Model::PersistedNode* loadCompleteNodeSubtree(const QString& modelName, Model::NodeIdType persistenceUnitId, Model::NodeIdType nodeId);

		virtual int loadIntValue();
		virtual QString loadStringValue();
		virtual double loadDoubleValue();

		// Methods from ClipboardStore
		virtual void putNode(const Model::Node* node);
		virtual void putNodes(const QList<const Model::Node*>& nodes);

		virtual int numNodes() const;
		virtual bool hasNext() const;
		virtual void next();
		virtual Model::Node* create(Model::Model* model, Model::Node* parent);

	private:
		XMLModel* xml;
		int numNodes_;

		void saveNodeFromOldStore(Model::PersistedNode* node);
		Model::LoadedNode loadNode(Model::Node* parent);
};

}

#endif /* SYSTEMCLIPBOARD_H_ */
