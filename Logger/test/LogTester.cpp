/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "LogTester.h"

#include "../src/Log.h"

namespace Logger {

LogTester::LogTester() :
	correctLogEntries{0}, log{Log::getLogger("logger")}
{
	QObject::connect(log, &Log::newLogEntry, this, &LogTester::newLogEntryAdded, Qt::QueuedConnection);
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
	// Search the log for an unread entry from the logger plug-in.
	Log::LogEntry le;
	bool loggerEntry = false;
	while ( log->hasUnreadEntries() && !loggerEntry)
	{
		le = log->getNextEntry();
		loggerEntry = le.pluginId == "logger";
	}

	if ( !loggerEntry ) return;

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
