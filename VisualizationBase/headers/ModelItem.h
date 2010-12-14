/***********************************************************************************************************************
 * ModelItem.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODELITEM_H_
#define MODELITEM_H_

#include "Item.h"
#include "ModelRenderer.h"
#include "ModelScene.h"
#include "ModelBase/headers/nodes/Node.h"

namespace Visualization {

class ModelItem: public Item
{
	private:
		Model::Node* node;
		int revision;

	public:
		ModelItem(Item* parent, Model::Node* node, Shape *shape = NULL);

		bool needsUpdate();
		void updateSubtreeState();

		Model::Node* getNode();

		ModelRenderer* renderer();
};

inline Model::Node* ModelItem::getNode() { return node; };
inline ModelRenderer* ModelItem::renderer() { return (static_cast<ModelScene*>(scene()))->renderer();} ;
}

#endif /* MODELITEM_H_ */
