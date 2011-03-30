/***********************************************************************************************************************
 * VStringLiteralStyle.h
 *
 *  Created on: Mar 10, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VSTRINGLITERALSTYLE_H_
#define VSTRINGLITERALSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/StaticStyle.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VStringLiteralStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::TextStyle string_;
		Visualization::StaticStyle preSymbol_;
		Visualization::StaticStyle postSymbol_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::TextStyle& string() const;
		const Visualization::StaticStyle& preSymbol() const;
		const Visualization::StaticStyle& postSymbol() const;
};

inline const Visualization::SequentialLayoutStyle& VStringLiteralStyle::layout() const { return layout_; }
inline const Visualization::TextStyle& VStringLiteralStyle::string() const { return string_; }
inline const Visualization::StaticStyle& VStringLiteralStyle::preSymbol() const { return preSymbol_; }
inline const Visualization::StaticStyle& VStringLiteralStyle::postSymbol() const { return postSymbol_; }

}

#endif /* VSTRINGLITERALSTYLE_H_ */
