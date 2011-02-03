/***********************************************************************************************************************
 * VProject.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/top_level/VProject.h"

#include "OOModel/headers/Project.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/PositionLayout.h"
#include "VisualizationBase/headers/layouts/PositionLayoutStyle.h"
#include "VisualizationBase/headers/items/VText.h"

using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VProject)

VProject::VProject(Item* parent, OOModel::Project* node, const VProjectStyle* style) :
	ModelItem(parent, node, style), header( new VText(NULL, node->nameNode(), &style->headerStyle())),
	layout(new PanelBorderLayout(this, &style->borderStyle())),
	content(new PositionLayout(NULL, &style->contentStyle()))
{
	layout->setTop(true);
	layout->top()->setMiddle(header);
	layout->setContent(content);
}

VProject::~VProject()
{
	SAFE_DELETE_ITEM(layout);

	// These were automatically deleted by layout's destructor
	header = NULL ;
	content = NULL;
}

void VProject::determineChildren()
{
	OOModel::Project* node = static_cast<OOModel::Project*> (getNode());

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

		// Projects
		for (int k = 0; k<node->projects()->size(); ++k)
			if (node->projects()->at(k) == content->at<ModelItem>(i)->getNode())
			{
				found = true;
				break;
			}

		// Libraries
		if (!found)
			for (int k = 0; k<node->libraries()->size(); ++k)
				if (node->libraries()->at(k) == content->at<ModelItem>(i)->getNode())
				{
					found = true;
					break;
				}

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

	// Add new projects
	for (int k = 0; k<node->projects()->size(); ++k)
		if ( !content->hasNode( node->projects()->at(k) ) )
			content->insert(renderer()->render(NULL, node->projects()->at(k)));

	// Add new Libraries
	for (int k = 0; k<node->libraries()->size(); ++k)
		if ( !content->hasNode( node->libraries()->at(k) ) )
			content->insert(renderer()->render(NULL, node->libraries()->at(k)));

	// Add new Modules
	for (int k = 0; k<node->modules()->size(); ++k)
		if ( !content->hasNode( node->modules()->at(k) ) )
			content->insert(renderer()->render(NULL, node->modules()->at(k)));

	// Add new Classes
	for (int k = 0; k<node->classes()->size(); ++k)
		if ( !content->hasNode( node->classes()->at(k) ) )
			content->insert(renderer()->render(NULL, node->classes()->at(k)));
}

void VProject::updateGeometry(int availableWidth, int availableHeight)
{
	ModelItem::updateGeometry(layout, availableWidth, availableHeight);
}

bool VProject::focusChild(FocusTarget location)
{
	return layout->focusChild(location);
}

}
