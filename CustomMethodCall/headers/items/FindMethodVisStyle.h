/***********************************************************************************************************************
 * FindMethodVisStyle.h
 *
 *  Created on: Mar 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef FINDMETHODVISSTYLE_H_
#define FINDMETHODVISSTYLE_H_

#include "../custommethodcall_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VListStyle.h"

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API FindMethodVisStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::TextStyle name_;
		Visualization::SymbolStyle separator_;
		Visualization::VListStyle arguments_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::TextStyle& name() const;
		const Visualization::SymbolStyle& separator() const;
		const Visualization::VListStyle& arguments() const;

		static FindMethodVisStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& FindMethodVisStyle::layout() const { return layout_; }
inline const Visualization::TextStyle& FindMethodVisStyle::name() const { return name_; }
inline const Visualization::SymbolStyle& FindMethodVisStyle::separator() const { return separator_; }
inline const Visualization::VListStyle& FindMethodVisStyle::arguments() const { return arguments_; }

}

#endif /* FINDMETHODVISSTYLE_H_ */
