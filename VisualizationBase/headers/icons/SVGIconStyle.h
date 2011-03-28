/***********************************************************************************************************************
 * SVGIconStyle.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SVGICONSTYLE_H_
#define SVGICONSTYLE_H_

#include "../visualizationbase_api.h"
#include "IconStyle.h"

#include <QtSvg/QSvgRenderer>
#include <QtCore/QString>

namespace Visualization {

class VISUALIZATIONBASE_API SVGIconStyle : public IconStyle
{
	private:
		QString filename_;
		QSvgRenderer renderer_;
		int width_;
		int height_;

	public:
		virtual void load(StyleLoader& sl);

		const QString& filename() const;
		int width() const;
		int height() const;

		void paint(QPainter* painter, int x, int y) const;
};

inline const QString& SVGIconStyle::filename() const { return filename_; }
inline int SVGIconStyle::width() const { return width_; }
inline int SVGIconStyle::height() const { return height_; }

}

#endif /* SVGICONSTYLE_H_ */
