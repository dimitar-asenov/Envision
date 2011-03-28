/***********************************************************************************************************************
 * ControlFlowItem.h
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CONTROLFLOWITEM_H_
#define CONTROLFLOWITEM_H_

#include "ControlFlowItemStyle.h"

#include "VisualizationBase/headers/items/Item.h"

namespace ControlFlowVisualization {

class ControlFlowItem : public Visualization::Item
{
	ITEM_COMMON(ControlFlowItem)

	public:
		enum PreferedExitDirection {EXIT_LEFT, EXIT_RIGHT};

		ControlFlowItem(Item* parent,  const StyleType* style);

		const QPoint& entrance();
		const QPoint& exit();
		const QList< QPoint >& breaks();
		const QList< QPoint >& continues();

		void setPreferredContinueExit(PreferedExitDirection preference);
		void setPreferredBreakExit(PreferedExitDirection preference);

		bool showAsControlFlow() const;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	protected:

		PreferedExitDirection preferredContinueExit_;
		PreferedExitDirection preferredBreakExit_;

		QPoint entrance_;
		QPoint exit_;
		QList< QPoint > breaks_;
		QList< QPoint > continues_;

		void addConnector(QList< QPoint >& points, bool arrowEnding);
		void addConnector(int xBegin, int yBegin, int xEnd, int yEnd, bool arrowEnding);
		void addConnector(const QPoint& begin, const QPoint& end, bool arrowEnding);
		void addToLastConnector(int x, int y);
		void addToLastConnector(const QPoint& point);
		void clearConnectors();

	private:
		QPainterPath connector(const QList< QPoint >& points, bool arrowEnding);

		QList< QList< QPoint > > connectors_;
		QList< bool > arrowEndings_;
};

inline const QPoint& ControlFlowItem::entrance() { return entrance_; }
inline const QPoint& ControlFlowItem::exit() { return exit_; }
inline const QList< QPoint >& ControlFlowItem::breaks() { return breaks_; }
inline const QList< QPoint >& ControlFlowItem::continues() { return continues_; }

inline void ControlFlowItem::setPreferredContinueExit(PreferedExitDirection preference) { preferredContinueExit_ = preference; }
inline void ControlFlowItem::setPreferredBreakExit(PreferedExitDirection preference) { preferredBreakExit_ = preference; }

}

#endif /* CONTROLFLOWITEM_H_ */
