/***********************************************************************************************************************
 * MethodIconStyle.cpp
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/MethodIconStyle.h"

#include "VisualizationBase/headers/Styles.h"

#include <cmath>
const static double PI = 3.14159265358979323846;

namespace OOVisualization {

MethodIconStyle* MethodIconStyle::getDefault()
{
	static MethodIconStyle defaultStyle;
	return &defaultStyle;
}

void MethodIconStyle::computePath()
{
	path_ = QPainterPath();

	int step = 360 / (2 * numTeeth_);
	int i = 0;

	for (int degree = step / 2; degree < 360; degree += step)
	{
		int len;
		float x, y;
		len = (i % 2) ? middleRadius_ : outterRadius_;
		x = len * std::cos(degree * (PI / 180.0));
		y = len * std::sin(degree * (PI / 180.0));

		if ( degree == step / 2 ) path_.moveTo(x, y);
		else
			path_.lineTo(x, y);

		len = (i % 2) ? outterRadius_ : middleRadius_;
		x = len * std::cos(degree * (PI / 180.0));
		y = len * std::sin(degree * (PI / 180.0));

		path_.lineTo(x, y);
		i = (i + 1);
	}

	path_.closeSubpath();
	path_.addEllipse(QPointF(0,0), innerRadius_, innerRadius_);
}

void MethodIconStyle::load()
{
	IconStyle::load();
	Visualization::Styles::load("width", width_);
	Visualization::Styles::load("numTeeth", numTeeth_);
	Visualization::Styles::load("innerRadius", innerRadius_);
	Visualization::Styles::load("middleRadius", middleRadius_);
	Visualization::Styles::load("outterRadius", outterRadius_);
	Visualization::Styles::load("posFirst", posFirst_);
	Visualization::Styles::load("posSecond", posSecond_);
	Visualization::Styles::load("brush", brush_);
	computePath();
}

}
