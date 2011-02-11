/***********************************************************************************************************************
 * VText.cpp
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VText.h"

#include "ModelBase/headers/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(VText)

VText::VText(Item* parent, Model::Text *text, const TextStyle *style) :
	TextRenderer<ModelItem>(parent, text, style)
{
	TextRenderer<ModelItem>::setText(text->get());
}

void VText::setText(const QString& newText)
{
	Model::Text* textNode = static_cast<Model::Text*> (getNode());
	textNode->model()->beginModification(textNode, "Set text");
	textNode->set(newText);
	textNode->model()->endModification();
	TextRenderer<ModelItem>::setText(newText);
}

}
