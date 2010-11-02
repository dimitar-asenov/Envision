/***********************************************************************************************************************
 * DefaultLog.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "DefaultLog.h"
#include <QMutexLocker>

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
	add(INFO, message);
}

void DefaultLog::add(Level l, QString message)
{
	QMutexLocker locker(&logAccess);

	loggedEvents.append(LogEntry());
	loggedEvents.last().level = l;
	loggedEvents.last().message = message;
	loggedEvents.last().pluginId = pluginId;
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
