/***********************************************************************************************************************
 * CommandPromptStyle.h
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef COMMANDPROMPTSTYLE_H_
#define COMMANDPROMPTSTYLE_H_

#include "interactionbase_api.h"

#include "TextAndDescriptionStyle.h"

namespace Interaction {

class INTERACTIONBASE_API CommandPromptStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::SequentialLayoutStyle suggestionContainer_;
		Visualization::SequentialLayoutStyle errorContainer_;
		Visualization::TextStyle commandText_;
		TextAndDescriptionStyle defaultSuggestion_;
		TextAndDescriptionStyle defaultError_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::SequentialLayoutStyle& suggestionContainer() const;
		const Visualization::SequentialLayoutStyle& errorContainer() const;
		const Visualization::TextStyle&  commandText() const;
		const TextAndDescriptionStyle&  defaultSuggestion() const;
		const TextAndDescriptionStyle&  defaultError() const;
};

inline const Visualization::SequentialLayoutStyle& CommandPromptStyle::layout() const {return layout_; }
inline const Visualization::SequentialLayoutStyle& CommandPromptStyle::suggestionContainer() const {return suggestionContainer_; }
inline const Visualization::SequentialLayoutStyle& CommandPromptStyle::errorContainer() const {return errorContainer_; }
inline const Visualization::TextStyle& CommandPromptStyle::commandText() const {return commandText_; }
inline const TextAndDescriptionStyle& CommandPromptStyle::defaultSuggestion() const {return defaultSuggestion_; }
inline const TextAndDescriptionStyle& CommandPromptStyle::defaultError() const {return defaultError_; }
}

#endif /* COMMANDPROMPTSTYLE_H_ */
