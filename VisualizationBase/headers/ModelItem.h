/***********************************************************************************************************************
 * ModelItem.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODELITEM_H_
#define MODELITEM_H_

#include "Item.h"
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
};

}

#endif /* MODELITEM_H_ */
