/***********************************************************************************************************************
 * SVGIconStyle.cpp
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "icons/SVGIconStyle.h"
#include "Styles.h"
#include "VisualizationException.h"

namespace Visualization {

SVGIconStyle* SVGIconStyle::getDefault()
{
	static SVGIconStyle defaultStyle;
	return &defaultStyle;
}

void SVGIconStyle::load()
{
	IconStyle::load();
	Styles::load("filename", filename_);
	Styles::load("width", width_);
	Styles::load("height", height_);
	if (!renderer_.load(filename_)) throw VisualizationException("Could not read SVG icon: " + filename_);
}

void SVGIconStyle::paint(QPainter* painter, int x, int y) const
{
	QSvgRenderer& r = const_cast<QSvgRenderer&> (renderer_);
	r.render(painter, QRectF(x, y, width_, height_));
}

}
