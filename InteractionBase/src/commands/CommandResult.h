/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * CommandResult.h
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef COMMANDRESULT_H_
#define COMMANDRESULT_H_

#include "../interactionbase_api.h"

#include "CommandError.h"
#include "CommandSuggestion.h"

namespace Interaction {

class INTERACTIONBASE_API CommandResult
{
	public:
		enum Code {OK, CanNotInterpret, Error};

		CommandResult(Code resultCode = OK);
		CommandResult(CommandError* error);
		CommandResult(CommandError* error, CommandSuggestion* suggestion);
		virtual ~CommandResult();

		Code code() const;
		const QList<CommandError*>& errors() const;
		const QList<CommandSuggestion*>& suggestions() const;

		void setCode(Code code);
		void addError(CommandError* error);
		void addSuggestion(CommandSuggestion* suggestion);

	private:
		Code outcomeCode;
		QList<CommandError*> errors_;
		QList<CommandSuggestion*> suggestions_;
};

inline CommandResult::Code CommandResult::code() const { return outcomeCode; };
inline const QList<CommandError*>& CommandResult::errors() const { return errors_; };
inline const QList<CommandSuggestion*>& CommandResult::suggestions() const {return suggestions_; };

}

#endif /* COMMANDRESULT_H_ */
