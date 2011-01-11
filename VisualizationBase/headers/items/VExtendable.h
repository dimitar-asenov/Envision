/***********************************************************************************************************************
 * VExtendable.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VEXTENDABLE_H_
#define VEXTENDABLE_H_

#include "../visualizationbase_api.h"

#include "ModelItem.h"
#include "../Styles.h"
#include "VExtendableStyle.h"
#include "../layouts/SequentialLayout.h"
#include "../layouts/PanelBorderLayout.h"
#include "ModelBase/headers/nodes/Extendable/ExtendableNode.h"

namespace Visualization {

class VISUALIZATIONBASE_API VExtendable : public ModelItem
{
	ITEM_COMMON(VExtendable, ModelItem)

	private:
		SequentialLayout header;
		PanelBorderLayout* layout; //only used when expanded
		SequentialLayout* attributes; //only used when expanded

		bool expandedSwtiched() const;

	protected:
		void determineChildren();
		void updateState();

	public:
		VExtendable(Item* parent, Model::ExtendableNode* node, const VExtendableStyle* style = Styles::item<VExtendable>("default"));
		virtual ~VExtendable();

		void setExpanded(bool expanded = true);

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

}

#endif /* VEXTENDABLE_H_ */
