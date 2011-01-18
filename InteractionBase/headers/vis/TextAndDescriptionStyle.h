/***********************************************************************************************************************
 * TextAndDescriptionStyle.h
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEXTANDDESCRIPTIONSTYLE_H_
#define TEXTANDDESCRIPTIONSTYLE_H_

#include "interactionbase_api.h"

#include "VisualizationBase/headers/items/ItemStyle.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayoutStyle.h"

namespace Interaction {

class INTERACTIONBASE_API TextAndDescriptionStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::TextStyle text_;
		Visualization::TextStyle description_;

	public:
		TextAndDescriptionStyle();
		void load();

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::TextStyle&  text() const;
		const Visualization::TextStyle&  description() const;

		static TextAndDescriptionStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& TextAndDescriptionStyle::layout() const {return layout_; }
inline const Visualization::TextStyle& TextAndDescriptionStyle::text() const {return text_; }
inline const Visualization::TextStyle& TextAndDescriptionStyle::description() const {return description_; }

}

#endif /* TEXTANDDESCRIPTIONSTYLE_H_ */
