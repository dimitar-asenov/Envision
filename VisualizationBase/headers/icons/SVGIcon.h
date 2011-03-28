/***********************************************************************************************************************
 * SVGIcon.h
 *
 *  Created on: Feb 9, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SVGICON_H_
#define SVGICON_H_

#include "Icon.h"
#include "SVGIconStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API SVGIcon : public Icon
{
	ITEM_COMMON(SVGIcon)

	public:
		SVGIcon(Item* parent, const SVGIconStyle *style = itemStyles().get());
		SVGIcon(Item* parent, const QString& iconStyleName);

		virtual void updateGeometry(int availableWidth, int availableHeight);
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

}

#endif /* SVGICON_H_ */
