/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "VIfStatementCF.h"

using namespace Visualization;
using namespace OOModel;
using namespace Model;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VIfStatementCF, "item")

VIfStatementCF::VIfStatementCF(Item* parent, NodeType* node, const StyleType* style) :
	Super{parent, node, style},
	conditionBackground(), condition(), thenBranch(), elseBranch()
{
}

VIfStatementCF::~VIfStatementCF()
{
	SAFE_DELETE_ITEM(conditionBackground);
	SAFE_DELETE_ITEM(thenBranch);
	SAFE_DELETE_ITEM(elseBranch);

	// These were deleted by the destructors of their backgrounds above.
	condition = nullptr;
}

bool VIfStatementCF::sizeDependsOnParent() const
{
	return false;
}

bool VIfStatementCF::isEmpty() const
{
	return false;
}

void VIfStatementCF::determineChildren()
{
	synchronizeItem(thenBranch, node()->thenBranch(), &style()->thenBranch());
	synchronizeItem(elseBranch, node()->elseBranch(), &style()->elseBranch());

	// Remove nodes which have changed
	if (condition && condition->node() != node()->condition())
	{
		SAFE_DELETE_ITEM(conditionBackground);
		condition = nullptr;
		conditionBackground = nullptr;
	}

	// Create nodes which are present in the tree
	if (!condition && node()->condition())
	{
		conditionBackground = new SequentialLayout{this, &style()->condition()};
		condition = renderer()->render(conditionBackground, node()->condition());
		conditionBackground->append(condition);
	}

	if (conditionBackground) conditionBackground->setStyle( &style()->condition() );
	thenBranch->setStyle(&style()->thenBranch());
	thenBranch->setPreferredBreakExit(preferredBreakExit_);
	thenBranch->setPreferredContinueExit(preferredContinueExit_);
	elseBranch->setStyle(&style()->elseBranch());
	elseBranch->setPreferredBreakExit(preferredBreakExit_);
	elseBranch->setPreferredContinueExit(preferredContinueExit_);
}

