/***********************************************************************************************************************
 * SequentialLayoutStyle.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SEQUENTIALLAYOUTSTYLE_H_
#define SEQUENTIALLAYOUTSTYLE_H_

#include "visualizationbase_api.h"

namespace Visualization {

class VISUALIZATIONBASE_API SequentialLayoutStyle
{
	public:
		enum SequntialLayoutDirection { LeftToRight, RightToLeft, TopToBottom, BottomToTop};
		enum SequntialLayoutAlignment { CenterAlignment, LeftAlignment, RightAlignment, TopAlignment, BottomAlignment};

	private:
		SequntialLayoutDirection direction_;
		SequntialLayoutAlignment alignment_;
		int spaceBetweenElements_;
		int leftMargin_;
		int rightMargin_;
		int topMargin_;
		int bottomMargin_;

	public:
		SequentialLayoutStyle();

		SequntialLayoutDirection direction() const;
		SequntialLayoutAlignment alignment() const;
		int spaceBetweenElements() const;
		int leftMargin() const;
		int rightMargin() const;
		int topMargin() const;
		int bottomMargin() const;

		void setDirection(SequntialLayoutDirection direction);
		void setAlignment(SequntialLayoutAlignment alignment);
		void setSpaceBetweenElements(int space);
		void setMargins(int marginSize);
		void setMargins(int left, int right, int top, int bottom);

		static SequentialLayoutStyle* getDefault();
};

}

#endif /* SEQUENTIALLAYOUTSTYLE_H_ */
