/***********************************************************************************************************************
 * VIfStatementCF.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VIfStatementCF.h"

using namespace OOVisualization;
using namespace Visualization;
using namespace OOModel;
using namespace Model;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VIfStatementCF)

VIfStatementCF::VIfStatementCF(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<ControlFlowItem, IfStatement>(parent, node, style),
	conditionBackground(NULL),
	condition(NULL),
	thenBranch( NULL ),
	elseBranch(NULL),
	vis_(NULL)
{
}

VIfStatementCF::~VIfStatementCF()
{
	SAFE_DELETE_ITEM(conditionBackground);
	SAFE_DELETE_ITEM(thenBranch);
	SAFE_DELETE_ITEM(elseBranch);
	SAFE_DELETE_ITEM(vis_);

	// These were deleted by the destructors of their backgrounds above.
	condition = NULL;
}

bool VIfStatementCF::sizeDependsOnParent() const
{
	if (showAsControlFlow()) return false;
	else return vis_->sizeDependsOnParent();
}

bool VIfStatementCF::isEmpty() const
{
	if (showAsControlFlow()) return false;
	else return vis_->isEmpty();
}

bool VIfStatementCF::focusChild(FocusTarget location)
{
	if (showAsControlFlow())
	{
		//TODO implement this
		return false;
	}
	else return vis_->focusChild(location);
}

void VIfStatementCF::determineChildren()
{
	if (showAsControlFlow())
	{
		SAFE_DELETE_ITEM(vis_);

		synchronizeItem(thenBranch, node()->thenBranch(), &style()->thenBranch());
		synchronizeItem(elseBranch, node()->elseBranch(), &style()->elseBranch());

		// Remove nodes which have changed
		if (condition && condition->node() != node()->condition())
		{
			SAFE_DELETE_ITEM(conditionBackground);
			condition = NULL;
			conditionBackground = NULL;
		}

		// Create nodes which are present in the model
		if (!condition && node()->condition())
		{
			conditionBackground = new SequentialLayout(NULL, &style()->condition());
			condition = renderer()->render(NULL, node()->condition());
			conditionBackground->append(condition);
			conditionBackground->setParentItem(this);
		}

		if (conditionBackground) conditionBackground->setStyle( &style()->condition() );
		thenBranch->setStyle(&style()->thenBranch());
		thenBranch->setPreferredBreakExit(preferredBreakExit_);
		thenBranch->setPreferredContinueExit(preferredContinueExit_);
		elseBranch->setStyle(&style()->elseBranch());
		elseBranch->setPreferredBreakExit(preferredBreakExit_);
		elseBranch->setPreferredContinueExit(preferredContinueExit_);
	}
	else
	{
		SAFE_DELETE_ITEM(conditionBackground);
		SAFE_DELETE_ITEM(thenBranch);
		SAFE_DELETE_ITEM(thenBranch);

		// These were deleted by the destructors of their backgrounds above.
		condition = NULL;

		synchronizeItem<VIfStatement>(vis_, node(), NULL);
	}
}

void VIfStatementCF::updateGeometry(int availableWidth, int availableHeight)
{
	clearConnectors();

	if (!showAsControlFlow())
	{
		Item::updateGeometry(vis_, availableWidth, availableHeight);
		return;
	}

	// First compute the size
	int halfCondition = conditionBackground->width()/2 + style()->pinLength();
	int leftHalf = halfCondition + thenBranch->entrance().x();
	int rightHalf = halfCondition + elseBranch->width() - elseBranch->entrance().x();

	int extraLeft = leftHalf - thenBranch->width() - style()->pinLength();
	if (extraLeft < 0)
	{
		leftHalf += -extraLeft;
		extraLeft = 0;
	}

	int extraRight = rightHalf - elseBranch->width() - style()->pinLength();
	if (extraRight < 0)
	{
		rightHalf += -extraRight;
		extraRight = 0;
	}

	int height = conditionBackground->height() + style()->pinLength();
	int branchesTop = height;
	height += ( thenBranch->height() > elseBranch->height() ) ? thenBranch->height() : elseBranch->height() ;
	height += 2*style()->pinLength();

	// Set the size
	if ( hasShape() ) getShape()->setInnerSize(leftHalf + rightHalf, height);
	else setSize(leftHalf + rightHalf, height);

	// Set the positions
	conditionBackground->setPos(leftHalf - conditionBackground->width()/2,style()->pinLength());
	thenBranch->setPos(0, branchesTop);
	elseBranch->setPos(leftHalf + extraRight + style()->pinLength(), branchesTop);
	entrance_ = QPoint(leftHalf, 0);

	// Put connectors
	addConnector(leftHalf, 0, leftHalf, style()->pinLength(), true);

	// Then
	addConnector(conditionBackground->pos().x(), style()->pinLength() + conditionBackground->height()/2,
			thenBranch->entrance().x(), style()->pinLength() + conditionBackground->height()/2, false);
	addToLastConnector(thenBranch->pos().toPoint() + thenBranch->entrance());

	// Else
	addConnector(conditionBackground->pos().x() + conditionBackground->width(), style()->pinLength() + conditionBackground->height()/2,
			elseBranch->pos().toPoint().x() + elseBranch->entrance().x(), style()->pinLength() + conditionBackground->height()/2, false);
	addToLastConnector(elseBranch->pos().toPoint() + elseBranch->entrance());

	// Process Connectors on the then Branch
	int thenBranchInnerBegin = height; // Height indicates no connectors on the inner side
	for (int i = 0; i < thenBranch->breaks().size(); ++i)
	{
		QPoint br = thenBranch->breaks().at(i);
		if (br.x() == 0) breaks_.append(thenBranch->pos().toPoint() + br);
		else if (thenBranch->pos().x() + br.x() < thenBranchInnerBegin)
			thenBranchInnerBegin = thenBranch->pos().x() + br.x();
	}

}

}
