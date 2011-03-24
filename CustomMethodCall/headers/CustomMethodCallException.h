/***********************************************************************************************************************
 * CustomMethodCallException.h
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef CUSTOMMETHODCALLEXCEPTION_H_
#define CUSTOMMETHODCALLEXCEPTION_H_

#include "custommethodcall_api.h"
#include "EnvisionException.h"

#include <QtCore/QString>

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API CustomMethodCallException : public Envision::EnvisionException
{
		public:
			CustomMethodCallException(const QString& message);
			const QString& name() const;
};

}

#endif /* MODELEXCEPTION_H_ */
