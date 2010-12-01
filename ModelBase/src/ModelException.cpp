/***********************************************************************************************************************
 * ModelException.cpp
 *
 *  Created on: Nov 11, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "ModelException.h"
#include <QTextStream>

namespace Model {

ModelException::ModelException(const QString& message) :
	Envision::EnvisionException(message)
{
}

const QString& ModelException::name() const
{
	static QString ename("ModelException");
	return ename;
}

}
