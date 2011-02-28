/***********************************************************************************************************************
 * ControlFlowItem.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/ControlFlowItem.h"
#include "items/VMethodCF.h"

#include <QtGui/QPainter>

#include <cmath>

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(ControlFlowItem)

ControlFlowItem::ControlFlowItem(Item* parent,  const StyleType* style) :
	Item(parent, style),
	preferredContinueExit_(EXIT_RIGHT),
	preferredBreakExit_(EXIT_LEFT)
{
}

bool ControlFlowItem::showAsControlFlow() const
{
	QGraphicsItem* item = parentItem();
	while (item)
	{
		VMethodCF* met = dynamic_cast<VMethodCF*> (item);
		if (met) return met->style()->showAsControlFlow();

		item = item->parentItem();
	}

	return false;
}

QPainterPath ControlFlowItem::connector(const QList< QPoint >& points, bool arrowEnding)
{
	QPainterPath path;

	// Draw Linear path
	if (!points.isEmpty() > 0) path.moveTo(points.at(0));
	for(int i = 1; i<points.size(); ++i) path.lineTo(points.at(i));

	// Draw arrow if specified
	if (arrowEnding && points.size() >= 2)
	{
		QPoint last = points.last();
		QPoint prev = points.at( points.length()-2 );

		if (last.y() == prev.y())
		{
			// Horizontal arrow
			if (last.x() < prev.x())
			{
				// Left arrow
				path.lineTo(last.x() + style()->pinLength()/2, last.y() - style()->pinLength()/4);
				path.lineTo(last);
				path.lineTo(last.x() + style()->pinLength()/2, last.y() + style()->pinLength()/4);
				path.lineTo(last);
			}
			else
			{
				// Right arrow
				path.lineTo(last.x() - style()->pinLength()/2, last.y() - style()->pinLength()/4);
				path.lineTo(last);
				path.lineTo(last.x() - style()->pinLength()/2, last.y() + style()->pinLength()/4);
				path.lineTo(last);
			}
		}
		else
		{
			// Vertical arrow
			if (last.y() < prev.y())
			{
				// Up arrow
				path.lineTo(last.x() - style()->pinLength()/4, last.y() + style()->pinLength()/2);
				path.lineTo(last);
				path.lineTo(last.x() + style()->pinLength()/4, last.y() + style()->pinLength()/2);
				path.lineTo(last);
			}
			else
			{
				// Down arrow
				path.lineTo(last.x() - style()->pinLength()/4, last.y() - style()->pinLength()/2);
				path.lineTo(last);
				path.lineTo(last.x() + style()->pinLength()/4, last.y() - style()->pinLength()/2);
				path.lineTo(last);
			}

		}
	}

	return path;
}

void ControlFlowItem::addConnector(QList< QPoint >& points, bool arrowEnding)
{
	connectors_.append(points);
	arrowEndings_.append(arrowEnding);
	setUpdateNeeded();
}

void ControlFlowItem::clearConnectors()
{
	connectors_.clear();
	arrowEndings_.clear();
	setUpdateNeeded();
}

void ControlFlowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Item::paint(painter, option, widget);

	int xOffset = 0;
	int yOffset = 0;
	if (hasShape())
	{
		xOffset = getShape()->contentLeft();
		yOffset = getShape()->contentTop();
	}

	painter->setPen( style()->pin());

	for(int i = 0; i < connectors_.size(); ++i)
	{
		painter->drawPath( connector(connectors_.at(i), arrowEndings_.at(i)) );
	}
}

}
