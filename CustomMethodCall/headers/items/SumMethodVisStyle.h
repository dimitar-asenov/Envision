/***********************************************************************************************************************
 * SumMethodVisStyle.h
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SUMMETHODVISSTYLE_H_
#define SUMMETHODVISSTYLE_H_

#include "../custommethodcall_api.h"

#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace CustomMethodCall {

class CUSTOMMETHODCALL_API SumMethodVisStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::SymbolStyle name_;
		Visualization::SequentialLayoutStyle arguments_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::SymbolStyle& name() const;
		const Visualization::SequentialLayoutStyle& arguments() const;

		static SumMethodVisStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& SumMethodVisStyle::layout() const { return layout_; }
inline const Visualization::SymbolStyle& SumMethodVisStyle::name() const { return name_; }
inline const Visualization::SequentialLayoutStyle& SumMethodVisStyle::arguments() const { return arguments_; }

}

#endif /* SUMMETHODVISSTYLE_H_ */
