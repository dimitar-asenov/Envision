/***********************************************************************************************************************
 * CommandSuggestion.h
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef COMMANDSUGGESTION_H_
#define COMMANDSUGGESTION_H_

#include "interactionbase_api.h"

#include "VisualizationBase/headers/items/Item.h"

namespace Interaction {

class INTERACTIONBASE_API CommandSuggestion
{
	public:
		CommandSuggestion(const QString& suggestion = QString(), const QString& description = QString(), Visualization::Item* visualization = NULL);
		virtual ~CommandSuggestion();

		const QString& suggestion();
		const QString& description();
		Visualization::Item* visualization();

		void setSuggestion(const QString& suggestion);
		void setDescription(const QString& description);
		void setVisualization(Visualization::Item* item);

	private:
		QString suggestedCommand;
		QString commandDescription;
		Visualization::Item* suggestionVisualization;

};

inline const QString& CommandSuggestion::suggestion() { return suggestedCommand; }
inline const QString& CommandSuggestion::description() { return commandDescription; }
inline Visualization::Item* CommandSuggestion::visualization() { return suggestionVisualization; }
inline void CommandSuggestion::setSuggestion(const QString& suggestion) { suggestedCommand = suggestion; }
inline void CommandSuggestion::setDescription(const QString& description) { commandDescription = description; }
inline void CommandSuggestion::setVisualization(Visualization::Item* item) { suggestionVisualization = item; }

}

#endif /* COMMANDSUGGESTION_H_ */
