/***********************************************************************************************************************
 * VArrayType.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VARRAYTYPE_H_
#define VARRAYTYPE_H_

#include "../oovisualization_api.h"

#include "VArrayTypeStyle.h"
#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/types/ArrayType.h"

namespace Visualization {
	class Symbol;
	class SequentialLayout;
}

namespace OOModel {
	class ArrayType;
}

namespace OOVisualization {

class OOVISUALIZATION_API VArrayType : public Visualization::ModelItem
{
	ITEM_COMMON(VArrayType, Visualization::ModelItem)

	public:
		VArrayType(Item* parent, OOModel::ArrayType* node, const VArrayTypeStyle* style = Visualization::Styles::item<VArrayType>("default"));
		virtual ~VArrayType();

		virtual bool focusChild(FocusTarget location);

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* container_;
		Visualization::Symbol* symbol_;
		Visualization::ModelItem* type_;
};

}

#endif /* VARRAYTYPE_H_ */
