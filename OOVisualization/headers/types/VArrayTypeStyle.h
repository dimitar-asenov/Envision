/***********************************************************************************************************************
 * VArrayTypeStyle.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VARRAYTYPESTYLE_H_
#define VARRAYTYPESTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API VArrayTypeStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle container_;
		Visualization::SymbolStyle symbol_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& container() const;
		const Visualization::SymbolStyle& symbol() const;

		static VArrayTypeStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& VArrayTypeStyle::container() const { return container_; }
inline const Visualization::SymbolStyle& VArrayTypeStyle::symbol() const { return symbol_; }

}

#endif /* VARRAYTYPESTYLE_H_ */
