/***********************************************************************************************************************
 * VVariableDeclarationStyle.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VVARIABLEDECLARATIONSTYLE_H_
#define VVARIABLEDECLARATIONSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/items/StaticStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VVariableDeclarationStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::TextStyle name_;
		Visualization::StaticStyle assignmentSymbol_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::TextStyle& name() const;
		const Visualization::StaticStyle& assignmentSymbol() const;
};

inline const Visualization::SequentialLayoutStyle& VVariableDeclarationStyle::layout() const { return layout_; }
inline const Visualization::TextStyle& VVariableDeclarationStyle::name() const { return name_; }
inline const Visualization::StaticStyle& VVariableDeclarationStyle::assignmentSymbol() const { return assignmentSymbol_; }

}

#endif /* VVARIABLEDECLARATIONSTYLE_H_ */
