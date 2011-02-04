/***********************************************************************************************************************
 * LayoutStyle.h
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LAYOUTSTYLE_H_
#define LAYOUTSTYLE_H_

#include "../visualizationbase_api.h"

#include "../items/ItemStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API LayoutStyle : public ItemStyle
{
	public:
		enum Orientation { HorizontalOrientation, VerticalOrientation};
		enum Alignment { CenterAlignment, LeftAlignment, RightAlignment, TopAlignment, BottomAlignment};
		enum Direction { LeftToRight, RightToLeft, TopToBottom, BottomToTop};

	private:
		int leftMargin_;
		int rightMargin_;
		int topMargin_;
		int bottomMargin_;
		bool drawShapeWhenEmpty_;

	public:
		LayoutStyle(int leftMargin = 0, int rightMargin = 0, int topMargin = 0, int bottomMargin = 0);
		virtual void load();

		int leftMargin() const;
		int rightMargin() const;
		int topMargin() const;
		int bottomMargin() const;
		bool drawShapeWhenEmpty() const;

		void setMargins(int marginSize);
		void setMargins(int left, int right, int top, int bottom);
};

inline int LayoutStyle::leftMargin() const { return leftMargin_; }
inline int LayoutStyle::rightMargin() const { return rightMargin_; }
inline int LayoutStyle::topMargin() const { return topMargin_; }
inline int LayoutStyle::bottomMargin() const {return bottomMargin_; }
inline bool LayoutStyle::drawShapeWhenEmpty() const {return drawShapeWhenEmpty_; }

}

#endif /* LAYOUTSTYLE_H_ */
