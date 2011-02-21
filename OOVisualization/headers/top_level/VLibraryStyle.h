/***********************************************************************************************************************
 * VLibraryStyle.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLIBRARYSTYLE_H_
#define VLIBRARYSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/icons/SVGIconStyle.h"
#include "VisualizationBase/headers/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/headers/layouts/PositionLayoutStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VLibraryStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle layout_;
		Visualization::SequentialLayoutStyle header_;
		Visualization::TextStyle name_;
		Visualization::SVGIconStyle icon_;
		Visualization::PositionLayoutStyle content_;

	public:
		void load();

		const Visualization::PanelBorderLayoutStyle& layout() const;
		const Visualization::SequentialLayoutStyle&  header() const;
		const Visualization::TextStyle&  name() const;
		const Visualization::SVGIconStyle&  icon() const;
		const Visualization::PositionLayoutStyle&  content() const;

		static VLibraryStyle* getDefault();
};

inline const Visualization::PanelBorderLayoutStyle& VLibraryStyle::layout() const {return layout_; }
inline const Visualization::SequentialLayoutStyle& VLibraryStyle::header() const {return header_; }
inline const Visualization::TextStyle& VLibraryStyle::name() const {return name_; }
inline const Visualization::SVGIconStyle& VLibraryStyle::icon() const {return icon_; }
inline const Visualization::PositionLayoutStyle& VLibraryStyle::content() const {return content_; }
}

#endif /* VLIBRARYSTYLE_H_ */
