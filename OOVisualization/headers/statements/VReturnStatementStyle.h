/***********************************************************************************************************************
 * VReturnStatementStyle.h
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VRETURNSTATEMENTSTYLE_H_
#define VRETURNSTATEMENTSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/StaticStyle.h"
#include "VisualizationBase/headers/items/VListStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VReturnStatementStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::StaticStyle symbol_;
		Visualization::VListStyle values_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::StaticStyle& symbol() const;
		const Visualization::VListStyle& values() const;
};

inline const Visualization::SequentialLayoutStyle& VReturnStatementStyle::layout() const { return layout_; }
inline const Visualization::StaticStyle& VReturnStatementStyle::symbol() const { return symbol_; }
inline const Visualization::VListStyle& VReturnStatementStyle::values() const { return values_; }
}

#endif /* VRETURNSTATEMENTSTYLE_H_ */
