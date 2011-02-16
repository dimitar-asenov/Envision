/***********************************************************************************************************************
 * VIfStatement.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VIfStatement.h"

#include "OOModel/headers/statements/IfStatement.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/icons/SVGIcon.h"

using namespace Visualization;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VIfStatement)

VIfStatement::VIfStatement(Item* parent, OOModel::IfStatement* node, const VIfStatementStyle* style) :
	ModelItem(parent, node, style),
	layout(new PanelBorderLayout(this, &style->layout())),
	header(new SequentialLayout(NULL, &style->header())),
	conditionBackground(new SequentialLayout(NULL, &style->condition())),
	condition(NULL),
	content(new SequentialLayout(NULL, &style->content())),
	thenBranchBackground(new SequentialLayout(NULL, &style->thenBranch())),
	elseBranchBackground(NULL),
	thenBranch(NULL),
	elseBranch(NULL)
{
	layout->setTop(true);
	layout->top()->setFirst(header);
	header->append(new SVGIcon(NULL, &style->icon()));
	header->append(conditionBackground);
	layout->setContent(content);
	content->append(thenBranchBackground);
}

VIfStatement::~VIfStatement()
{
	SAFE_DELETE_ITEM(layout);

	// These were automatically deleted by layout's destructor
	header = NULL;
	conditionBackground = NULL;
	condition = NULL;
	content = NULL;
	thenBranchBackground = NULL;
	elseBranchBackground = NULL;
	thenBranch = NULL;
	elseBranch = NULL;
}

void VIfStatement::determineChildren()
{
	OOModel::IfStatement* node = static_cast<OOModel::IfStatement*> (getNode());

	if (!condition)
	{
		condition = renderer()->render(NULL, node->condition());
		conditionBackground->append(condition);
		thenBranch = renderer()->render(NULL, node->thenBranch());
		thenBranchBackground->append(thenBranch);
	}

	if (elseBranch && elseBranch->getNode() != node->elseBranch())
	{
		content->remove(1);
		elseBranch = NULL;
		elseBranchBackground = NULL;
	}

	if (!elseBranch && node->elseBranch())
	{
		elseBranchBackground = new SequentialLayout(NULL, &style()->elseBranch());
		elseBranch = renderer()->render(NULL, node->elseBranch());
		elseBranchBackground->append(elseBranch);
		content->append(elseBranchBackground);
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout->setStyle(&style()->layout());
	header->setStyle(&style()->header());
	header->at<SVGIcon>(0)->setStyle(&style()->icon());
	conditionBackground->setStyle( &style()->condition() );
	content->setStyle(&style()->content());
	thenBranchBackground->setStyle( &style()->thenBranch() );
}

void VIfStatement::updateGeometry(int, int)
{
	if ( hasShape() )
	{
		getShape()->setOffset(layout->getXOffsetForExternalShape(), layout->getYOffsetForExternalShape());
		getShape()->setOutterSize(layout->getOutterWidthForExternalShape(), layout->getOutterHeightForExternalShape());
	}
	setSize( layout->size() );
}

bool VIfStatement::focusChild(FocusTarget location)
{
	return layout->focusChild(location);
}

}
