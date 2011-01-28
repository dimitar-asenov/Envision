/***********************************************************************************************************************
 * OOModelException.cpp
 *
 *  Created on: Jan 28, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "OOModelException.h"

namespace OOModel {

OOModelException::OOModelException(const QString& message) :
	Envision::EnvisionException(message)
{
}

const QString& OOModelException::name() const
{
	static QString ename("OOModelException");
	return ename;
}

}
