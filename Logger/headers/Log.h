/***********************************************************************************************************************
 * Log.h
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LOG_H_
#define LOG_H_

#include "logger_api.h"
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QObject>

namespace Logger {

/*
 * The Log interface includes functionality for recording system events.
 */
class LOGGER_API Log : public QObject
{
	Q_OBJECT

	public:

		/**
		 * The log levels to which each message can belong.
		 */
		enum Level {LOGDEBUG, LOGERROR, LOGWARNING, LOGINFO};

		/**
		 * The information of each entry in the log.
		 */
		struct LogEntry
		{
			QString pluginId;
			Log::Level level;
			QString message;
		};

		/**
		 * Adds a new message with LOGINFO level to the log.
		 *
		 * The plug-in id that will be associated with the message is determined by the Log object instance.
		 *
		 * @param message
		 * 				The message to add
		 */
		virtual void add(QString message) = 0;

		/**
		 * Adds a new message with the specified level to the log.
		 *
		 * The plug-in id that will be associated with the message is determined by the Log object instance.
		 *
		 * @param l
		 * 				The log level of the message
		 * @oaram message
		 * 				The message to add
		 */
		virtual void add(Level l, QString message) = 0;

		/**
		 * Returns true if there are unread messages in the log.
		 */
		virtual bool hasUnreadEntries() = 0;

		/**
		 * Returns the next unread message in the log queue.
		 *
		 * This also advances the pointer of the last read message by one.
		 *
		 * If there are no unread entries, returns an empty (default) LogEntry.
		 */
		virtual LogEntry getNextEntry() = 0;

		/**
		 * Returns a list of all unread messages in the log.
		 *
		 * If there are no unread messages the list is empty.
		 */
		virtual QList<LogEntry> getUnreadEntries() = 0;


		/**
		 * Returns an instance of the default logger that will be used to log events for the specified plugin.
		 *
		 * @param requestingPluginId
		 * 				The plug-in id that will be used when adding new messages to the log.
		 */
		static Log* getLogger(QString requestingPluginId);

		virtual ~Log();

	signals:
		/**
		 * Emitted when a new log entry (or several) have been added to the log.
		 */
		void newLogEntry();
};


}

#endif /* LOG_H_ */
