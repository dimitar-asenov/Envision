/***********************************************************************************************************************
 * VModuleStyle.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMODULESTYLE_H_
#define VMODULESTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/icons/SVGIconStyle.h"
#include "VisualizationBase/headers/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/headers/layouts/PositionLayoutStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VModuleStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle layout_;
		Visualization::SequentialLayoutStyle header_;
		Visualization::TextStyle name_;
		Visualization::SVGIconStyle icon_;
		Visualization::PositionLayoutStyle content_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::PanelBorderLayoutStyle& layout() const;
		const Visualization::SequentialLayoutStyle&  header() const;
		const Visualization::TextStyle&  name() const;
		const Visualization::SVGIconStyle&  icon() const;
		const Visualization::PositionLayoutStyle&  content() const;
};

inline const Visualization::PanelBorderLayoutStyle& VModuleStyle::layout() const {return layout_; }
inline const Visualization::SequentialLayoutStyle& VModuleStyle::header() const {return header_; }
inline const Visualization::TextStyle& VModuleStyle::name() const {return name_; }
inline const Visualization::SVGIconStyle& VModuleStyle::icon() const {return icon_; }
inline const Visualization::PositionLayoutStyle& VModuleStyle::content() const {return content_; }

}

#endif /* VMODULESTYLE_H_ */
