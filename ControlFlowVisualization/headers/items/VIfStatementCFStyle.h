/***********************************************************************************************************************
 * VIfStatementCFStyle.h
 *
 *  Created on: Mar 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VIFSTATEMENTCFSTYLE_H_
#define VIFSTATEMENTCFSTYLE_H_

#include "../controlflowvisualization_api.h"
#include "ControlFlowItemStyle.h"
#include "VListCFStyle.h"

#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VIfStatementCFStyle : public ControlFlowItemStyle
{
	private:
		Visualization::SequentialLayoutStyle condition_;
		VListCFStyle thenBranch_;
		VListCFStyle elseBranch_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& condition() const;
		const VListCFStyle& thenBranch() const;
		const VListCFStyle& elseBranch() const;
};

inline const Visualization::SequentialLayoutStyle& VIfStatementCFStyle::condition() const { return condition_; }
inline const VListCFStyle& VIfStatementCFStyle::thenBranch() const { return thenBranch_; }
inline const VListCFStyle& VIfStatementCFStyle::elseBranch() const { return elseBranch_; }

}

#endif /* VIFSTATEMENTCFSTYLE_H_ */
