/***********************************************************************************************************************
 * filepersistence.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "filepersistence.h"

Q_EXPORT_PLUGIN2( filepersistence, FilePersistence::FilePersistence )

namespace FilePersistence {

bool FilePersistence::initialize(Envision::EnvisionManager&)
{
	return true;
}

void FilePersistence::selfTest(QString)
{

}

}
