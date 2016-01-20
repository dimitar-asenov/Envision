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

#include "VLoopStatementCF.h"

using namespace Visualization;
using namespace OOModel;
using namespace Model;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VLoopStatementCF, "item")

VLoopStatementCF::VLoopStatementCF(Item* parent, NodeType* node, const StyleType* style) :
	Super{parent, node, style},
	conditionBackground(),
	initStepBackground(),
	updateStepBackground(),
	condition(),
	initStep(),
	updateStep(),
	body()
{
}

VLoopStatementCF::~VLoopStatementCF()
{
	SAFE_DELETE_ITEM(conditionBackground);
	SAFE_DELETE_ITEM(initStepBackground);
	SAFE_DELETE_ITEM(updateStepBackground);
	SAFE_DELETE_ITEM(body);

	// These were deleted by the destructors of their backgrounds above.
	condition = nullptr;
	initStep = nullptr;
	updateStep = nullptr;
}

bool VLoopStatementCF::sizeDependsOnParent() const
{
	return false;
}

bool VLoopStatementCF::isEmpty() const
{
	return false;
}

void VLoopStatementCF::determineChildren()
{
	synchronizeItem(body, node()->body(), &style()->body());

	// Remove nodes which have changed
	if (initStep && initStep->node() != node()->initStep())
	{
		SAFE_DELETE_ITEM(initStepBackground);
		initStep = nullptr;
		initStepBackground = nullptr;
	}

	if (condition && condition->node() != node()->condition())
	{
		SAFE_DELETE_ITEM(conditionBackground);
		condition = nullptr;
		conditionBackground = nullptr;
	}

	if (updateStep && updateStep->node() != node()->updateStep())
	{
		SAFE_DELETE_ITEM(updateStepBackground);
		updateStep = nullptr;
		updateStepBackground = nullptr;
	}

	// Create nodes which are present in the tree
	if (!initStep && node()->initStep())
	{
		initStepBackground = new SequentialLayout{this, &style()->initStep()};
		initStep = renderer()->render(initStepBackground, node()->initStep());
		initStepBackground->append(initStep);
	}

	if (!updateStep && node()->updateStep())
	{
		updateStepBackground = new SequentialLayout{this, &style()->updateStep()};
		updateStep = renderer()->render(updateStepBackground, node()->updateStep());
		updateStepBackground->append(updateStep);
	}

	if (!condition && node()->condition())
	{
		conditionBackground = new SequentialLayout{this, &style()->condition()};
		condition = renderer()->render(conditionBackground, node()->condition());
		conditionBackground->append(condition);
	}

	if (conditionBackground) conditionBackground->setStyle( &style()->condition() );
	if (initStepBackground) initStepBackground->setStyle( &style()->initStep() );
	if (updateStepBackground) updateStepBackground->setStyle( &style()->updateStep() );
	body->setStyle(&style()->body());
	body->setPreferredBreakExit(ControlFlowItem::EXIT_LEFT);
	body->setPreferredContinueExit(ControlFlowItem::EXIT_RIGHT);
}

