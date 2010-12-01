/***********************************************************************************************************************
 * Log.h
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LOG_H_
#define LOG_H_

#include "logger_api.h"
#include <QObject>
#include <QString>
#include <QList>

namespace Logger {

class LOGGER_API Log : public QObject
{
	Q_OBJECT

	public:
		enum Level {LOGDEBUG, LOGERROR, LOGWARNING, LOGINFO};

		struct LogEntry
		{
			QString pluginId;
			Log::Level level;
			QString message;
		};

		virtual void add(QString message) = 0;
		virtual void add(Level l, QString message) = 0;

		virtual bool hasUnreadEntries() = 0;
		virtual LogEntry getNextEntry() = 0;
		virtual QList<LogEntry> getUnreadEntries() = 0;

		static Log* getLogger(QString requestingPluginId);

		virtual ~Log();

	signals:
		void newLogEntry();
};


}

#endif /* LOG_H_ */
