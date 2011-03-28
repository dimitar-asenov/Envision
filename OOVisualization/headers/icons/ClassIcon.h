/***********************************************************************************************************************
 * ClassIcon.h
 *
 *  Created on: Feb 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CLASSICON_H_
#define CLASSICON_H_

#include "../oovisualization_api.h"
#include "ClassIconStyle.h"

#include "VisualizationBase/headers/icons/Icon.h"

namespace OOVisualization {

class OOVISUALIZATION_API ClassIcon : public Visualization::Icon
{
	ITEM_COMMON(ClassIcon)

	public:
		ClassIcon(Item* parent, const ClassIconStyle *style = itemStyles().get());

		virtual void updateGeometry(int availableWidth, int availableHeight);
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

}

#endif /* CLASSICON_H_ */