void VLoopStatementCF::updateGeometry(int, int)
{
	clearConnectors();

	QPoint initPos;
	QPoint condPos;
	QPoint updatePos;
	QPoint bodyPos;

	int left = 0;
	int right = 0;

	int y = style()->pinLength(); // Used when computing height

	// Determine the relative position of elements
	if (initStep)
	{
		left = - initStepBackground->widthInParent()/2;
		initPos = QPoint(left, y);
		right = initPos.x() + initStepBackground->widthInParent();

		y += initStepBackground->heightInParent();
	}

	y += style()->pinLength();

	if (updateStep)
	{
		updatePos = QPoint( 2*style()->pinLength(), y);

		if (right < updatePos.x() + updateStepBackground->widthInParent())
			right = updatePos.x() + updateStepBackground->widthInParent();

		y += updateStepBackground->heightInParent();
	}

	y += style()->pinLength();

	if (condition)
	{
		condPos = QPoint(- conditionBackground->widthInParent()/2, y);

		if (left > condPos.x()) left = condPos.x();
		if (right < condPos.x() + conditionBackground->widthInParent())
					right = condPos.x() + conditionBackground->widthInParent();

		y += conditionBackground->heightInParent();
	}

	if (body)
	{
		bodyPos = QPoint( 2*style()->pinLength(), y);
		if (right < bodyPos.x() + body->widthInParent() + 2*style()->pinLength() + style()->pin().width())
					right = bodyPos.x() + body->widthInParent() + 2*style()->pinLength() + style()->pin().width();

		y += body->heightInParent() + style()->pinLength() + style()->pin().width();
	}

	// Set the size
	if ( hasShape() ) getShape()->setInnerSize(right - left, y);
	else setSize(right - left, y);

	// Set the control flow elements
	entrance_ = QPoint(-left, 0);
	exit_ = QPoint( -left, y);

	// Set the positions
	if (initStep) initStepBackground->setPos(initPos - QPoint(left, 0));
	if (updateStep) updateStepBackground->setPos(updatePos - QPoint(left, 0));
	if (condition) conditionBackground->setPos(condPos - QPoint(left, 0));
	if (body) body->setPos(bodyPos - QPoint(left, 0));

	// Put connectors
	y = style()->pinLength();
	if (initStep)
	{
		addConnector(entrance_, entrance_ + QPoint(0, y), true);
		y += initStepBackground->heightInParent();
	}

	addConnector(entrance_ + QPoint(0, y), entrance_ + QPoint(0, y+style()->pinLength()), false);
	y += style()->pinLength();

	QPoint updateConnect(entrance_.x() + style()->pinLength(), y);

	if (updateStep)
	{
		updateConnect.ry() += updateStepBackground->heightInParent()/2;
		addConnector(updateConnect + QPoint(style()->pinLength(), 0),
				updateConnect - QPoint(style()->pinLength(), 0), true);

		updateConnect.rx() += updateStepBackground->widthInParent() + 2*style()->pinLength();

		addConnector(entrance_.x(), y, entrance_.x(), y + updateStepBackground->heightInParent(), false);

		y += updateStepBackground->heightInParent();
	}

	addConnector(updateConnect, updateConnect - QPoint(style()->pinLength(), 0), true);

	QPoint condTrueConnect;
	QPoint condFalseConnect;

	if (condition)
	{
		addConnector(entrance_.x(), y, entrance_.x(), y + style()->pinLength(), true);
		y += style()->pinLength();

		condTrueConnect.setY(y + conditionBackground->heightInParent()/2);
		condTrueConnect.setX(condPos.x() - left + conditionBackground->widthInParent() + style()->pinLength());

		addConnector(condTrueConnect, condTrueConnect - QPoint(style()->pinLength(), 0), false);

		y += conditionBackground->heightInParent();

		addConnector(entrance_.x(), y, entrance_.x(), y + style()->pinLength(), false);
		y += style()->pinLength();

		condFalseConnect.setY(y);
		condFalseConnect.setX(entrance_.x());
	}

	int lowestContinue = 0;
	if (body)
	{
		QPoint bPos = body->pos().toPoint();

		addConnector(condTrueConnect, QPoint(body->entrance().x() + bPos.x(), condTrueConnect.y()), false );
		addToLastConnector(body->entrance() + bPos);

		int r = right - left - style()->pin().width();
		// Process breaks and continues on the right sides.
		for (int i = 0; i < body->breaks().size(); ++i)
		{
			QPoint br = body->breaks().at(i);
			if (br.x() == 0) addConnector(bPos + br, QPoint(condFalseConnect.x(), (bPos + br).y()), true );
		}
		for (int i = 0; i < body->continues().size(); ++i)
		{
			QPoint cont = body->continues().at(i);
			if (cont.x() > 0)
				addConnector(bPos.x() + body->widthInParent(), bPos.y() + cont.y(), r, bPos.y() + cont.y(), true );
			if (cont.y() + body->pos().y() > lowestContinue) lowestContinue = cont.y() + body->pos().y();
		}

		if (!body->exit().isNull())
		{
			addConnector(bPos + body->exit(), QPoint(r, bPos.y() + body->exit().y()), false );
			addToLastConnector(r, updateConnect.y());
			addToLastConnector(updateConnect);

		} else if (lowestContinue > 0)
		{
			addConnector(r, lowestContinue, r, updateConnect.y(), false );
			addToLastConnector(updateConnect);
		}
	}

	addConnector(condFalseConnect, exit_, false);
}

}
