/***********************************************************************************************************************
 * VIfStatementStyle.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VIFSTATEMENTSTYLE_H_
#define VIFSTATEMENTSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"
#include "VisualizationBase/headers/items/VListStyle.h"
#include "VisualizationBase/headers/icons/SVGIconStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VIfStatementStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle layout_;
		Visualization::SequentialLayoutStyle header_;
		Visualization::SVGIconStyle icon_;
		Visualization::SequentialLayoutStyle condition_;

		Visualization::SequentialLayoutStyle content_;
		Visualization::VListStyle thenBranch_;
		Visualization::VListStyle elseBranch_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::PanelBorderLayoutStyle& layout() const;
		const Visualization::SequentialLayoutStyle& header() const;
		const Visualization::SVGIconStyle& icon() const;
		const Visualization::SequentialLayoutStyle& condition() const;

		const Visualization::SequentialLayoutStyle& content() const;
		const Visualization::VListStyle& thenBranch() const;
		const Visualization::VListStyle& elseBranch() const;
};

inline const Visualization::PanelBorderLayoutStyle& VIfStatementStyle::layout() const { return layout_; }
inline const Visualization::SequentialLayoutStyle& VIfStatementStyle::header() const { return header_; }
inline const Visualization::SVGIconStyle& VIfStatementStyle::icon() const { return icon_; }
inline const Visualization::SequentialLayoutStyle& VIfStatementStyle::condition() const { return condition_; }

inline const Visualization::SequentialLayoutStyle& VIfStatementStyle::content() const { return content_; }
inline const Visualization::VListStyle& VIfStatementStyle::thenBranch() const { return thenBranch_; }
inline const Visualization::VListStyle& VIfStatementStyle::elseBranch() const { return elseBranch_; }

}

#endif /* VIFSTATEMENTSTYLE_H_ */
