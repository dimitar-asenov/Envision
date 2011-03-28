/***********************************************************************************************************************
 * SequentialLayoutStyle.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SEQUENTIALLAYOUTSTYLE_H_
#define SEQUENTIALLAYOUTSTYLE_H_

#include "../visualizationbase_api.h"

#include "LayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API SequentialLayoutStyle : public LayoutStyle
{
	private:
		Direction direction_;
		Alignment alignment_;
		int spaceBetweenElements_;

	public:
		SequentialLayoutStyle();
		virtual void load(StyleLoader& sl);

		Direction direction() const;
		Alignment alignment() const;
		int spaceBetweenElements() const;

		void setDirection(Direction direction);
		void setAlignment(Alignment alignment);
		void setSpaceBetweenElements(int space);
};

inline LayoutStyle::Direction SequentialLayoutStyle::direction() const { return direction_; }
inline LayoutStyle::Alignment SequentialLayoutStyle::alignment() const {	return alignment_; }
inline int SequentialLayoutStyle::spaceBetweenElements() const {	return spaceBetweenElements_; }

}

#endif /* SEQUENTIALLAYOUTSTYLE_H_ */
