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

#include "DebugConnector.h"

#include "Reply.h"
#include "Command.h"

#include "messages/AllMessages.h"

namespace OODebug {

DebugConnector::DebugConnector()
{
	QObject::connect(&tcpSocket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
						  this, &DebugConnector::handleSocketError);
}

DebugConnector::~DebugConnector()
{
	tcpSocket_.abort();
}

void DebugConnector::connect(QString vmHostName, int vmHostPort)
{
	if (tcpSocket_.isOpen())
		tcpSocket_.abort();
	QObject::disconnect(&tcpSocket_, &QTcpSocket::readyRead, this, &DebugConnector::dispatchEvents);
	Command::resetIds();
	// The connection setup is handled here:
	// First when we are connected we have to send the handshake. For receiving the hanshake reply,
	// we use a dedicated function which disconnects itself and connects the actual read function
	// to the readyRead slot. In this way we don't have the handshake handling in the normal read function.
	QObject::connect(&tcpSocket_, &QTcpSocket::readyRead, this, &DebugConnector::readHandshake);
	tcpSocket_.connectToHost(vmHostName, vmHostPort);
	tcpSocket_.waitForConnected();
	sendHandshake();
}

bool DebugConnector::suspend()
{
	if (vmAlive_)
	{
		auto r = makeReply<Reply>(sendCommand(SuspendCommand{}));
		return Protocol::Error::NONE == r.error();
	}
	return false;
}

bool DebugConnector::resume()
{
	if (vmAlive_)
	{
		auto r = makeReply<Reply>(sendCommand(ResumeCommand{}));
		return Protocol::Error::NONE == r.error();
	}
	return false;
}

void DebugConnector::wantResume(bool resume)
{
	if (resume)
	{
		if (resumeRequest_ != ResumeRequest::DONTRESUME) resumeRequest_ = ResumeRequest::RESUME;
	}
	else
	{
		resumeRequest_ = ResumeRequest::DONTRESUME;
	}
}

QString DebugConnector::fileNameForReference(qint64 referenceId)
{
	auto reply = makeReply<SourceFile>(sendCommand(SourceFileCommand{referenceId}));
	return reply.sourceFile();
}

QString DebugConnector::signatureOf(qint64 referenceId)
{
	auto reply = makeReply<Signature>(sendCommand(SignatureCommand{referenceId}));
	return reply.signature();
}

qint64 DebugConnector::classIdOf(const QString& signature)
{
	auto classesBySignature = makeReply<ClassesBySignature>(sendCommand(ClassesBySignatureCommand{signature}));
	if (classesBySignature.classes().size() < 1)
		return NO_RESULT;
	// If we have more than one class id for one signature, we should check what the situation is and
	// adapt the following code to handle this situation correctly.
	Q_ASSERT(classesBySignature.classes().size() == 1);
	return classesBySignature.classes()[0].typeID();
}

qint64 DebugConnector::methodIdOf(qint64 classId, const QString& signature)
{
	auto methods = makeReply<Methods>(sendCommand(MethodsCommand{classId}));
	for (auto method : methods.methods())
	{
		// TODO: check for signature
		if (method.name() == signature) return method.methodID();
	}
	return NO_RESULT;
}

LineTable DebugConnector::lineTable(qint64 classId, qint64 methodId)
{
	return makeReply<LineTable>(sendCommand(LineTableCommand{classId, methodId}));
}

QList<qint64> DebugConnector::allThreadIds()
{
	auto reply = makeReply<AllThreads>(sendCommand(AllThreadsCommand{}));
	return reply.threadIds();
}

QString DebugConnector::threadName(qint64 threadId)
{
	auto reply = makeReply<ThreadName>(sendCommand(ThreadNameCommand{threadId}));
	return reply.threadName();
}

Frames DebugConnector::frames(qint64 threadId, qint32 numFrames, qint32 startFrame)
{
	return makeReply<Frames>(sendCommand(FramesCommand{threadId, startFrame, numFrames}));
}

VariableTable DebugConnector::variableTableForMethod(qint64 classId, qint64 methodId)
{
	return makeReply<VariableTable>(sendCommand(VariableTableCommand{classId, methodId}));
}

Values DebugConnector::values(qint64 threadId, qint64 frameId, QList<StackVariable> variables)
{
	return makeReply<Values>(sendCommand(GetValuesCommand{threadId, frameId, variables}));
}

QString DebugConnector::stringFromId(qint64 stringId)
{
	auto reply = makeReply<StringValue>(sendCommand(StringValueCommand{stringId}));
	return reply.stringValue();
}

int DebugConnector::arrayLength(qint64 arrayId)
{
	auto r = makeReply<Length>(sendCommand(LengthCommand{arrayId}));
	return r.arrayLength();
}

ArrayValues DebugConnector::arrayValues(qint64 arrayId, qint32 firstIndex, qint32 length)
{
	return makeReply<ArrayValues>(sendCommand(GetArrayValuesCommand{arrayId, firstIndex, length}));
}

bool DebugConnector::breakAtClassLoad(QString className)
{
	auto r = makeReply<Reply>(sendCommand(BreakClassLoad{className}));
	return Protocol::Error::NONE == r.error();
}

int DebugConnector::setBreakpoint(Location breakLocation)
{
	auto r = makeReply<EventSetReply>(sendCommand(BreakpointCommand{breakLocation}));
	return r.requestId();
}

bool DebugConnector::clearBreakpoint(qint32 requestId)
{
	auto r = makeReply<Reply>(sendCommand(EventClearCommand{Protocol::EventKind::BREAKPOINT, requestId}));
	return r.error() == Protocol::Error::NONE;
}

int DebugConnector::singleStep(qint64 threadId, Protocol::StepSize stepSize, Protocol::StepDepth stepDepth)
{
	auto r = makeReply<EventSetReply>(sendCommand(StepCommand{threadId, stepSize, stepDepth}));
	return r.requestId();
}

void DebugConnector::handleSocketError(QAbstractSocket::SocketError socketError)
{
	// If the VM just went off we will receive a remote closed error this expected:
	if (socketError == QAbstractSocket::RemoteHostClosedError && !vmAlive_) return;
	qDebug() << "Socket ERROR: " << socketError;
}

void DebugConnector::dispatchEvents()
{
	readFromSocket();
	auto it = messagesReadyForProcessing_.begin();
	while (it != messagesReadyForProcessing_.end())
	{
		auto data = *it;
		auto command = makeReply<Command>(data);
		if (command.commandSet() == Protocol::CommandSet::Event
			 && command.command() == MessagePart::cast(Protocol::EventCommands::Composite))
		{
			messagesReadyForProcessing_.erase(it);
			handleComposite(data);
			// The handling of one event might mess with our iterator so just emit the signal that we get called again
			// once the handling is done
			Q_EMIT tcpSocket_.readyRead();
			break;
		}
		else
		{
			++it;
		}
	}
}

void DebugConnector::readFromSocket()
{
	QByteArray dataRead = tcpSocket_.readAll();
	// If we still have a part of a packet add it here.
	if (!incompleteData_.isEmpty())
	{
		dataRead.prepend(incompleteData_);
		incompleteData_ = QByteArray{};
	}
	// If we haven't read enough retry later.
	if (dataRead.size() < int(sizeof(qint32)))
	{
		incompleteData_ = dataRead;
		return;
	}
	// check if the packet is complete
	QDataStream inStream{dataRead};
	qint32 packetLen;
	inStream >> packetLen;
	if (packetLen > dataRead.length())
	{
		// We don't have a complete packet yet, try later.
		incompleteData_ = dataRead;
		return;
	}
	// It might be that we received more than one message, so we store the additional data.
	if (packetLen < dataRead.length())
	{
		incompleteData_ = dataRead;
		incompleteData_.remove(0, packetLen);
		dataRead.truncate(packetLen);
	}
	messagesReadyForProcessing_ << dataRead;
}

QByteArray DebugConnector::sendCommand(const Command& command)
{
	QByteArray raw;
	QDataStream stream{&raw, QIODevice::ReadWrite};
	stream << command;
	// Insert the length, it is always at position 0
	QByteArray len;
	qint32 dataLen = raw.length();
	QDataStream lenStream{&len, QIODevice::ReadWrite};
	lenStream << dataLen;
	raw.replace(0, len.length(), len);
	tcpSocket_.write(raw);
	tcpSocket_.waitForBytesWritten();

	return waitForReply(command.id());
}

QByteArray DebugConnector::waitForReply(qint32 requestId)
{
	while (true)
	{
		// First check if the data is already here
		for (int i = 0; i < messagesReadyForProcessing_.length(); ++i)
		{
			auto r = makeReply<Reply>(messagesReadyForProcessing_[i]);
			if (r.id() == requestId) return messagesReadyForProcessing_.takeAt(i);
		}
		tcpSocket_.waitForReadyRead();
		readFromSocket();
	}
}

void DebugConnector::readHandshake()
{
	QByteArray dataRead = tcpSocket_.readAll();
	Q_ASSERT(dataRead.length() >= Protocol::handshake.length());
	if (dataRead.startsWith(Protocol::handshake))
	{
		QObject::disconnect(&tcpSocket_, &QTcpSocket::readyRead, this, &DebugConnector::readHandshake);
		QObject::connect(&tcpSocket_, &QTcpSocket::readyRead, this, &DebugConnector::dispatchEvents);
		if (dataRead.length() > Protocol::handshake.length())
		{
			// remove the handshake
			dataRead.remove(0, Protocol::handshake.length());
			incompleteData_ = dataRead;
			// trigger reads such that we handle the additional data
			readFromSocket();
		}
		checkVersion();
		checkIdSizes();
		dispatchEvents();
	}
	else
	{
		// We didn't receive the handshake, this should never happen, so we just stop here.
		// In the future we should handle this somehow nicer.
		Q_ASSERT(false);
	}
}

void DebugConnector::sendHandshake()
{
	tcpSocket_.write(Protocol::handshake);
	tcpSocket_.waitForBytesWritten();
}

void DebugConnector::checkVersion()
{
	auto info = makeReply<VersionInfo>(sendCommand(VersionCommand{}));
	qDebug() << "VM-INFO: " << info.jdwpMajor() << info.jdwpMinor();
}

void DebugConnector::checkIdSizes()
{
	// Our protocol is only designed for 64 bit machine make sure that this is the case:
	auto idSizes = makeReply<IDSizes>(sendCommand(IDSizeCommand{}));
	Q_ASSERT(sizeof(qint64) == idSizes.fieldIDSize());
	Q_ASSERT(sizeof(qint64) == idSizes.methodIDSize());
	Q_ASSERT(sizeof(qint64) == idSizes.objectIDSize());
	Q_ASSERT(sizeof(qint64) == idSizes.referenceTypeIDSize());
	Q_ASSERT(sizeof(qint64) == idSizes.frameIDSize());
}

void DebugConnector::handleComposite(QByteArray data)
{
	auto c = makeReply<CompositeCommand>(data);
	// If we receive a non composite event, we have an implementation error.
	Q_ASSERT(c.commandSet() == Protocol::CommandSet::Event);
	Q_ASSERT(c.command() == MessagePart::cast(Protocol::EventCommands::Composite));

	resumeRequest_ = ResumeRequest::NEUTRAL;

	for (auto& event : c.events())
	{
		// We always want to know the VM state
		if (Protocol::EventKind::VM_START == event.eventKind())
			vmAlive_ = true;
		else if (Protocol::EventKind::VM_DEATH == event.eventKind())
			vmAlive_ = false;
		// Dispatch event to listeners
		if (auto listener = eventListeners_[event.eventKind()])
			listener(event);
		else
			qDebug() << "EVENT" << static_cast<int>(event.eventKind());
	}

	// If we have multiple events, we might have a breakpoint from a probe which would be auto resumed
	// and also a single step event for which we don't want to resume. So check if all agree on resuming.
	if (resumeRequest_ == ResumeRequest::RESUME) resume();
}

}
