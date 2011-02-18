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

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/Styles.h"

namespace Visualization {
	class VText;
	class SequentialLayout;
}

namespace OOModel {
	class FormalResult;
}

namespace OOVisualization {

class OOVISUALIZATION_API VFormalResult : public Visualization::ModelItem
{
	ITEM_COMMON(VFormalResult)

	public:
		VFormalResult(Item* parent, OOModel::FormalResult* node, const VFormalResultStyle* style = Visualization::Styles::item<VFormalResult>("default"));
		virtual ~VFormalResult();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::VText* name_;
		Visualization::ModelItem* type_;
};

}

#endif /* VFORMALRESULT_H_ */
