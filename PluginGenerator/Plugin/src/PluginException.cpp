/***********************************************************************************************************************
 * PLUGINNAMEException.cpp
 *
 *  Created on: DATE
 *      Author: AUTHOR
 **********************************************************************************************************************/

#include "PLUGINNAMEException.h"

namespace NAMESPACE {

PLUGINNAMEException::PLUGINNAMEException(const QString& message) :
	Envision::EnvisionException(message)
{
}

const QString& PLUGINNAMEException::name() const
{
	static QString ename("PLUGINNAMEException");
	return ename;
}

}
