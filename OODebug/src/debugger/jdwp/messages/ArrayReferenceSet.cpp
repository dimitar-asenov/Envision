/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "ArrayReferenceSet.h"


namespace OODebug {

LengthCommand::LengthCommand(qint64 arrayId)
: Command{Protocol::CommandSet::ArrayReference, Protocol::ArrayReferenceCommands::Length}
{
	arrayID = arrayId;
}

LengthCommand::~LengthCommand() {}
Length::~Length() {}

GetArrayValuesCommand::GetArrayValuesCommand(qint64 arrayId, qint32 firstIndex, qint32 length)
: Command{Protocol::CommandSet::ArrayReference, Protocol::ArrayReferenceCommands::GetValues}
{
	arrayObject = arrayId;
	this->firstIndex = firstIndex;
	this->length = length;
}

GetArrayValuesCommand::~GetArrayValuesCommand() {}
ArrayValues::~ArrayValues() {}

int ArrayValues::kind() const
{
	// check if type is object value then return 0, otherwise return casted value
	if (type() == Protocol::Tag::ARRAY || type() == Protocol::Tag::OBJECT || type() == Protocol::Tag::STRING)
		return 0;
	return cast(type());
}

}
