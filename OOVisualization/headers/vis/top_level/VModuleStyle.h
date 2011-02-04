/***********************************************************************************************************************
 * VModuleStyle.h
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMODULESTYLE_H_
#define VMODULESTYLE_H_

#include "../../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/PanelBorderLayoutStyle.h"
#include "VisualizationBase/headers/layouts/PositionLayoutStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VModuleStyle : public Visualization::ItemStyle
{
	private:
		Visualization::PanelBorderLayoutStyle borderStyle_;
		Visualization::TextStyle headerStyle_;
		Visualization::PositionLayoutStyle contentStyle_;

	public:
		VModuleStyle();
		void load();

		const Visualization::PanelBorderLayoutStyle& borderStyle() const;
		const Visualization::TextStyle&  headerStyle() const;
		const Visualization::PositionLayoutStyle&  contentStyle() const;

		static VModuleStyle* getDefault();
};

inline const Visualization::PanelBorderLayoutStyle& VModuleStyle::borderStyle() const {return borderStyle_; }
inline const Visualization::TextStyle& VModuleStyle::headerStyle() const {return headerStyle_; }
inline const Visualization::PositionLayoutStyle& VModuleStyle::contentStyle() const {return contentStyle_; }

}

#endif /* VMODULESTYLE_H_ */
