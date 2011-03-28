/***********************************************************************************************************************
 * TextAndDescription.h
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEXTANDDESCRIPTION_H_
#define TEXTANDDESCRIPTION_H_

#include "interactionbase_api.h"
#include "TextAndDescriptionStyle.h"

#include "VisualizationBase/headers/items/Item.h"
#include "VisualizationBase/headers/items/Text.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace Interaction {

class INTERACTIONBASE_API TextAndDescription : public Visualization::Item
{
	ITEM_COMMON(TextAndDescription)

	public:
		TextAndDescription(Item* parent, const StyleType* style = itemStyles().get());
		virtual ~TextAndDescription();

		virtual bool sizeDependsOnParent() const;

		void setContents(const QString& text, const QString& description);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::SequentialLayout* layout;
		Visualization::Text* text;
		Visualization::Text* description;
};

}

#endif /* TEXTANDDESCRIPTION_H_ */
