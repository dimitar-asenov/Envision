/***********************************************************************************************************************
 * VBlockCFStyle.h
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VBLOCKCFSTYLE_H_
#define VBLOCKCFSTYLE_H_

#include "../controlflowvisualization_api.h"
#include "ControlFlowItemStyle.h"
#include "VListCFStyle.h"

#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VBlockCFStyle : public ControlFlowItemStyle
{
	private:
		VListCFStyle statements_;

	public:
		void load();

		const VListCFStyle& statements() const;

		static VBlockCFStyle* getDefault();
};

inline const VListCFStyle& VBlockCFStyle::statements() const { return statements_; }

}

#endif /* VBLOCKCFSTYLE_H_ */
