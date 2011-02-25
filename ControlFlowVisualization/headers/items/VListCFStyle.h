/***********************************************************************************************************************
 * VListCFStyle.h
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLISTCFSTYLE_H_
#define VLISTCFSTYLE_H_

#include "../controlflowvisualization_api.h"
#include "ControlFlowItemStyle.h"

#include "VisualizationBase/headers/items/ItemStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VListCFStyle : public ControlFlowItemStyle
{
	private:
		Visualization::SequentialLayoutStyle sequence_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& sequence() const;

		static VListCFStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VListCFStyle::sequence() const { return sequence_; }

}

#endif /* VLISTCFSTYLE_H_ */
