/***********************************************************************************************************************
 * VStringLiteral.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VSTRINGLITERAL_H_
#define VSTRINGLITERAL_H_

#include "../oovisualization_api.h"

#include "VStringLiteralStyle.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/LayoutProvider.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/expressions/StringLiteral.h"

namespace Visualization {
	class Symbol;
}

namespace OOVisualization {

class OOVISUALIZATION_API VStringLiteral : public Visualization::ItemWithNode< Visualization::LayoutProvider<>, OOModel::StringLiteral>
{
	ITEM_COMMON(VStringLiteral)

	public:
		VStringLiteral(Item* parent, NodeType *literal, const StyleType *style = Visualization::Styles::item<VStringLiteral>("default"));
		virtual ~VStringLiteral();

	protected:
		virtual void determineChildren();

	private:
		Visualization::Symbol* pre_;
		Visualization::Symbol* post_;
		Visualization::VText* vis_;
};

}

#endif /* VSTRINGLITERAL_H_ */
