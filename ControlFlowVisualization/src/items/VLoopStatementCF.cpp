/***********************************************************************************************************************
 * VLoopStatementCF.cpp
 *
 *  Created on: Feb 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VLoopStatementCF.h"

using namespace OOVisualization;
using namespace Visualization;
using namespace OOModel;
using namespace Model;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VLoopStatementCF)

VLoopStatementCF::VLoopStatementCF(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<ControlFlowItem, LoopStatement>(parent, node, style),
	conditionBackground(NULL),
	initStepBackground(NULL),
	updateStepBackground(NULL),
	condition(NULL),
	initStep(NULL),
	updateStep(NULL),
	body( NULL ),
	vis_(NULL)
{
}

VLoopStatementCF::~VLoopStatementCF()
{
	SAFE_DELETE_ITEM(conditionBackground);
	SAFE_DELETE_ITEM(initStepBackground);
	SAFE_DELETE_ITEM(updateStepBackground);
	SAFE_DELETE_ITEM(body);
	SAFE_DELETE_ITEM(vis_);

	// These were deleted by the destructors of their backgrounds above.
	condition = NULL;
	initStep = NULL;
	updateStep = NULL;
}

bool VLoopStatementCF::sizeDependsOnParent() const
{
	if (showAsControlFlow()) return false;
	else return vis_->sizeDependsOnParent();
}

bool VLoopStatementCF::isEmpty() const
{
	if (showAsControlFlow()) return false;
	else return vis_->isEmpty();
}

bool VLoopStatementCF::focusChild(FocusTarget location)
{
	if (showAsControlFlow())
	{
		//TODO implement this
		return false;
	}
	else return vis_->focusChild(location);
}

void VLoopStatementCF::determineChildren()
{
	if (showAsControlFlow())
	{
		SAFE_DELETE_ITEM(vis_);

		synchronizeItem(body, node()->body(), &style()->body());

		// Remove nodes which have changed
		if (initStep && initStep->node() != node()->initStep())
		{
			SAFE_DELETE_ITEM(initStepBackground);
			initStep = NULL;
			initStepBackground = NULL;
		}

		if (condition && condition->node() != node()->condition())
		{
			SAFE_DELETE_ITEM(conditionBackground);
			condition = NULL;
			conditionBackground = NULL;
		}

		if (updateStep && updateStep->node() != node()->updateStep())
		{
			SAFE_DELETE_ITEM(updateStepBackground);
			updateStep = NULL;
			updateStepBackground = NULL;
		}

		// Create nodes which are present in the model
		if (!initStep && node()->initStep())
		{
			initStepBackground = new SequentialLayout(NULL, &style()->initStep());
			initStep = renderer()->render(NULL, node()->initStep());
			initStepBackground->append(initStep);
			initStepBackground->setParentItem(this);
		}

		if (!updateStep && node()->updateStep())
		{
			updateStepBackground = new SequentialLayout(NULL, &style()->updateStep());
			updateStep = renderer()->render(NULL, node()->updateStep());
			updateStepBackground->append(updateStep);
			updateStepBackground->setParentItem(this);
		}

		if (!condition && node()->condition())
		{
			conditionBackground = new SequentialLayout(NULL, &style()->condition());
			condition = renderer()->render(NULL, node()->condition());
			conditionBackground->append(condition);
			conditionBackground->setParentItem(this);
		}

		if (conditionBackground) conditionBackground->setStyle( &style()->condition() );
		if (initStepBackground) initStepBackground->setStyle( &style()->initStep() );
		if (updateStepBackground) updateStepBackground->setStyle( &style()->updateStep() );
		body->setStyle(&style()->body());
		body->setPreferredBreakExit(ControlFlowItem::EXIT_LEFT);
		body->setPreferredContinueExit(ControlFlowItem::EXIT_RIGHT);
	}
	else
	{
		SAFE_DELETE_ITEM(conditionBackground);
		SAFE_DELETE_ITEM(initStepBackground);
		SAFE_DELETE_ITEM(updateStepBackground);
		SAFE_DELETE_ITEM(body);

		// These were deleted by the destructors of their backgrounds above.
		condition = NULL;
		initStep = NULL;
		updateStep = NULL;

		synchronizeItem<VLoopStatement>(vis_, node(), NULL);
	}
}

void VLoopStatementCF::updateGeometry(int availableWidth, int availableHeight)
{
	clearConnectors();

	if (!showAsControlFlow())
	{
		Item::updateGeometry(vis_, availableWidth, availableHeight);
		return;
	}

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
		left = - initStepBackground->width()/2;
		initPos = QPoint(left, y);
		right = initPos.x() + initStepBackground->width();

		y += initStepBackground->height();
	}

	y += style()->pinLength();

	if (updateStep)
	{
		updatePos = QPoint( 2*style()->pinLength(), y);

		if (right < updatePos.x() + updateStepBackground->width())
			right = updatePos.x() + updateStepBackground->width();

		y += updateStepBackground->height();
	}

	y += style()->pinLength();

	if (condition)
	{
		condPos = QPoint(- conditionBackground->width()/2, y);

		if (left > condPos.x()) left = condPos.x();
		if (right < condPos.x() + conditionBackground->width())
					right = condPos.x() + conditionBackground->width();

		y += conditionBackground->height();
	}

	if (body)
	{
		bodyPos = QPoint( 2*style()->pinLength(), y);
		if (right < bodyPos.x() + body->width() + 2*style()->pinLength() + style()->pin().width())
					right = bodyPos.x() + body->width() + 2*style()->pinLength() + style()->pin().width();

		y += body->height() + style()->pinLength() + style()->pin().width();
	}

	// Set the size
	if ( hasShape() ) getShape()->setInnerSize(right - left, y);
	else setSize(right - left, y);

	// Set the control flow elements
	entrance_ = QPoint(-left, 0);
	exit_ = QPoint( -left, y);

	// Set the positions
	if (initStep) initStepBackground->setPos(initPos - QPoint(left,0));
	if (updateStep) updateStepBackground->setPos(updatePos - QPoint(left,0));
	if (condition) conditionBackground->setPos(condPos - QPoint(left,0));
	if (body) body->setPos(bodyPos - QPoint(left,0));

	// Put connectors
	y = style()->pinLength();
	if (initStep)
	{
		addConnector(entrance_, entrance_ + QPoint(0, y), true);
		y += initStepBackground->height();
	}

	addConnector(entrance_ + QPoint(0,y), entrance_ + QPoint(0, y+style()->pinLength()), false);
	y += style()->pinLength();

	QPoint updateConnect(entrance_.x() + style()->pinLength(), y);

	if (updateStep)
	{
		updateConnect.ry() += updateStepBackground->height()/2;
		addConnector(updateConnect + QPoint(style()->pinLength(), 0), updateConnect - QPoint(style()->pinLength(), 0), true);

		updateConnect.rx() += updateStepBackground->width() + 2*style()->pinLength();

		addConnector(entrance_.x(), y, entrance_.x(), y + updateStepBackground->height(), false);

		y += updateStepBackground->height();
	}

	addConnector(updateConnect, updateConnect - QPoint(style()->pinLength(), 0), true);

	QPoint condTrueConnect;
	QPoint condFalseConnect;

	if (condition)
	{
		addConnector(entrance_.x(), y, entrance_.x(), y + style()->pinLength(), true);
		y += style()->pinLength();

		condTrueConnect.setY(y + conditionBackground->height()/2);
		condTrueConnect.setX(condPos.x() - left + conditionBackground->width() + style()->pinLength());

		addConnector(condTrueConnect, condTrueConnect - QPoint(style()->pinLength(),0), false);

		y += conditionBackground->height();

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
			if (cont.x() > 0) addConnector(bPos.x() + body->width(), bPos.y() + cont.y() , r, bPos.y() + cont.y(), true );
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
