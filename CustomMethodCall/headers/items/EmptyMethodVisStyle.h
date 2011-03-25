/***********************************************************************************************************************
 * EmptyMethodVisStyle.h
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EMPTYMETHODVISSTYLE_H_
#define EMPTYMETHODVISSTYLE_H_

#include "../custommethodcall_api.h"

#include "VisualizationBase/headers/icons/SVGIconStyle.h"
#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API EmptyMethodVisStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::SVGIconStyle icon_;
		Visualization::SymbolStyle separator_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::SVGIconStyle& icon() const;
		const Visualization::SymbolStyle& separator() const;

		static EmptyMethodVisStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& EmptyMethodVisStyle::layout() const { return layout_; }
inline const Visualization::SVGIconStyle& EmptyMethodVisStyle::icon() const { return icon_; }
inline const Visualization::SymbolStyle& EmptyMethodVisStyle::separator() const { return separator_; }

}

#endif /* EMPTYMETHODVISSTYLE_H_ */
