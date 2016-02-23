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
#include "../Reply.h"

namespace OODebug {

struct OODEBUG_API SignatureCommand : public Command {
		SignatureCommand(qint64 referenceId);
		virtual ~SignatureCommand() override;
		MessageField<qint64> refType{&SignatureCommand::refType, this};
};

struct OODEBUG_API Signature : public Reply {
		virtual ~Signature() override;
		MessageField<QString> signature{&Signature::signature, this};
};

struct OODEBUG_API JVMMethod : public MessagePart {
		virtual ~JVMMethod() override;
		MessageField<qint64> methodID{&JVMMethod::methodID, this};
		MessageField<QString> name{&JVMMethod::name, this};
		MessageField<QString> signature{&JVMMethod::signature, this};
		MessageField<qint32> modBits{&JVMMethod::modBits, this};
};

struct OODEBUG_API MethodsCommand : public Command {
		MethodsCommand(qint64 classId);
		virtual ~MethodsCommand() override;
		MessageField<qint64> refTypeId{&MethodsCommand::refTypeId, this};
};

struct OODEBUG_API Methods : public Reply {
		virtual ~Methods() override;
		MessageField<QList<JVMMethod>> methods{&Methods::methods, this};
};

struct OODEBUG_API SourceFileCommand : public Command {
		SourceFileCommand(qint64 referenceId);
		virtual ~SourceFileCommand() override;
		MessageField<qint64> refType{&SourceFileCommand::refType, this};
};

struct OODEBUG_API SourceFile : public Reply {
		virtual ~SourceFile() override;
		MessageField<QString> sourceFile{&SourceFile::sourceFile, this};
};

}
