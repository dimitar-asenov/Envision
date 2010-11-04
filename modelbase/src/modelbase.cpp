/***********************************************************************************************************************
 * modelbase.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "modelbase.h"

using namespace Logger;

Q_EXPORT_PLUGIN2( modelbase, Model::ModelBase )

namespace Model {

Log* ModelBase::logger = NULL;

bool ModelBase::initialize(Envision::EnvisionManager&)
{
	logger = Logger::Log::getLogger("modelbase");
	return true;
}

void ModelBase::selfTest(QString)
{
}

Log* ModelBase::log()
{
	return logger;
}

}
