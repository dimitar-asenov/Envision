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
