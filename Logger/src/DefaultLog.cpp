/***********************************************************************************************************************
 * DefaultLog.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "DefaultLog.h"
#include <QtCore/QMutexLocker>
#include <QtCore/QTextStream>

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
