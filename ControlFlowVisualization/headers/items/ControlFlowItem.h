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
#include "VisualizationBase/headers/Styles.h"

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

		void setPreferredExit(PreferedExitDirection preference);

		bool showAsControlFlow() const;

	protected:

		PreferedExitDirection preferredExit_;

		QPoint entrance_;
		QPoint exit_;
		QList< QPoint > breaks_;
		QList< QPoint > continues_;
};

inline const QPoint& ControlFlowItem::entrance() { return entrance_; }
inline const QPoint& ControlFlowItem::exit() { return exit_; }
inline const QList< QPoint >& ControlFlowItem::breaks() { return breaks_; }
inline const QList< QPoint >& ControlFlowItem::continues() { return continues_; }

inline void ControlFlowItem::setPreferredExit(PreferedExitDirection preference) { preferredExit_ = preference; }

}

#endif /* CONTROLFLOWITEM_H_ */
