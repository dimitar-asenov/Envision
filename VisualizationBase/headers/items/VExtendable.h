/***********************************************************************************************************************
 * VExtendable.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VEXTENDABLE_H_
#define VEXTENDABLE_H_

#include "visualizationbase_api.h"

#include "ModelItem.h"
#include "layouts/SequentialLayout.h"
#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"

namespace Visualization {

class VISUALIZATIONBASE_API VExtendable : public ModelItem
{
	private:
		SequentialLayout header;
		SequentialLayout attributes;

	public:
		VExtendable(Item* parent, Model::ExtendableNode* node);

		void determineChildren();
		void updateState();
};

}

#endif /* VEXTENDABLE_H_ */
