/***********************************************************************************************************************
 * DefaultLog.h
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef DEFAULTLOG_H_
#define DEFAULTLOG_H_

#include "Log.h"
#include <QtCore/QMutex>
#include <QtCore/QVector>

namespace Logger {

class DefaultLog: public Log
{
	private:
		QString pluginId;
		int loggedEntriesSeen;

		static QVector<Log::LogEntry> loggedEvents;
		static QMutex logAccess;

	public:
		DefaultLog(QString requestingPluginId);
		~DefaultLog();

		void add(QString message);
		void add(Level l, QString message);

		bool hasUnreadEntries();
		Log::LogEntry getNextEntry();
		QList<Log::LogEntry> getUnreadEntries();
};

}

#endif /* DEFAULTLOG_H_ */
