/***********************************************************************************************************************
 * VForEachStatement.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VForEachStatement.h"

#include "OOModel/headers/statements/ForEachStatement.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/icons/SVGIcon.h"

using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VForEachStatement)

VForEachStatement::VForEachStatement(Item* parent, OOModel::ForEachStatement* node, const VForEachStatementStyle* style) :
	ModelItem(parent, node, style),
	layout(new PanelBorderLayout(this, &style->layout())),
	header(new SequentialLayout(NULL, &style->header())),
	varContainer(new SequentialLayout(NULL, &style->varContainer())),
	collectionBackground(new SequentialLayout(NULL, &style->collection())),
	varName( new VText(NULL, node->varNameNode(), &style->varName())),
	collection(NULL),
	varType(NULL),
	body(NULL)
{
	layout->setTop(true);
	layout->top()->setFirst(header);
	header->append(new SVGIcon(NULL, &style->icon()));
	header->append(varContainer);
	varContainer->append(varName);
	header->append(collectionBackground);
}

VForEachStatement::~VForEachStatement()
{
	SAFE_DELETE_ITEM(layout);

	// These were automatically deleted by layout's destructor
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
	OOModel::ForEachStatement* node = static_cast<OOModel::ForEachStatement*> (getNode());

	if (!body)
	{
		body = renderer()->render(NULL, node->body());
		layout->setContent(body);
	}

	if (!collection)
	{
		collection = renderer()->render(NULL, node->collection());
		collectionBackground->append(collection);
	}

	// Remove nodes which have changed
	if (varType && varType->getNode() != node->varType())
	{
		varContainer->removeAll(varType);
		varType = NULL;
	}

	// Create nodes which are present in the model
	if (!varType && node->varType())
	{
		varType = renderer()->render(NULL, node->varType());
		varContainer->prepend(varType);
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout->setStyle(&style()->layout());
	header->setStyle(&style()->header());
	header->at<SVGIcon>(0)->setStyle(&style()->icon());
	varContainer->setStyle( &style()->varContainer() );
	collectionBackground->setStyle( &style()->collection() );
	varName->setStyle( &style()->varName() );
}

void VForEachStatement::updateGeometry(int, int)
{
	if ( hasShape() )
	{
		getShape()->setOffset(layout->getXOffsetForExternalShape(), layout->getYOffsetForExternalShape());
		getShape()->setOutterSize(layout->getOutterWidthForExternalShape(), layout->getOutterHeightForExternalShape());
	}
	setSize( layout->size() );
}

bool VForEachStatement::focusChild(FocusTarget location)
{
	return layout->focusChild(location);
}

}
