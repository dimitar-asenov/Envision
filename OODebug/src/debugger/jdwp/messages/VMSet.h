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

#include "../../../oodebug_api.h"

#include "../Command.h"
#include "../Protocol.h"
#include "../Reply.h"

namespace OODebug {

struct OODEBUG_API VersionCommand : public Command {
	VersionCommand();
	virtual ~VersionCommand() override;
};

// Replace ([^\t]+)\t([^\t]+)\t[^\n]*\n with MessageField<\1> \2{this};\n

struct OODEBUG_API VersionInfo : public Reply {
		virtual ~VersionInfo() override;

		MessageField<QString> description{&VersionInfo::description, this};
		MessageField<qint32> jdwpMajor{&VersionInfo::jdwpMajor, this};
		MessageField<qint32> jdwpMinor{&VersionInfo::jdwpMinor, this};
		MessageField<QString> vmVersion{&VersionInfo::vmVersion, this};
		MessageField<QString> vmName{&VersionInfo::vmName, this};
};

struct OODEBUG_API ClassesBySignatureCommand : public Command {
		ClassesBySignatureCommand(QString signature);
		virtual ~ClassesBySignatureCommand() override;

		MessageField<QString> signature{&ClassesBySignatureCommand::signature, this};
};

struct OODEBUG_API ClassBySignature : public MessagePart {
		virtual ~ClassBySignature() override;
		MessageField<Protocol::TypeTagKind> refTypeTag{&ClassBySignature::refTypeTag, this};
		MessageField<qint64> typeID{&ClassBySignature::typeID, this};
		MessageField<Protocol::ClassStatus> status{&ClassBySignature::status, this};
};

struct OODEBUG_API ClassesBySignature : public Reply {
		virtual ~ClassesBySignature() override;
		MessageField<QList<ClassBySignature>> classes{&ClassesBySignature::classes, this};
};

struct OODEBUG_API AllThreadsCommand : public Command {
		AllThreadsCommand();
		virtual ~AllThreadsCommand() override;
};

struct OODEBUG_API AllThreads : public Reply {
		virtual ~AllThreads() override;
		MessageField<QList<qint64>> threadIds{&AllThreads::threadIds, this};
};

struct OODEBUG_API IDSizeCommand : public Command {
		IDSizeCommand();
		virtual ~IDSizeCommand() override;
};

struct OODEBUG_API IDSizes : public Reply {
		virtual ~IDSizes() override;

		MessageField<int> fieldIDSize{&IDSizes::fieldIDSize, this};
		MessageField<int> methodIDSize{&IDSizes::methodIDSize, this};
		MessageField<int> objectIDSize{&IDSizes::objectIDSize, this};
		MessageField<int> referenceTypeIDSize{&IDSizes::referenceTypeIDSize, this};
		MessageField<int> frameIDSize{&IDSizes::frameIDSize, this};
};

struct OODEBUG_API SuspendCommand : public Command {
		SuspendCommand();
		virtual ~SuspendCommand() override;
};

struct OODEBUG_API ResumeCommand : public Command {
		ResumeCommand();
		virtual ~ResumeCommand() override;
};

}
