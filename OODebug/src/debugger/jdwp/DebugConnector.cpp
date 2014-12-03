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

void DebugConnector::connect(QString mainClassName, QString vmHostName, int vmHostPort)
{
	mainClassName_ = mainClassName;
	if (tcpSocket_.isOpen())
		tcpSocket_.abort();
	QObject::disconnect(&tcpSocket_, &QTcpSocket::readyRead, this,
							  static_cast<void (DebugConnector::*)()>(&DebugConnector::read));
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

void DebugConnector::handleSocketError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "ERROR: " << socketError;
	qDebug() << socketError;
}

void DebugConnector::read()
{
	read(-1);
}

int DebugConnector::read(qint32 requestId)
{
	QByteArray read = tcpSocket_.readAll();
	// If we still have a part of a packet add it here.
	if (!incompleteData_.isEmpty())
	{
		read.prepend(incompleteData_);
		incompleteData_ = QByteArray();
	}
	// If we haven't read enough retry later
	if (read.size() < int(sizeof(qint32)))
	{
		incompleteData_ = read;
		return -1;
	}
	// check if the packet is complete
	QDataStream inStream(read);
	qint32 packetLen;
	inStream >> packetLen;
	if (packetLen > read.length())
	{
		incompleteData_ = read;
		return -1;
	}
	// We have read the whole data for this message so handle it.
	qint32 id;
	inStream >> id;

	// It might be that we received more than one packet, if so we have to recursively call this function.
	if (packetLen < read.length())
	{
		incompleteData_ = read;
		incompleteData_.remove(0, packetLen);
		// trigger reads such that we handle the additional data
		if (-1 == id) this->read();
	}
	read.remove(packetLen + 1, read.length());

	if (-1 == requestId)
	{
		handleComposite(read);
		return 0;
	}
	auto nextInd = readyData_.length();
	readyData_ << read;
	if (requestId == id) return nextInd;
	return -1;
}

QByteArray DebugConnector::sendCommand(const Command& command)
{
	QByteArray raw;
	QDataStream stream(&raw, QIODevice::ReadWrite);
	stream << command;
	// Insert the length, it is always at position 0
	QByteArray len;
	qint32 dataLen = raw.length();
	QDataStream lenStream(&len, QIODevice::ReadWrite);
	lenStream << dataLen;
	raw.replace(0, len.length(), len);
	tcpSocket_.write(raw);
	tcpSocket_.waitForBytesWritten();

	int responseIndex = -1;
	while (-1 == responseIndex)
	{
		tcpSocket_.waitForReadyRead();
		responseIndex = read(command.id());
	}
	return readyData_.takeAt(responseIndex);
}

void DebugConnector::readHandshake()
{
	QByteArray read = tcpSocket_.readAll();
	Q_ASSERT(read.length() >= Protocol::handshake.length());
	if (read.startsWith(Protocol::handshake))
	{
		QObject::disconnect(&tcpSocket_, &QTcpSocket::readyRead, this, &DebugConnector::readHandshake);
		QObject::connect(&tcpSocket_, &QTcpSocket::readyRead, this,
							  static_cast<void (DebugConnector::*)()>(&DebugConnector::read));
		if (read.length() > Protocol::handshake.length())
		{
			// remove the handshake
			read.remove(0, Protocol::handshake.length());
			incompleteData_ = read;
			// trigger reads such that we handle the additional data
			this->read();
		}
		checkVersion();
		checkIdSizes();
		sendBreakAtStart();
	}
	else
	{
		qDebug() << "Handshake not received: " << read.toHex() << Protocol::handshake.toHex();
	}
}

void DebugConnector::sendHandshake()
{
	tcpSocket_.write(Protocol::handshake);
	tcpSocket_.waitForBytesWritten();
}

void DebugConnector::checkVersion()
{
	auto info = makeReply<VersionInfo>(sendCommand(VersionCommand()));
	qDebug() << "VM-INFO: " << info.jdwpMajor() << info.jdwpMinor();
}

void DebugConnector::checkIdSizes()
{
	// Our protocol is only designed for 64 bit machine make sure that this is the case:
	auto idSizes = makeReply<IDSizes>(sendCommand(IDSizeCommand()));
	Q_ASSERT(sizeof(qint64) == idSizes.fieldIDSize());
	Q_ASSERT(sizeof(qint64) == idSizes.methodIDSize());
	Q_ASSERT(sizeof(qint64) == idSizes.objectIDSize());
	Q_ASSERT(sizeof(qint64) == idSizes.referenceTypeIDSize());
	Q_ASSERT(sizeof(qint64) == idSizes.frameIDSize());
}

void DebugConnector::sendBreakAtStart()
{
	auto r = makeReply<Reply>(sendCommand(BreakClassLoad(mainClassName_)));
	Q_ASSERT(Protocol::Error::NONE == r.error());
	resume();
}

bool DebugConnector::resume()
{
	auto r = makeReply<Reply>(sendCommand(ResumeCommand()));
	return Protocol::Error::NONE == r.error();
}

qint64 DebugConnector::getClassId(const QString& signature)
{
	auto it = classIdMap_.find(signature);
	if (it == classIdMap_.end())
	{
		auto classesBySignature = makeReply<ClassesBySignature>(sendCommand(ClassesBySignatureCommand(signature)));
		Q_ASSERT(classesBySignature.classes().size() == 1);
		return classIdMap_[signature] = classesBySignature.classes()[0].typeID();
	}
	else
	{
		return it.value();
	}
}

qint64 DebugConnector::getMethodId(qint64 classId, const QString& signature)
{
	auto methods = makeReply<MethodsReply>(sendCommand(MethodsCommand(classId)));
	for (auto method : methods.methods())
	{
		// TODO: check for signature
		if (method.name() == signature) return method.methodID();
	}
	return -1;
}

int DebugConnector::sendBreakpoint(Location breakLocation)
{
	auto r = makeReply<EventSetReply>(sendCommand(BreakpointCommand(breakLocation)));
	return r.requestId();
}

void DebugConnector::handleComposite(QByteArray data)
{
	auto c = makeReply<CompositeCommand>(data);
	// If we receive a non composite event, we have an implementation error.
	Q_ASSERT(c.commandSet() == Protocol::CommandSet::Event);
	Q_ASSERT(c.command() == MessagePart::cast(Protocol::EventCommands::Composite));

	for (auto& event : c.events())
	{
		if (auto listener = eventListeners_[event.eventKind()])
			listener(event);
		else
			qDebug() << "EVENT" << static_cast<int>(event.eventKind());
	}
}

} /* namespace OODebug */
