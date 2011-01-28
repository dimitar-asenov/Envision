/***********************************************************************************************************************
 * OOModelException.h
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef OOMODELEXCEPTION_H_
#define OOMODELEXCEPTION_H_

#include "oomodel_api.h"
#include "EnvisionException.h"

#include <QtCore/QString>

namespace OOModel {

class OOMODEL_API OOModelException : public Envision::EnvisionException
{
		public:
			OOModelException(const QString& message);
			const QString& name() const;
};

}

#endif /* MODELEXCEPTION_H_ */
