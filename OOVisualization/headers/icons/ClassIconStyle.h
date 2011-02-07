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

	public:
		virtual void load();

		int width() const;

		static ClassIconStyle* getDefault();
};

inline int ClassIconStyle::width() const { return width_; }

}

#endif /* CLASSICONSTYLE_H_ */
