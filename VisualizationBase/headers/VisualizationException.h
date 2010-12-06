/***********************************************************************************************************************
 * VisualizationException.h
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VISUALIZATIONEXCEPTION_H_
#define VISUALIZATIONEXCEPTION_H_

#include "visualizationbase_api.h"
#include "EnvisionException.h"

namespace Visualization {

class VISUALIZATIONBASE_API VisualizationException : public Envision::EnvisionException
{
	public:
		VisualizationException(const QString& message);
		const QString& name() const;
};

}

#endif /* VISUALIZATIONBASEEXCEPTION_H_ */
