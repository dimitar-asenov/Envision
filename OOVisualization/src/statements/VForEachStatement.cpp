/***********************************************************************************************************************
 * VForEachStatement.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VForEachStatement.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/icons/SVGIcon.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VForEachStatement)

VForEachStatement::VForEachStatement(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<PanelBorderLayout>, ForEachStatement>(parent, node, style),
	header(new SequentialLayout(NULL, &style->header())),
	varContainer(new SequentialLayout(NULL, &style->varContainer())),
	collectionBackground(new SequentialLayout(NULL, &style->collection())),
	varName( new VText(NULL, node->varNameNode(), &style->varName())),
	collection(NULL),
	varType(NULL),
	body( NULL )
{
	layout()->setTop(true);
	layout()->top()->setFirst(header);
	header->append(new SVGIcon(NULL, &style->icon()));
	header->append(varContainer);
	varContainer->append(varName);
	header->append(collectionBackground);
}

VForEachStatement::~VForEachStatement()
{
	// These were automatically deleted by LayoutProvider's destructor
	header = NULL;
	varContainer = NULL;
	collectionBackground = NULL;
	varName = NULL;
	collection = NULL;
	varType = NULL;
	body = NULL;
}

void VForEachStatement::determineChildren()
{
	layout()->synchronizeContent(body, node()->body(), &style()->body());

	collectionBackground->synchronizeFirst(collection, node()->collection());
	varContainer->synchronizeFirst(varType, node()->varType());
	varContainer->synchronizeLast(varName, node()->varNameNode(), &style()->varName());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle(&style()->layout());
	header->setStyle(&style()->header());
	header->at<SVGIcon>(0)->setStyle(&style()->icon());
	varContainer->setStyle( &style()->varContainer() );
	collectionBackground->setStyle( &style()->collection() );
	varName->setStyle( &style()->varName() );
	body->setStyle(&style()->body());
}

}
