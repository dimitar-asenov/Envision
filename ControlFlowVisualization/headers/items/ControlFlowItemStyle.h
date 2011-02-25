/***********************************************************************************************************************
 * ControlFlowItemStyle.h
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CONTROLFLOWITEMSTYLE_H_
#define CONTROLFLOWITEMSTYLE_H_

#include "../controlflowvisualization_api.h"

#include "VisualizationBase/headers/items/ItemStyle.h"

#include <QtGui/QPen>

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API ControlFlowItemStyle : public Visualization::ItemStyle
{
	private:
		int pinLength_;
		QPen pin_;

	public:
		void load();

		int pinLength() const;
		const QPen& pin() const;

		static ControlFlowItemStyle* getDefault();
};

inline int ControlFlowItemStyle::pinLength() const {	return pinLength_; }
inline const QPen& ControlFlowItemStyle::pin() const {	return pin_; }

}

#endif /* CONTROLFLOWITEMSTYLE_H_ */
