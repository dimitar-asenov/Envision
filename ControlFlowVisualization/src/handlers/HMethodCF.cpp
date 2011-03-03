/***********************************************************************************************************************
 * HMethodCF.cpp
 *
 *  Created on: Mar 3, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "handlers/HMethodCF.h"

#include "items/VMethodCF.h"
#include "VisualizationBase/headers/Scene.h"

namespace ControlFlowVisualization {

HMethodCF::HMethodCF()
{
}

HMethodCF* HMethodCF::instance()
{
	static HMethodCF h;
	return &h;
}

void HMethodCF::mouseDoubleClickEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->modifiers() == 0 && event->button() == Qt::LeftButton)
	{
		VMethodCF *met = dynamic_cast<VMethodCF*> (target);
		if (met)
		{
			if (met->style()->showAsControlFlow()) met->setStyle(Visualization::Styles::item<VMethodCF>("default"));
			else met->setStyle(Visualization::Styles::item<VMethodCF>("showcontrolflow"));
		}
	}
	else GenericHandler::mouseDoubleClickEvent(target, event);
}

}
