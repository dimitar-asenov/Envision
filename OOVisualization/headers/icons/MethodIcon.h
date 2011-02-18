/***********************************************************************************************************************
 * MethodIcon.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef METHODICON_H_
#define METHODICON_H_

#include "../oovisualization_api.h"
#include "MethodIconStyle.h"

#include "VisualizationBase/headers/icons/Icon.h"
#include "VisualizationBase/headers/Styles.h"

namespace OOVisualization {

class OOVISUALIZATION_API MethodIcon : public Visualization::Icon
{
	ITEM_COMMON(MethodIcon)

	public:
		MethodIcon(Item* parent, const MethodIconStyle *style = Visualization::Styles::icon<MethodIcon>("default"));

		virtual void updateGeometry(int availableWidth, int availableHeight);
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

}

#endif /* METHODICON_H_ */
