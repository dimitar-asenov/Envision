/***********************************************************************************************************************
 * CommandResult.h
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef COMMANDRESULT_H_
#define COMMANDRESULT_H_

#include "interactionbase_api.h"

#include "CommandError.h"
#include "CommandSuggestion.h"

#include <QtCore/QList>

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
