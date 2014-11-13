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

namespace OODebug {

class Command;

class OODEBUG_API DebugConnector : public QObject
{
	Q_OBJECT

	public:
		DebugConnector(QString vmHostName = "localhost", int vmHostPort = 4000);
		~DebugConnector();

	private:
		using HandleFunction = std::function<void(DebugConnector&, const QByteArray&)>;

		static void handleSocketError(QAbstractSocket::SocketError socketError);
		void read();

		void sendCommand(Command& c, HandleFunction handler);

		void readHandshake();
		void sendHandshake();
		/**
		 * Parses and handles the \a data of a complete read packet.
		 */
		void handlePacket(qint32 id, QByteArray data);


		void sendVersionRequest();
		void handleVersion(QByteArray data);


		inline qint32 nextId();

		QHash<int, HandleFunction> handlingMap_;

		QTcpSocket* tcpSocket_{new QTcpSocket()};

		QByteArray incompleteData_;

		qint32 id_{};
};

qint32 DebugConnector::nextId() { return ++id_; }

} /* namespace OODebug */
