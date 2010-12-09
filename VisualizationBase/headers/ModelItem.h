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

	public:
		ModelItem(Item* parent, Model::Node* node);
};

}

#endif /* MODELITEM_H_ */
