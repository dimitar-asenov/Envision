/***********************************************************************************************************************
 * FilePersistenceException.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "FilePersistenceException.h"

namespace FilePersistence {

FilePersistenceException::FilePersistenceException(const QString& message) :
	Envision::EnvisionException(message)
{
}

const QString& FilePersistenceException::name() const
{
	static QString ename("ModelException");
	return ename;
}

}
