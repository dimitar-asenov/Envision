/***********************************************************************************************************************
 * VModule.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/VModule.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/PositionLayout.h"
#include "VisualizationBase/headers/layouts/PositionLayoutStyle.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/icons/SVGIcon.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VModule)

VModule::VModule(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<PanelBorderLayout>, Module>(parent, node, style),
	header( new SequentialLayout(NULL, &style->header())),
	name( new VText(NULL, node->nameNode(), &style->name())),
	content(new PositionLayout(NULL, &style->content()))
{
	layout()->setTop(true);
	layout()->top()->setMiddle(header);
	header->append(new SVGIcon(NULL, &style->icon()));
	header->append(name);
	layout()->setContent(content);
}

VModule::~VModule()
{
	// These were automatically deleted by LayoutProvider's destructor
	header = NULL ;
	name = NULL;
	content = NULL;
}

void VModule::determineChildren()
{
	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	header->setStyle(&style()->header());
	name->setStyle(&style()->name());
	content->setStyle(&style()->content());
	layout()->setStyle(&style()->layout());
	header->at<SVGIcon>(0)->setStyle(&style()->icon());

	QList<Model::Node*> nodes;
	for (int k = 0; k<node()->modules()->size(); ++k) nodes.append(node()->modules()->at(k));
	for (int k = 0; k<node()->classes()->size(); ++k) nodes.append(node()->classes()->at(k));
	content->synchronizeWithNodes(nodes, renderer());
}

}
