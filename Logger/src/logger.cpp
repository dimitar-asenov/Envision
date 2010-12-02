/***********************************************************************************************************************
 * logger.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "logger.h"
#include "Log.h"
#include "LogTester.h"

#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>

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

	QTextStream out(stdout);

	if (lt.allTestsOK())
		out << "LOGGER: All tests passed" << endl;
	else
		out << "LOGGER: Tests failed!" << endl;
}

}
