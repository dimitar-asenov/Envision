/***********************************************************************************************************************
 * LogTester.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "LogTester.h"

namespace Logger {

LogTester::LogTester() :
	correctLogEntries(0), log(Log::getLogger("logger"))
{
	QObject::connect(log, SIGNAL(newLogEntry()), this, SLOT(newLogEntryAdded()), Qt::QueuedConnection);
}

void LogTester::runTests()
{
	log->add("A default message");
	log->add(Log::LOGDEBUG, "A debug log");
	log->add(Log::LOGERROR, "An error log");
	log->add(Log::LOGINFO, "An info log");
	log->add(Log::LOGWARNING, "A warning log");
}

void LogTester::newLogEntryAdded()
{
	if ( log->hasUnreadEntries() == false ) return;

	Log::LogEntry le = log->getNextEntry();

	if ( le.pluginId != "logger" ) return;

	if ( correctLogEntries == 0 && le.level == Log::LOGINFO && le.message == "A default message" ) correctLogEntries++;
	if ( correctLogEntries == 1 && le.level == Log::LOGDEBUG && le.message == "A debug log" ) correctLogEntries++;
	if ( correctLogEntries == 2 && le.level == Log::LOGERROR && le.message == "An error log" ) correctLogEntries++;
	if ( correctLogEntries == 3 && le.level == Log::LOGINFO && le.message == "An info log" ) correctLogEntries++;
	if ( correctLogEntries == 4 && le.level == Log::LOGWARNING && le.message == "A warning log" ) correctLogEntries++;
}

bool LogTester::allTestsOK()
{
	return correctLogEntries == 5;
}

}
