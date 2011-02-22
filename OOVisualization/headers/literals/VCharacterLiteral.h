/***********************************************************************************************************************
 * VCharacterLiteral.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCHARACTERLITERAL_H_
#define VCHARACTERLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ItemWithNode.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/items/VCharacter.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/expressions/CharacterLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VCharacterLiteral : public Visualization::ItemWithNode< Visualization::Item, OOModel::CharacterLiteral>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VCharacterLiteral, Visualization::TextStyle)

	public:
		VCharacterLiteral(Item* parent, NodeType* literal, const StyleType* style = Visualization::Styles::item<VCharacterLiteral>("default"));
		virtual ~VCharacterLiteral();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::VCharacter* vis_;
};

}

#endif /* VCHARACTERLITERAL_H_ */
