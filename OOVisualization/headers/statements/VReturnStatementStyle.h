/***********************************************************************************************************************
 * VReturnStatementStyle.h
 *
 *  Created on: Feb 15, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VRETURNSTATEMENTSTYLE_H_
#define VRETURNSTATEMENTSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/items/VListStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VReturnStatementStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle container_;
		Visualization::SymbolStyle symbol_;
		Visualization::VListStyle values_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& container() const;
		const Visualization::SymbolStyle& symbol() const;
		const Visualization::VListStyle& values() const;

		static VReturnStatementStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VReturnStatementStyle::container() const { return container_; }
inline const Visualization::SymbolStyle& VReturnStatementStyle::symbol() const { return symbol_; }
inline const Visualization::VListStyle& VReturnStatementStyle::values() const { return values_; }
}

#endif /* VRETURNSTATEMENTSTYLE_H_ */
