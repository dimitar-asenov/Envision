/***********************************************************************************************************************
 * ModelException.h
 *
 *  Created on: Nov 11, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef MODELEXCEPTION_H_
#define MODELEXCEPTION_H_

#include "modelbase_api.h"
#include "EnvisionException.h"

#include <QtCore/QString>

namespace Model {

class MODELBASE_API ModelException : public Envision::EnvisionException
{
		public:
			ModelException(const QString& message);
			const QString& name() const;
};

}

#endif /* MODELEXCEPTION_H_ */
