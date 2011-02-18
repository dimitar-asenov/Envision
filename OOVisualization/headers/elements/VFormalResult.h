/***********************************************************************************************************************
 * VFormalResult.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFORMALRESULT_H_
#define VFORMALRESULT_H_

#include "../oovisualization_api.h"
#include "VFormalResultStyle.h"

#include "OOModel/headers/elements/FormalResult.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
}

namespace OOVisualization {

class OOVISUALIZATION_API VFormalResult : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::FormalResult>
{
	ITEM_COMMON(VFormalResult)

	public:
		VFormalResult(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VFormalResult>("default"));
		virtual ~VFormalResult();

	protected:
		void determineChildren();

	private:
		Visualization::VText* name_;
		Visualization::Item* type_;
};

}

#endif /* VFORMALRESULT_H_ */
