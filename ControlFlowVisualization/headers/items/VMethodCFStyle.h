/***********************************************************************************************************************
 * VMethodCFStyle.h
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMETHODCFSTYLE_H_
#define VMETHODCFSTYLE_H_

#include "../controlflowvisualization_api.h"
#include "VListCFStyle.h"

#include "OOVisualization/headers/top_level/VMethodStyle.h"

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API VMethodCFStyle : public OOVisualization::VMethodStyle
{
	private:
		bool showAsControlFlow_;
		VListCFStyle contentCF_;

	public:
		void load();

		bool showAsControlFlow() const;
		const VListCFStyle& contentCF() const;

		static VMethodCFStyle* getDefault();
};

inline bool VMethodCFStyle::showAsControlFlow() const { return showAsControlFlow_; }
inline const VListCFStyle& VMethodCFStyle::contentCF() const { return contentCF_; }

}

#endif /* VMETHODCFSTYLE_H_ */
