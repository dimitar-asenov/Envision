/***********************************************************************************************************************
 * VModule.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VModule.h"

#include "OOModel/headers/Module.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/PositionLayout.h"
#include "VisualizationBase/headers/layouts/PositionLayoutStyle.h"
#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VModule)

VModule::VModule(Item* parent, OOModel::Module* node, const VModuleStyle* style) :
	ModelItem(parent, node, style), header( new VText(NULL, node->nameNode(), &style->headerStyle())),
	layout(new PanelBorderLayout(this, &style->borderStyle())),
	content(new PositionLayout(NULL, &style->contentStyle()))
{
	layout->setTop(true);
	layout->top()->setMiddle(header);
	layout->setContent(content);
}

VModule::~VModule()
{
	SAFE_DELETE_ITEM(layout);

	// These were automatically deleted by layout's destructor
	header = NULL ;
	content = NULL;
}

void VModule::determineChildren()
{
	OOModel::Module* node = static_cast<OOModel::Module*> (getNode());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	header->setStyle(&style()->headerStyle());
	content->setStyle(&style()->contentStyle());
	layout->setStyle(&style()->borderStyle());

	// Remove elements from attributes which are outdated
	for(int i = content->length() - 1; i>=0; --i)
	{
		bool found = false;

		// Modules
		if (!found)
			for (int k = 0; k<node->modules()->size(); ++k)
				if (node->modules()->at(k) == content->at<ModelItem>(i)->getNode())
				{
					found = true;
					break;
				}

		// Classes
		if (!found)
			for (int k = 0; k<node->classes()->size(); ++k)
				if (node->classes()->at(k) == content->at<ModelItem>(i)->getNode())
				{
					found = true;
					break;
				}

		if (!found) content->remove(i);
	}

	// Add new Modules
	for (int k = 0; k<node->modules()->size(); ++k)
		if ( !content->hasNode( node->modules()->at(k) ) )
			content->insert(renderer()->render(NULL, node->modules()->at(k)));

	// Add new Classes
	for (int k = 0; k<node->classes()->size(); ++k)
		if ( !content->hasNode( node->classes()->at(k) ) )
			content->insert(renderer()->render(NULL, node->classes()->at(k)));
}

void VModule::updateGeometry(int, int)
{
	if ( hasShape() )
	{
		getShape()->setOffset(layout->getXOffsetForExternalShape(), layout->getYOffsetForExternalShape());
		getShape()->setOutterSize(layout->getOutterWidthForExternalShape(), layout->getOutterHeightForExternalShape());
	}
	setSize( layout->size() );
}

bool VModule::focusChild(FocusTarget location)
{
	return layout->focusChild(location);
}

}
