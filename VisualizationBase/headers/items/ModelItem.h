/***********************************************************************************************************************
 * ModelItem.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODELITEM_H_
#define MODELITEM_H_

#include "visualizationbase_api.h"

#include "Item.h"
#include "ModelRenderer.h"
#include "ModelBase/headers/nodes/Node.h"

namespace Visualization {

class VISUALIZATIONBASE_API ModelItem: public Item
{
	private:
		Model::Node* node;
		int revision;

	public:
		ModelItem(Item* parent, Model::Node* node, const ItemStyle *style = NULL);

		virtual void updateSubtreeState();
		virtual bool needsUpdate();

		Model::Node* getNode();

		ModelRenderer* renderer();
};

inline Model::Node* ModelItem::getNode() { return node; };

}

#endif /* MODELITEM_H_ */
