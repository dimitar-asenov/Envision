/***********************************************************************************************************************
 * CommandExecutionEngine.h
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef COMMANDEXECUTIONENGINE_H_
#define COMMANDEXECUTIONENGINE_H_

#include "interactionbase_api.h"

#include "VisualizationBase/headers/items/Item.h"

#include <QtCore/QString>
#include <QtCore/QList>

namespace Interaction {

class CommandResult;

class INTERACTIONBASE_API CommandExecutionEngine
{
	public:
		virtual ~CommandExecutionEngine();
		static CommandExecutionEngine* instance();

		virtual void execute(Visualization::Item *target, const QString& command);
		CommandResult* acquireResult();
		void deleteLastCommandResult();

	protected:
		CommandExecutionEngine();

		virtual Visualization::Item* navigate(Visualization::Item *originator, const QString& navigationString);

		virtual QStringList tokenize(const QString&string, const QString& quoteSymbols, const QString& escapeSymbols);
		virtual QStringList tokenizeNonQuoted(const QString&string);
		virtual bool doQuotesMatch(const QString& string, const QString& quoteSymbols, const QString& escapeSymbols);
		virtual bool isEscaped(const QString& string, int indexToCheck, const QString& escapeSymbols);

	private:
		CommandResult* lastCommandResult;
		bool resultIsExternallyAcquired;
};

}

#endif /* COMMANDEXECUTIONENGINE_H_ */
