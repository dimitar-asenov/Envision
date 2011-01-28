/***********************************************************************************************************************
 * PLUGINNAMEException.h
 *
 *  Created on: DATE
 *      Author: AUTHOR
 **********************************************************************************************************************/

#ifndef PLUGINNAME_UPPERCASEEXCEPTION_H_
#define PLUGINNAME_UPPERCASEEXCEPTION_H_

#include "PLUGINNAME_LOWERCASE_api.h"
#include "EnvisionException.h"

#include <QtCore/QString>

namespace NAMESPACE {

class PLUGINNAME_UPPERCASE_API PLUGINNAMEException : public Envision::EnvisionException
{
		public:
			PLUGINNAMEException(const QString& message);
			const QString& name() const;
};

}

#endif /* MODELEXCEPTION_H_ */
