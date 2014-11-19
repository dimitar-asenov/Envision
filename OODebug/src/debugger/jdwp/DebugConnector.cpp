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

#include "Protocol.h"
#include "Reply.h"
#include "Command.h"

#include "reply/VersionInfo.h"
#include "command/SetCommand.h"

namespace OODebug {

DebugConnector::DebugConnector()
{
	QObject::connect(tcpSocket_, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
						  this, &DebugConnector::handleSocketError);
}

DebugConnector::~DebugConnector()
{
	tcpSocket_->abort();
	delete tcpSocket_;
}

void DebugConnector::connect(QString vmHostName, int vmHostPort)
{
	if (tcpSocket_->isOpen())
		tcpSocket_->close();
	// The connection setup is handled here:
	// First when we are connected we have to send the handshake. For receiving the hanshake reply,
	// we use a dedicated function which disconnects itself and connects the actual read function
	// to the readyRead slot. In this way we don't have the handshake handling in the normal read function.
	QObject::connect(tcpSocket_, &QTcpSocket::readyRead, this, &DebugConnector::readHandshake);
	QObject::connect(tcpSocket_, &QTcpSocket::connected, this, &DebugConnector::sendHandshake);
	tcpSocket_->connectToHost(vmHostName, vmHostPort);
}

void DebugConnector::handleSocketError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "ERROR: " << socketError;
	qWarning() << socketError;
}

void DebugConnector::read()
{
	QByteArray read =	tcpSocket_->readAll();
	// If we still have a part of a packet add it here.
	if (!incompleteData_.isEmpty()) {
		read.prepend(incompleteData_);
		incompleteData_ = QByteArray();
	}
	// If we haven't read enough retry later
	if (read.size() < int(sizeof(qint32))) {
		incompleteData_ = read;
		return;
	}
	// check if the packet is complete
	QDataStream inStream(read);
	qint32 len;
	inStream >> len;
	if (len > read.length()) {
		incompleteData_ = read;
		return;
	}
	// We have read the whole data for this message so handle it.
	qint32 id;
	inStream >> id;
	handlePacket(id, read);
}

void DebugConnector::sendCommand(Command& c, HandleFunction handler)
{
	handlingMap_[c.id()] = handler;
	QByteArray raw;
	QDataStream stream(&raw, QIODevice::ReadWrite);
	stream << c;
	// Insert the length, it is always at position 0
	QByteArray len;
	qint32 dataLen = raw.length();
	QDataStream lenStream(&len, QIODevice::ReadWrite);
	lenStream << dataLen;
	raw.replace(0, len.length(), len);
	tcpSocket_->write(raw);
}

void DebugConnector::readHandshake()
{
	QByteArray read =	tcpSocket_->readAll();
	if (read == Protocol::handshake)
	{
		QObject::disconnect(tcpSocket_, &QTcpSocket::readyRead, this, &DebugConnector::readHandshake);
		QObject::connect(tcpSocket_, &QTcpSocket::readyRead, this, &DebugConnector::read);
		// TODO: This is just for testing now
		sendVersionRequest();
	}
	else
	{
		qWarning() << "Handshake not received: " << read;
	}
}

void DebugConnector::sendHandshake()
{
	tcpSocket_->write(Protocol::handshake.toLatin1());
}

void DebugConnector::handlePacket(qint32 id, QByteArray data)
{
	auto handlingFun = handlingMap_.take(id);
	if (handlingFun) // We received a reply
	{
		// check for an error
		auto r = makeReply<Reply>(data);
		if (Protocol::Error::NONE == r.error())
		{
			handlingFun(*this, data);
		}
		else
		{
			// TODO: handle this better
			qWarning() << "Error received: " << static_cast<int>(r.error());
		}
	}
	else // We received a command
	{
		// TODO: handle commands
		qWarning() << "Command received" << data.toHex();
	}
}

void DebugConnector::sendVersionRequest()
{
	Command c(nextId(), Protocol::CommandSet::VirtualMachine, Protocol::VirtualMachineCommands::Version);
	sendCommand(c, &DebugConnector::handleVersion);
}

void DebugConnector::handleVersion(QByteArray data)
{
	auto info = makeReply<VersionInfo>(data);
	qDebug() << "VM-INFO: " << info.jdwpMajor() << info.jdwpMinor();
	sendBreakAtStart();
}

void DebugConnector::sendBreakAtStart()
{
	QString pattern{"Test"};
	BreakClassLoad	load(nextId(), pattern);
	sendCommand(load, nullptr);
}

} /* namespace OODebug */
