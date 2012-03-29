/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * DefaultLog.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "DefaultLog.h"

namespace Logger {

QVector<Log::LogEntry> DefaultLog::loggedEvents;
QMutex DefaultLog::logAccess;

DefaultLog::DefaultLog(QString requestingPluginId) :
	pluginId(requestingPluginId), loggedEntriesSeen(0)
{
}

DefaultLog::~DefaultLog()
{}

void DefaultLog::add(QString message)
{
	add(LOGINFO, message);
}

void DefaultLog::add(Level l, QString message)
{
	QMutexLocker locker(&logAccess);

	QTextStream out(stdout);
	QTextStream err(stderr);

	if (l == LOGERROR) err<<	"ERROR   " << pluginId << ": " << message << endl;
	if (l == LOGDEBUG) out<<	"DEBUG   " << pluginId << ": " << message << endl;
	if (l == LOGINFO) out<<		"INFO    " << pluginId << ": " << message << endl;
	if (l == LOGWARNING) out<<	"WARNING " << pluginId << ": " << message << endl;

	loggedEvents.append(LogEntry());
	loggedEvents.last().level = l;
	loggedEvents.last().message = message;
	loggedEvents.last().pluginId = pluginId;

	emit newLogEntry();
}

bool DefaultLog::hasUnreadEntries()
{
	QMutexLocker locker(&logAccess);

	return loggedEvents.size() > loggedEntriesSeen;
}

Log::LogEntry DefaultLog::getNextEntry()
{
	QMutexLocker locker(&logAccess);

	if ( loggedEvents.size() == loggedEntriesSeen ) return LogEntry();

	loggedEntriesSeen++;
	return loggedEvents[loggedEntriesSeen - 1];
}

QList<Log::LogEntry> DefaultLog::getUnreadEntries()
{
	QMutexLocker locker(&logAccess);

	QList<Log::LogEntry> result;
	while ( loggedEntriesSeen < loggedEvents.size() )
	{
		result.append(loggedEvents[loggedEntriesSeen]);
		loggedEntriesSeen++;
	}
	return result;
}

}
