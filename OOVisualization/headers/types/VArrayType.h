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
#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/types/ArrayType.h"

namespace Visualization {
	class Symbol;
}

namespace OOModel {
	class ArrayType;
}

namespace OOVisualization {

class OOVISUALIZATION_API VArrayType : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::ArrayType>
{
	ITEM_COMMON(VArrayType)

	public:
		VArrayType(Item* parent, NodeType* node, const StyleType* style = Visualization::Styles::item<VArrayType>("default"));
		virtual ~VArrayType();


	protected:
		void determineChildren();

	private:
		Visualization::Symbol* symbol_;
		Visualization::Item* type_;
};

}

#endif /* VARRAYTYPE_H_ */
