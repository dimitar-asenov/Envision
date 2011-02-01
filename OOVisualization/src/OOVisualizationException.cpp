/***********************************************************************************************************************
 * OOVisualizationException.cpp
 *
 *  Created on: Feb 01, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "OOVisualizationException.h"

namespace OOVisualization {

OOVisualizationException::OOVisualizationException(const QString& message) :
	Envision::EnvisionException(message)
{
}

const QString& OOVisualizationException::name() const
{
	static QString ename("OOVisualizationException");
	return ename;
}

}
