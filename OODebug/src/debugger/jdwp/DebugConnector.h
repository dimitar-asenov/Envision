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

struct Event;

struct VersionInfo;
struct Location;
struct LineTable;
struct Frames;
struct VariableTable;
struct Values;
struct StackVariable;
struct ArrayValues;

/**
 * A Connector to a Java VM via the JDWP protocol.
 */
class OODEBUG_API DebugConnector : public QObject
{
	Q_OBJECT

	public:
		DebugConnector();
		~DebugConnector();

		using EventListener = std::function<void (Event)>;

		/**
		 * Connects to the JVM at \a vmHostName on port \a vmHostPort.
		 */
		void connect(QString vmHostName = "localhost", int vmHostPort = 4000);

		/**
		 * Add the \a listener callback to the connector. The \a listener is called whenever and Event of type \a kind
		 * is received.
		 *
		 * If there is already a \a listener for this kind the previous one is replaced!
		 */
		void addEventListener(Protocol::EventKind kind, EventListener listener);

		/**
		 * Suspends the execution on the VM and returns true on succes.
		 */
		bool suspend();
		/**
		 * Resumes the VM, do not use this function inside event handlers, use \a wantResume instead.
		 */
		bool resume();
		/**
		 * Specify that you want to resume, if there are multiple events all should agree otherwise we don't resume.
		 *
		 * Note that this function should only be used in event handlers.
		 */
		void wantResume(bool resume);

		QString fileNameForReference(qint64 referenceId);
		QString signatureOf(qint64 referenceId);
		qint64 classIdOf(const QString& signature);
		qint64 methodIdOf(qint64 classId, const QString& signature);
		LineTable lineTable(qint64 classId, qint64 methodId);

		QList<qint64> allThreadIds();
		QString threadName(qint64 threadId);

		Frames frames(qint64 threadId, qint32 numFrames, qint32 startFrame = 0);
		VariableTable variableTableForMethod(qint64 classId, qint64 methodId);
		Values values(qint64 threadId, qint64 frameId, QList<StackVariable> variables);
		QString stringFromId(qint64 stringId);
		int arrayLength(qint64 arrayId);
		ArrayValues arrayValues(qint64 arrayId, qint32 firstIndex, qint32 length);

		bool breakAtClassLoad(QString className);

		int setBreakpoint(Location breakLocation);
		bool clearBreakpoint(qint32 requestId);

		int singleStep(qint64 threadId, Protocol::StepSize stepSize = Protocol::StepSize::LINE,
							Protocol::StepDepth stepDepth = Protocol::StepDepth::OVER);

		/**
		 * Returns if the program on the target VM is running.
		 */
		bool vmAlive();

		static constexpr int NO_RESULT{-1};
	private:
		void handleSocketError(QAbstractSocket::SocketError socketError);
		void dispatchEvents();
		void readFromSocket();

		/**
		 * Sends the \a command to the VM and returns the corresponding reply as a QByteArray.
		 */
		QByteArray sendCommand(const Command& command);
		/**
		 * Waits until the Reply for the request with id \a requestId is received.
		 */
		QByteArray waitForReply(qint32 requestId);

		void readHandshake();
		void sendHandshake();

		void checkVersion();
		void checkIdSizes();

		void handleComposite(QByteArray data);

		QTcpSocket tcpSocket_;

		QByteArray incompleteData_;

		QHash<Protocol::EventKind, EventListener> eventListeners_;

		// Each entry is a full message which is ready to be parsed & handled
		QList<QByteArray> messagesReadyForProcessing_;

		bool vmAlive_{};

		// Stores if all events agree to resume
		enum class ResumeRequest : int  {NEUTRAL, RESUME, DONTRESUME} resumeRequest_{};
};

inline void DebugConnector::addEventListener(Protocol::EventKind kind, DebugConnector::EventListener listener)
{ eventListeners_[kind] = listener; }
inline bool DebugConnector::vmAlive() { return vmAlive_; }

} /* namespace OODebug */
