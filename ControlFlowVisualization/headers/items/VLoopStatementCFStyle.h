/***********************************************************************************************************************
 * VLoopStatementCFStyle.h
 *
 *  Created on: Feb 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLOOPSTATEMENTCFSTYLE_H_
#define VLOOPSTATEMENTCFSTYLE_H_

#include "../controlflowvisualization_api.h"
#include "ControlFlowItemStyle.h"
#include "VListCFStyle.h"

#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"
#include "VisualizationBase/headers/items/ItemStyle.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VLoopStatementCFStyle : public ControlFlowItemStyle
{
	private:
		Visualization::SequentialLayoutStyle initStep_;
		Visualization::SequentialLayoutStyle condition_;
		Visualization::SequentialLayoutStyle updateStep_;
		VListCFStyle body_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& initStep() const;
		const Visualization::SequentialLayoutStyle& condition() const;
		const Visualization::SequentialLayoutStyle& updateStep() const;
		const VListCFStyle& body() const;
};

inline const Visualization::SequentialLayoutStyle& VLoopStatementCFStyle::initStep() const { return initStep_; }
inline const Visualization::SequentialLayoutStyle& VLoopStatementCFStyle::condition() const { return condition_; }
inline const Visualization::SequentialLayoutStyle& VLoopStatementCFStyle::updateStep() const { return updateStep_; }
inline const VListCFStyle& VLoopStatementCFStyle::body() const { return body_; }

}

#endif /* VLOOPSTATEMENTCFSTYLE_H_ */
