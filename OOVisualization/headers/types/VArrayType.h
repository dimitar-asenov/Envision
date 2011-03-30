/***********************************************************************************************************************
 * VArrayType.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VARRAYTYPE_H_
#define VARRAYTYPE_H_

#include "../oovisualization_api.h"

#include "OOModel/headers/types/ArrayType.h"

#include "VArrayTypeStyle.h"
#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"

namespace Visualization {
	class Static;
}

namespace OOVisualization {

class OOVISUALIZATION_API VArrayType : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::ArrayType>
{
	ITEM_COMMON(VArrayType)

	public:
		VArrayType(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VArrayType();


	protected:
		void determineChildren();

	private:
		Visualization::Static* symbol_;
		Visualization::Item* type_;
};

}

#endif /* VARRAYTYPE_H_ */
