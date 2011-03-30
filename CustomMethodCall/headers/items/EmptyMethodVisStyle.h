/***********************************************************************************************************************
 * EmptyMethodVisStyle.h
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EMPTYMETHODVISSTYLE_H_
#define EMPTYMETHODVISSTYLE_H_

#include "../custommethodcall_api.h"

#include "VisualizationBase/headers/items/StaticStyle.h"
#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API EmptyMethodVisStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::StaticStyle icon_;
		Visualization::SymbolStyle separator_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::StaticStyle& icon() const;
		const Visualization::SymbolStyle& separator() const;
};

inline const Visualization::SequentialLayoutStyle& EmptyMethodVisStyle::layout() const { return layout_; }
inline const Visualization::StaticStyle& EmptyMethodVisStyle::icon() const { return icon_; }
inline const Visualization::SymbolStyle& EmptyMethodVisStyle::separator() const { return separator_; }

}

#endif /* EMPTYMETHODVISSTYLE_H_ */