void VIfStatementCF::updateGeometry(int, int)
{
	clearConnectors();
	breaks_.clear();
	continues_.clear();

	// First compute the size
	int halfCondition = conditionBackground->widthInParent()/2 + style()->pinLength();
	int leftHalf = halfCondition + thenBranch->entrance().x();
	int rightHalf = halfCondition + elseBranch->widthInParent() - elseBranch->entrance().x();

	int extraLeft = leftHalf - thenBranch->widthInParent() - style()->pinLength();
	if (extraLeft < 0)
	{
		leftHalf += -extraLeft;
		extraLeft = 0;
	}

	int extraRight = rightHalf - elseBranch->widthInParent() - style()->pinLength();
	if (extraRight < 0)
	{
		rightHalf += -extraRight;
		extraRight = 0;
	}

	int height = conditionBackground->heightInParent() + style()->pinLength();
	int branchesTop = height;
	height += ( thenBranch->heightInParent() > elseBranch->heightInParent() )
			? thenBranch->heightInParent() : elseBranch->heightInParent();
	height += 3*style()->pinLength();

	// Set the size
	if ( hasShape() ) getShape()->setInnerSize(leftHalf + rightHalf, height);
	else setSize(leftHalf + rightHalf, height);

	// Set the positions
	conditionBackground->setPos(leftHalf - conditionBackground->widthInParent()/2, style()->pinLength());
	thenBranch->setPos(0, branchesTop);
	elseBranch->setPos(leftHalf + extraRight + style()->pinLength(), branchesTop);
	entrance_ = QPoint(leftHalf, 0);

	// Put connectors
	addConnector(leftHalf, 0, leftHalf, style()->pinLength(), true);

	// Then
	addConnector(conditionBackground->pos().x(), style()->pinLength() + conditionBackground->heightInParent()/2,
			thenBranch->entrance().x(), style()->pinLength() + conditionBackground->heightInParent()/2, false);
	addToLastConnector(thenBranch->pos().toPoint() + thenBranch->entrance());

	// Else
	addConnector(conditionBackground->pos().x() + conditionBackground->widthInParent(),
			style()->pinLength() + conditionBackground->heightInParent()/2,
			elseBranch->pos().toPoint().x() + elseBranch->entrance().x(),
			style()->pinLength() + conditionBackground->heightInParent()/2, false);
	addToLastConnector(elseBranch->pos().toPoint() + elseBranch->entrance());

	// Process Connectors on the then branch
	int thenBranchInnerBegin = height; // Height indicates no connectors on the inner side
	for (int i = 0; i < thenBranch->breaks().size(); ++i)
	{
		QPoint p = thenBranch->breaks().at(i);
		if (p.x() == 0) breaks_.append(thenBranch->pos().toPoint() + p);
		else if (thenBranch->pos().y() + p.y() < thenBranchInnerBegin)
			thenBranchInnerBegin = thenBranch->pos().y() + p.y();
	}
	for (int i = 0; i < thenBranch->continues().size(); ++i)
	{
		QPoint p = thenBranch->continues().at(i);
		if (p.x() == 0) continues_.append(thenBranch->pos().toPoint() + p);
		else if (thenBranch->pos().y() + p.y() < thenBranchInnerBegin)
			thenBranchInnerBegin = thenBranch->pos().y() + p.y();
	}

	// Process Connectors on the else branch
	int elseBranchInnerBegin = height; // Height indicates no connectors on the inner side
	for (int i = 0; i < elseBranch->breaks().size(); ++i)
	{
		QPoint p = elseBranch->breaks().at(i);
		if (p.x() > 0) breaks_.append(QPoint(1, elseBranch->pos().y() + p.y()));
		else if (elseBranch->pos().y() + p.y() < elseBranchInnerBegin)
			elseBranchInnerBegin = elseBranch->pos().y() + p.y();
	}
	for (int i = 0; i < elseBranch->continues().size(); ++i)
	{
		QPoint p = elseBranch->continues().at(i);
		if (p.x() > 0) continues_.append(QPoint(1, elseBranch->pos().y() + p.y()));
		else if (elseBranch->pos().y() + p.y() < elseBranchInnerBegin)
			elseBranchInnerBegin = elseBranch->pos().y() + p.y();
	}

	// If there are any break or continue statements on the inside put the corresponding connectors
	if (thenBranchInnerBegin < height)
	{
		addConnector(thenBranch->widthInParent(), thenBranchInnerBegin, thenBranch->widthInParent(),
				height - 3*style()->pinLength(), false);
		addToLastConnector(widthInParent(), height - 3*style()->pinLength());

		QPoint c = QPoint(1, height - 3*style()->pinLength());
		if (preferredBreakExit_ == ControlFlowItem::EXIT_RIGHT) breaks_.append(c);
		else continues_.append(c);
	}

	if (elseBranchInnerBegin < height)
	{
		addConnector(elseBranch->pos().x(), elseBranchInnerBegin, elseBranch->pos().x(),
				height - 2*style()->pinLength(), false);
		addToLastConnector(0, height - 2*style()->pinLength());

		QPoint c = QPoint(0, height - 2*style()->pinLength());
		if (preferredBreakExit_ == ControlFlowItem::EXIT_LEFT) breaks_.append(c);
		else continues_.append(c);
	}

	// Handle the exit and its connectors
	if (thenBranch->exit().isNull() && elseBranch->exit().isNull()) exit_ = QPoint(0, 0);
	else if (thenBranch->exit().isNull())
	{
		exit_ = QPoint(elseBranch->pos().x() + elseBranch->exit().x(), height);
		addConnector(elseBranch->pos().toPoint() + elseBranch->exit(), exit_, false);
	}
	else if (elseBranch->exit().isNull())
	{
		exit_ = QPoint(thenBranch->exit().x(), height);
		addConnector(thenBranch->pos().toPoint() + thenBranch->exit(), exit_, false);
	}
	else
	{
		exit_ = QPoint(entrance_.x(), height);
		int lineHeight = height - style()->pinLength();
		addConnector(exit_ - QPoint(0, style()->pinLength()), exit_, false);
		addConnector(thenBranch->pos().toPoint() + thenBranch->exit(),
				QPoint(thenBranch->pos().x() + thenBranch->exit().x(), lineHeight), false);
		addToLastConnector(elseBranch->pos().x() + elseBranch->exit().x(), lineHeight);
		addToLastConnector(elseBranch->pos().toPoint() + elseBranch->exit());
	}

}

}
