/***********************************************************************************************************************
 * CommandPrompt.h
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef COMMANDPROMPT_H_
#define COMMANDPROMPT_H_

#include "interactionbase_api.h"
#include "CommandPromptStyle.h"
#include "../commands/CommandSuggestion.h"
#include "../commands/CommandError.h"

#include "VisualizationBase/headers/Styles.h"
#include "VisualizationBase/headers/items/Item.h"
#include "VisualizationBase/headers/items/Text.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"\

#include <QtCore/QList>

namespace Interaction {

class INTERACTIONBASE_API CommandPrompt : public Visualization::Item
{
	ITEM_COMMON(CommandPrompt, Visualization::Item)

	public:
		CommandPrompt(Visualization::Item* commandReceiver, const CommandPromptStyle* style = Visualization::Styles::item<CommandPrompt>("default"));
		virtual ~CommandPrompt();

		void setSuggestions(const QList<CommandSuggestion*>& suggestions);
		void setErrors(const QList<CommandError*>& errors);

		Visualization::Item* commandReceiver();

		QString text() const;
		void initializeCommand();

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::Item* commandReceiver_;
		Visualization::SequentialLayout* layout;
		Visualization::SequentialLayout* suggestionContainer;
		Visualization::SequentialLayout* errorContainer;
		Visualization::Text* command;
		QList<CommandSuggestion*> suggestions;
		QList<CommandError*> errors;

		template <class T>
		void deleteMessages(QList<T*>& messages, Visualization::SequentialLayout*& containerLayout);
};

inline Visualization::Item* CommandPrompt::commandReceiver() { return commandReceiver_; }
inline QString CommandPrompt::text() const {return command->getText();}

}

#endif /* COMMANDPROMPT_H_ */
