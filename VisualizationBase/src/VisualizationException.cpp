/***********************************************************************************************************************
 * VisualizationException.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "VisualizationException.h"

namespace Visualization {

VisualizationException::VisualizationException(const QString& message) :
	Envision::EnvisionException(message)
{
}

const QString& VisualizationException::name() const
{
	static QString ename("ModelException");
	return ename;
}

}
