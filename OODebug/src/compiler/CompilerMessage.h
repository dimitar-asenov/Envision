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

#include "../oodebug_api.h"

namespace OODebug {

/**
 * Represents a single message from the compiler. This may be an \a Error, a \a Warning, a \a Note,
 * or something \a Other.
 *
 * A note might have a \a rootMessage, which represents the error or warning this note is connected to.
 */
class OODEBUG_API CompilerMessage
{
	public:
		enum MessageType { Error, Warning, Note, Other};
		CompilerMessage(MessageType type, const QString& fileName, const QString& message, int lineNumber,
							 int columnNumber, std::shared_ptr<CompilerMessage> rootMessage = nullptr) :
			type_{type}, fileName_{fileName}, message_{message},
			lineNumber_{lineNumber}, columnNumber_{columnNumber}, rootMessage_{rootMessage} {}

		MessageType type() const;
		QString fileName() const;
		QString message() const;
		int lineNumber() const;
		int columnNumber() const;
		std::shared_ptr<CompilerMessage> rootMessage() const;

	private:
		MessageType type_{};
		QString fileName_;
		QString message_;
		int lineNumber_{};
		int columnNumber_{};
		std::shared_ptr<CompilerMessage> rootMessage_;
};

inline CompilerMessage::MessageType CompilerMessage::type() const { return type_;}
inline QString CompilerMessage::fileName() const { return fileName_;}
inline QString CompilerMessage::message() const { return message_;}
inline int CompilerMessage::lineNumber() const { return lineNumber_;}
inline int CompilerMessage::columnNumber() const { return columnNumber_;}
inline std::shared_ptr<CompilerMessage> CompilerMessage::rootMessage() const { return rootMessage_;}

} /* namespace OODebug */
