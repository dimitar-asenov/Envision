/***********************************************************************************************************************
 * logger.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "logger.h"
#include <iostream>
#include "Log.h"
#include "LogTester.h"
#include <QCoreApplication>

Q_EXPORT_PLUGIN2( logger, Logger::LoggerPlugin )

namespace Logger {

bool LoggerPlugin::initialize(Envision::EnvisionManager&)
{
	return true;
}

void LoggerPlugin::selfTest(QString)
{
	LogTester lt;
	lt.runTests();
	qApp->processEvents();
	if (lt.allTestsOK())
		std::cout << "LOGGER: All tests passed" << std::endl;
	else
		std::cout << "LOGGER: Tests failed!" << std::endl;
}

}
