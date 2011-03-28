/***********************************************************************************************************************
 * InsertMethodVisStyle.h
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef INSERTMETHODVISSTYLE_H_
#define INSERTMETHODVISSTYLE_H_

#include "../custommethodcall_api.h"

#include "VisualizationBase/headers/icons/SVGIconStyle.h"
#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VListStyle.h"

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API InsertMethodVisStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::SVGIconStyle icon_;
		Visualization::SymbolStyle separator_;
		Visualization::VListStyle arguments_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::SVGIconStyle& icon() const;
		const Visualization::SymbolStyle& separator() const;
		const Visualization::VListStyle& arguments() const;
};

inline const Visualization::SequentialLayoutStyle& InsertMethodVisStyle::layout() const { return layout_; }
inline const Visualization::SVGIconStyle& InsertMethodVisStyle::icon() const { return icon_; }
inline const Visualization::SymbolStyle& InsertMethodVisStyle::separator() const { return separator_; }
inline const Visualization::VListStyle& InsertMethodVisStyle::arguments() const { return arguments_; }

}

#endif /* INSERTMETHODVISSTYLE_H_ */
