/***********************************************************************************************************************
 * VForEachStatementStyle.h
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFOREACHSTATEMENTSTYLE_H_
#define VFOREACHSTATEMENTSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/icons/SVGIconStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VForEachStatementStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle layout_;
		Visualization::SequentialLayoutStyle header_;
		Visualization::SVGIconStyle icon_;
		Visualization::SequentialLayoutStyle varContainer_;
		Visualization::TextStyle varName_;
		Visualization::SequentialLayoutStyle collection_;

	public:
		void load();

		const Visualization::PanelBorderLayoutStyle& layout() const;
		const Visualization::SequentialLayoutStyle& header() const;
		const Visualization::SVGIconStyle& icon() const;

		const Visualization::SequentialLayoutStyle& varContainer() const;
		const Visualization::TextStyle& varName() const;
		const Visualization::SequentialLayoutStyle& collection() const;

		static VForEachStatementStyle* getDefault();
};

inline const Visualization::PanelBorderLayoutStyle& VForEachStatementStyle::layout() const { return layout_; }
inline const Visualization::SequentialLayoutStyle& VForEachStatementStyle::header() const { return header_; }
inline const Visualization::SVGIconStyle& VForEachStatementStyle::icon() const { return icon_; }

inline const Visualization::SequentialLayoutStyle& VForEachStatementStyle::varContainer() const { return varContainer_; }
inline const Visualization::TextStyle& VForEachStatementStyle::varName() const { return varName_; }
inline const Visualization::SequentialLayoutStyle& VForEachStatementStyle::collection() const { return collection_; }

}

#endif /* VFOREACHSTATEMENTSTYLE_H_ */
