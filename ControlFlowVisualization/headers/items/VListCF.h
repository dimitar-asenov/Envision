/***********************************************************************************************************************
 * VListCF.h
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLISTCF_H_
#define VLISTCF_H_

#include "../controlflowvisualization_api.h"
#include "VListCFStyle.h"
#include "ControlFlowItem.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

#include "ModelBase/headers/nodes/List.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VListCF : public Visualization::ItemWithNode< ControlFlowItem, Model::List>
{
	ITEM_COMMON(VListCF)

	public:
		VListCF(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VListCF();

		virtual bool focusChild(FocusTarget location);
		virtual bool isEmpty() const;

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		QVector< Visualization::Item* > items_;

		QList< Visualization::Item* > extractSingleItems();
		void buildCompositeItems( QList< Visualization::Item* >& singleItems );
};

}

#endif /* VLISTCF_H_ */
