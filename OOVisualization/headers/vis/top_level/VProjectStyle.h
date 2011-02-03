/***********************************************************************************************************************
 * VProjectStyle.h
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VPROJECTSTYLE_H_
#define VPROJECTSTYLE_H_

#include "../../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/headers/layouts/PositionLayoutStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VProjectStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle borderStyle_;
		Visualization::TextStyle headerStyle_;
		Visualization::PositionLayoutStyle contentStyle_;

	public:
		VProjectStyle();
		void load();

		const Visualization::PanelBorderLayoutStyle& borderStyle() const;
		const Visualization::TextStyle&  headerStyle() const;
		const Visualization::PositionLayoutStyle&  contentStyle() const;

		static VProjectStyle* getDefault();
};

inline const Visualization::PanelBorderLayoutStyle& VProjectStyle::borderStyle() const {return borderStyle_; }
inline const Visualization::TextStyle& VProjectStyle::headerStyle() const {return headerStyle_; }
inline const Visualization::PositionLayoutStyle& VProjectStyle::contentStyle() const {return contentStyle_; }

}

#endif /* VPROJECTSTYLE_H_ */
