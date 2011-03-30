/***********************************************************************************************************************
 * OperatorStyle.h
 *
 *  Created on: Feb 14, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef OPERATORSTYLE_H_
#define OPERATORSTYLE_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/StaticStyle.h"
#include "VisualizationBase/headers/layouts/SequentialLayout.h"

namespace OOVisualization {

class OOVISUALIZATION_API OperatorStyle : public Visualization::ItemStyle
{
	private:
		Visualization::SequentialLayoutStyle layout_;
		Visualization::StaticStyle preSymbol_;
		Visualization::StaticStyle inSymbol_;
		Visualization::StaticStyle postSymbol_;

	public:
		void load(Visualization::StyleLoader& sl);

		const Visualization::SequentialLayoutStyle& layout() const;
		const Visualization::StaticStyle& preSymbol() const;
		const Visualization::StaticStyle& inSymbol() const;
		const Visualization::StaticStyle& postSymbol() const;
};

inline const Visualization::SequentialLayoutStyle& OperatorStyle::layout() const { return layout_; }
inline const Visualization::StaticStyle& OperatorStyle::preSymbol() const { return preSymbol_; }
inline const Visualization::StaticStyle& OperatorStyle::inSymbol() const { return inSymbol_; }
inline const Visualization::StaticStyle& OperatorStyle::postSymbol() const { return postSymbol_; }

class OOVISUALIZATION_API OperatorSequenceStyle : public Visualization::ItemStyle
{
	private:
		QVector<OperatorStyle> seq_;

	public:
		void load(Visualization::StyleLoader& sl);

		const OperatorStyle& op(int i) const;
};

inline const OperatorStyle& OperatorSequenceStyle::op(int i) const { return seq_.at(i); }

}

#endif /* OPERATORSTYLE_H_ */
