/***********************************************************************************************************************
 * ClassIconStyle.h
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CLASSICONSTYLE_H_
#define CLASSICONSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/icons/IconStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API ClassIconStyle :public Visualization::IconStyle
{
	private:
		int width_;
		int rectWidth_;
		int rectHeight_;

	public:
		virtual void load(Visualization::StyleLoader& sl);

		int width() const;
		int rectWidth() const;
		int rectHeight() const;
};

inline int ClassIconStyle::width() const { return width_; }
inline int ClassIconStyle::rectWidth() const { return rectWidth_; }
inline int ClassIconStyle::rectHeight() const { return rectHeight_; }

}

#endif /* CLASSICONSTYLE_H_ */
