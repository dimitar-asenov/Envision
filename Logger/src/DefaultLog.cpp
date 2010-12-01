/***********************************************************************************************************************
 * DefaultLog.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "DefaultLog.h"
#include <QMutexLocker>
#include <iostream>

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

	if (l == LOGERROR) std::cerr<<	"ERROR   " << qPrintable(pluginId) << ": " << qPrintable(message) << std::endl;
	if (l == LOGDEBUG) std::cout<<	"DEBUG   " << qPrintable(pluginId) << ": " << qPrintable(message) << std::endl;
	if (l == LOGINFO) std::cout<<		"INFO    " << qPrintable(pluginId) << ": " << qPrintable(message) << std::endl;
	if (l == LOGWARNING) std::cout<<	"WARNING " << qPrintable(pluginId) << ": " << qPrintable(message) << std::endl;

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
