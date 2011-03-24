/***********************************************************************************************************************
 * CustomMethodCallException.cpp
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "CustomMethodCallException.h"

namespace CustomMethodCall {

CustomMethodCallException::CustomMethodCallException(const QString& message) :
	Envision::EnvisionException(message)
{
}

const QString& CustomMethodCallException::name() const
{
	static QString ename("CustomMethodCallException");
	return ename;
}

}
