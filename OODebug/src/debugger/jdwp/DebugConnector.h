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

#include "../../oodebug_api.h"

#include "Protocol.h"

namespace OODebug {

class Command;

class Event;

class VersionInfo;
class Location;
class LineTable;
class Frames;
class VariableTable;
class Values;
class StackVariable;

class OODEBUG_API DebugConnector : public QObject
{
	Q_OBJECT

	public:
		DebugConnector();
		~DebugConnector();

		using EventListener = std::function<void (Event)>;

		/**
		 * As certain info is only available when we load the class where the main method resides
		 * you need to pass the name of the class where the main method is in, in \a mainClassName.
		 */
		void connect(QString mainClassName, QString vmHostName = "localhost", int vmHostPort = 4000);

		void addEventListener(Protocol::EventKind kind, EventListener listener);

		bool resume();

		qint64 getClassId(const QString& signature);
		qint64 getMethodId(qint64 classId, const QString& signature);
		LineTable getLineTable(qint64 classId, qint64 methodId);

		Frames getFrames(qint64 threadId, qint32 numFrames, qint32 startFrame = 0);
		VariableTable getVariableTable(qint64 classId, qint64 methodId);
		Values getValues(qint64 threadId, qint64 frameId, QList<StackVariable> variables);


		int sendBreakpoint(Location breakLocation);
		bool clearBreakpoint(qint32 requestId);

		bool vmRunning();
	private:
		void handleSocketError(QAbstractSocket::SocketError socketError);
		void read();
		int read(qint32 requestId);

		QByteArray sendCommand(const Command& command);

		void readHandshake();
		void sendHandshake();

		void checkVersion();
		void checkIdSizes();

		void sendBreakAtStart();

		void handleComposite(QByteArray data);

		QTcpSocket tcpSocket_;

		QByteArray incompleteData_;

		QString mainClassName_;

		QHash<Protocol::EventKind, EventListener> eventListeners_;

		QHash<QString, qint64> classIdMap_;

		// Each entry is a full Message which is ready to be parsed & handled
		QList<QByteArray> readyData_;

		bool vmAlive_{};
};

/**
 * Adds a new event listener for the \a kind of event.
 * If there is already a listener for this kind the previous one is replaced!
 */
inline void DebugConnector::addEventListener(Protocol::EventKind kind, DebugConnector::EventListener listener)
{ eventListeners_[kind] = listener; }
inline bool DebugConnector::vmRunning() { return vmAlive_; }

} /* namespace OODebug */
