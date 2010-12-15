/***********************************************************************************************************************
 * PanelLayoutStyle.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PANELLAYOUTSTYLE_H_
#define PANELLAYOUTSTYLE_H_

#include "visualizationbase_api.h"

#include "SequentialLayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API PanelLayoutStyle
{
	public:
		enum PanelLayoutOrientation { HorizontalOrientation, VerticalOrientation};
		enum PanelLayoutAlignment { CenterAlignment, LeftAlignment, RightAlignment, TopAlignment, BottomAlignment};

	private:

		PanelLayoutOrientation orientation_;
		PanelLayoutAlignment alignment_;
		int spaceBetweenElements_;
		int leftMargin_;
		int rightMargin_;
		int topMargin_;
		int bottomMargin_;

	public:
		PanelLayoutStyle();

		PanelLayoutOrientation orientation() const;
		PanelLayoutAlignment alignment() const;
		int spaceBetweenElements() const;
		int leftMargin() const;
		int rightMargin() const;
		int topMargin() const;
		int bottomMargin() const;

		void setOrientation(PanelLayoutOrientation orientation);
		void setAlignment(PanelLayoutAlignment alignment);
		void setSpaceBetweenElements(int space);
		void setMargins(int marginSize);
		void setMargins(int left, int right, int top, int bottom);

		static PanelLayoutStyle* getDefault();
};

inline PanelLayoutStyle::PanelLayoutOrientation PanelLayoutStyle::orientation() const { return orientation_; }
inline PanelLayoutStyle::PanelLayoutAlignment PanelLayoutStyle::alignment() const {	return alignment_; }
inline int PanelLayoutStyle::spaceBetweenElements() const {	return spaceBetweenElements_; }
inline int PanelLayoutStyle::leftMargin() const { return leftMargin_; }
inline int PanelLayoutStyle::rightMargin() const {	return rightMargin_; }
inline int PanelLayoutStyle::topMargin() const { return topMargin_; }
inline int PanelLayoutStyle::bottomMargin() const {	return bottomMargin_; }

}

#endif /* PANELLAYOUTSTYLE_H_ */
