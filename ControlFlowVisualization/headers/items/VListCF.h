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
#include "VisualizationBase/headers/Styles.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

#include "ModelBase/headers/nodes/List.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VListCF : public Visualization::ItemWithNode< ControlFlowItem, Model::List>
{
	ITEM_COMMON(VListCF)

	public:
		VListCF(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VListCF>("default"));
		virtual ~VListCF();

		virtual bool focusChild(FocusTarget location);
		virtual bool isEmpty() const;

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		QVector< Visualization::Item* > items_;

		void appendSingle( Visualization::Item* item);
		void insertSingle( Visualization::Item* item, int pos);
		Visualization::Item* atSingle(int pos);
		void swapSingle(int i, int j);
		void removeLastSingle();
		int lengthSingle();
};

}

#endif /* VLISTCF_H_ */
