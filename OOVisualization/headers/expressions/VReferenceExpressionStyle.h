/***********************************************************************************************************************
 * VReferenceExpressionStyle.h
 *
 *  Created on: Feb 8, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VREFERENCEEXPRESSIONSTYLE_H_
#define VREFERENCEEXPRESSIONSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VReferenceExpressionStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::TextStyle name_;
		Visualization::SymbolStyle separator_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::TextStyle& name() const;
		const Visualization::SymbolStyle& separator() const;

		static VReferenceExpressionStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VReferenceExpressionStyle::layout() const { return layout_; }
inline const Visualization::TextStyle& VReferenceExpressionStyle::name() const { return name_; }
inline const Visualization::SymbolStyle& VReferenceExpressionStyle::separator() const { return separator_; }

}

#endif /* VREFERENCEEXPRESSIONSTYLE_H_ */
