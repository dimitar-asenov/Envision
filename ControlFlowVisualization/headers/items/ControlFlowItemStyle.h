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

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API ControlFlowItemStyle : public Visualization::ItemStyle
{
	private:
		bool showAsControlFlow_;

	public:
		void load();

		bool showAsControlFlow() const;

		static ControlFlowItemStyle* getDefault();
};

inline bool ControlFlowItemStyle::showAsControlFlow() const { return showAsControlFlow_; }

}

#endif /* CONTROLFLOWITEMSTYLE_H_ */
