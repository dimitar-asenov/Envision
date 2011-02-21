/***********************************************************************************************************************
 * OperatorStyle.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef OPERATORSTYLE_H_
#define OPERATORSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/SymbolStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API OperatorStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::SymbolStyle preSymbol_;
		Visualization::SymbolStyle inSymbol_;
		Visualization::SymbolStyle postSymbol_;

	public:
		void load();

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::SymbolStyle& preSymbol() const;
		const Visualization::SymbolStyle& inSymbol() const;
		const Visualization::SymbolStyle& postSymbol() const;

		static OperatorStyle* getDefault();
};

inline const Visualization::SequentialLayoutStyle& OperatorStyle::layout() const { return layout_; }
inline const Visualization::SymbolStyle& OperatorStyle::preSymbol() const { return preSymbol_; }
inline const Visualization::SymbolStyle& OperatorStyle::inSymbol() const { return inSymbol_; }
inline const Visualization::SymbolStyle& OperatorStyle::postSymbol() const { return postSymbol_; }

class OOVISUALIZATION_API OperatorSequenceStyle : public Visualization::ItemStyle
{
	private:
		QVector<OperatorStyle> seq_;

	public:
		void load();

		const OperatorStyle& op(int i) const;

		static OperatorSequenceStyle* getDefault();
};

inline const OperatorStyle& OperatorSequenceStyle::op(int i) const { return seq_.at(i); }

}

#endif /* OPERATORSTYLE_H_ */
