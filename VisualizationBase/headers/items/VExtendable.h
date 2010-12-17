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
#include "layouts/PanelBorderLayout.h"
#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"

namespace Visualization {

class VISUALIZATIONBASE_API VExtendable : public ModelItem
{
	private:
		PanelBorderLayout layout;
		SequentialLayout header;
		SequentialLayout attributes;

	protected:
		void determineChildren();
		void updateState();

	public:
		VExtendable(Item* parent, Model::ExtendableNode* node);
		static const QString& className();
};

inline const QString& VExtendable::className() {static QString name("VExtendable"); return name;}

}

#endif /* VEXTENDABLE_H_ */
