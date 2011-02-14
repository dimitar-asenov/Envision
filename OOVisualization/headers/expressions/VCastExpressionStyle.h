/***********************************************************************************************************************
 * VCastExpressionStyle.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VCASTEXPRESSIONSTYLE_H_
#define VCASTEXPRESSIONSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VCastExpressionStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle container_;
		Visualization::SymbolStyle separator_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& container() const;
		const Visualization::SymbolStyle& separator() const;

		static VCastExpressionStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VCastExpressionStyle::container() const { return container_; }
inline const Visualization::SymbolStyle& VCastExpressionStyle::separator() const { return separator_; }

}

#endif /* VCASTEXPRESSIONSTYLE_H_ */
