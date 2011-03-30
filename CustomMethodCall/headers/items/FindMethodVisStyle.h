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
#include "VisualizationBase/headers/items/StaticStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VListStyle.h"

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API FindMethodVisStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::TextStyle name_;
		Visualization::StaticStyle separator_;
		Visualization::VListStyle arguments_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::TextStyle& name() const;
		const Visualization::StaticStyle& separator() const;
		const Visualization::VListStyle& arguments() const;
};

inline const Visualization::SequentialLayoutStyle& FindMethodVisStyle::layout() const { return layout_; }
inline const Visualization::TextStyle& FindMethodVisStyle::name() const { return name_; }
inline const Visualization::StaticStyle& FindMethodVisStyle::separator() const { return separator_; }
inline const Visualization::VListStyle& FindMethodVisStyle::arguments() const { return arguments_; }

}

#endif /* FINDMETHODVISSTYLE_H_ */
