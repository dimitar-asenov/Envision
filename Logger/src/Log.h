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

#pragma once

#include "logger_api.h"

namespace Logger {

/**
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
		 * @param level
		 * 				The log level of the message
		 * @param message
		 * 				The message to add
		 */
		virtual void add(Level level, QString message) = 0;

		/**
		 * A convenience function that adds a new info message to the log.
		 */
		void info(const QString& message);

		/**
		* A convenience function that adds a new warning message to the log.
		*/
		void warning(const QString& message);

		/**
		* A convenience function that adds a new error message to the log.
		*/
		void error(const QString& message);

		/**
		 * A convenience function that logs a new debug message to the log.
		 */
		void debug(const QString& message);

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

	Q_SIGNALS:
		/**
		 * Emitted when a new log entry (or several) have been added to the log.
		 */
		void newLogEntry();
};

inline void Log::info(const QString& message) { add(LOGINFO, message); }
inline void Log::warning(const QString& message) { add(LOGWARNING, message); }
inline void Log::error(const QString& message) { add(LOGERROR, message); }
inline void Log::debug(const QString& message) { add(LOGDEBUG, message); }

}
