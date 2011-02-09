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

	public:
		virtual void load();

		const QString& filename() const;
		const QSvgRenderer& renderer() const;

		void paint(QPainter* painter) const;

		static SVGIconStyle* getDefault();
};

inline const QString& SVGIconStyle::filename() const { return filename_; }
inline const QSvgRenderer& SVGIconStyle::renderer() const { return renderer_; }

}

#endif /* SVGICONSTYLE_H_ */
