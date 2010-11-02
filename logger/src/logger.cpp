/***********************************************************************************************************************
 * logger.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "logger.h"
#include <iostream>

Q_EXPORT_PLUGIN2( logger, Logger::LoggerPlugin )

namespace Logger {

bool LoggerPlugin::initialize(Envision::EnvisionManager&)
{
	std::cout << "here" << std::endl;
	return true;
}

void LoggerPlugin::selfTest(QString)
{
}

}
