/***********************************************************************************************************************
 * VMethodCallStatementStyle.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VMETHODCALLSTATEMENTSTYLE_H_
#define VMETHODCALLSTATEMENTSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"
#include "VisualizationBase/headers/items/VListStyle.h"

namespace OOVisualization {

class OOVISUALIZATION_API VMethodCallStatementStyle : public Visualization::ItemStyle
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

		static VMethodCallStatementStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VMethodCallStatementStyle::layout() const { return layout_; }
inline const Visualization::TextStyle& VMethodCallStatementStyle::name() const { return name_; }
inline const Visualization::SymbolStyle& VMethodCallStatementStyle::separator() const { return separator_; }
inline const Visualization::VListStyle& VMethodCallStatementStyle::arguments() const { return arguments_; }

}

#endif /* VMETHODCALLSTATEMENTSTYLE_H_ */
