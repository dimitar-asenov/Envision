/***********************************************************************************************************************
 * OOVisualizationException.h
 *
 *  Created on: Feb 01, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef OOVISUALIZATIONEXCEPTION_H_
#define OOVISUALIZATIONEXCEPTION_H_

#include "oovisualization_api.h"
#include "EnvisionException.h"

#include <QtCore/QString>

namespace OOVisualization {

class OOVISUALIZATION_API OOVisualizationException : public Envision::EnvisionException
{
		public:
			OOVisualizationException(const QString& message);
			const QString& name() const;
};

}

#endif /* MODELEXCEPTION_H_ */
