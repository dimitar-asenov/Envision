/***********************************************************************************************************************
 * VLoopStatementStyle.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLOOPSTATEMENTSTYLE_H_
#define VLOOPSTATEMENTSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"
#include "VisualizationBase/headers/icons/SVGIconStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VLoopStatementStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle layout_;
		Visualization::SequentialLayoutStyle header_;
		Visualization::SVGIconStyle icon_;
		Visualization::SequentialLayoutStyle initStep_;
		Visualization::SequentialLayoutStyle condition_;
		Visualization::SequentialLayoutStyle updateStep_;

	public:
		void load();

		const Visualization::PanelBorderLayoutStyle& layout() const;
		const Visualization::SequentialLayoutStyle& header() const;
		const Visualization::SVGIconStyle& icon() const;

		const Visualization::SequentialLayoutStyle& initStep() const;
		const Visualization::SequentialLayoutStyle& condition() const;
		const Visualization::SequentialLayoutStyle& updateStep() const;

		static VLoopStatementStyle* getDefault();
};

inline const Visualization::PanelBorderLayoutStyle& VLoopStatementStyle::layout() const { return layout_; }
inline const Visualization::SequentialLayoutStyle& VLoopStatementStyle::header() const { return header_; }
inline const Visualization::SVGIconStyle& VLoopStatementStyle::icon() const { return icon_; }

inline const Visualization::SequentialLayoutStyle& VLoopStatementStyle::initStep() const { return initStep_; }
inline const Visualization::SequentialLayoutStyle& VLoopStatementStyle::condition() const { return condition_; }
inline const Visualization::SequentialLayoutStyle& VLoopStatementStyle::updateStep() const { return updateStep_; }

}

#endif /* VLOOPSTATEMENTSTYLE_H_ */
