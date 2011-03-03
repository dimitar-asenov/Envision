/***********************************************************************************************************************
 * HMethodCF.h
 *
 *  Created on: Mar 3, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef HMETHODCF_H_
#define HMETHODCF_H_

#include "../controlflowvisualization_api.h"

#include "InteractionBase/headers/handlers/GenericHandler.h"

namespace ControlFlowVisualization {

class INTERACTIONBASE_API HMethodCF : public Interaction::GenericHandler
{
	protected:
		HMethodCF();

	public:
		static HMethodCF* instance();

		virtual void mouseDoubleClickEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event);
};

}

#endif /* HMETHODCF_H_ */
