/***********************************************************************************************************************
 * TextAndDescription.cpp
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/TextAndDescription.h"

namespace Interaction {

ITEM_COMMON_DEFINITIONS(TextAndDescription)

using namespace Visualization;

TextAndDescription::TextAndDescription(Item* parent, const StyleType* style) :
	Item(parent, style), layout(new SequentialLayout(this, &style->layout())), text( new Text(NULL, &style->text())),
	description( new Text(NULL, &style->description()))
{
	layout->append(text);
	layout->append(description);
}
TextAndDescription::~TextAndDescription()
{
	SAFE_DELETE_ITEM(layout);

	// These will automatically be deleted by layout's destructor
	text = NULL;
	description = NULL;
}

bool TextAndDescription::sizeDependsOnParent() const
{
	return true;
}

void TextAndDescription::setContents(const QString& text_, const QString& description_)
{
	text->setText(text_);
	description->setText(description_);
}

void TextAndDescription::determineChildren()
{
}

void TextAndDescription::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(layout, availableWidth, availableHeight);
}

}
