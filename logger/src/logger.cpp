/***********************************************************************************************************************
 * logger.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "logger.h"
#include <iostream>
#include "Log.h"

Q_EXPORT_PLUGIN2( logger, Logger::LoggerPlugin )

namespace Logger {

bool LoggerPlugin::initialize(Envision::EnvisionManager&)
{
	std::cout << "here" << std::endl;
	return true;
}

void LoggerPlugin::selfTest(QString)
{
	Log* l = Log::getLogger("logger");

	l->add("A default message");
	l->add(Log::LOGDEBUG, "A debug log");
	l->add(Log::LOGERROR, "An error log");
	l->add(Log::LOGINFO, "An info log");
	l->add(Log::LOGWARNING, "A warning log");
}

}
