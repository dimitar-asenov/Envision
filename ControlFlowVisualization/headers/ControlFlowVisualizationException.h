/***********************************************************************************************************************
 * ControlFlowVisualizationException.h
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CONTROLFLOWVISUALIZATIONEXCEPTION_H_
#define CONTROLFLOWVISUALIZATIONEXCEPTION_H_

#include "controlflowvisualization_api.h"
#include "EnvisionException.h"

#include <QtCore/QString>

namespace ControlFlowVisualization {

class CONTROLFLOWVISUALIZATION_API ControlFlowVisualizationException : public Envision::EnvisionException
{
		public:
			ControlFlowVisualizationException(const QString& message);
			const QString& name() const;
};

}

#endif /* MODELEXCEPTION_H_ */
