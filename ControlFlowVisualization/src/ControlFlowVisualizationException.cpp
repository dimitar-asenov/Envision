/***********************************************************************************************************************
 * ControlFlowVisualizationException.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ControlFlowVisualizationException.h"

namespace ControlFlowVisualization {

ControlFlowVisualizationException::ControlFlowVisualizationException(const QString& message) :
	Envision::EnvisionException(message)
{
}

const QString& ControlFlowVisualizationException::name() const
{
	static QString ename("ControlFlowVisualizationException");
	return ename;
}

}
