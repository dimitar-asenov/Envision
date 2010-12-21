/***********************************************************************************************************************
 * PanelBorderLayoutStyle.h
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PANELBORDERLAYOUTSTYLE_H_
#define PANELBORDERLAYOUTSTYLE_H_

#include "visualizationbase_api.h"

#include "LayoutStyle.h"
#include "PanelLayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API PanelBorderLayoutStyle : public LayoutStyle
{
	private:
		PanelLayoutStyle topStyle_;
		PanelLayoutStyle leftStyle_;
		PanelLayoutStyle bottomStyle_;
		PanelLayoutStyle rightStyle_;

		int leftInnerMargin_;
		int rightInnerMargin_;
		int topInnerMargin_;
		int bottomInnerMargin_;

	public:
		PanelBorderLayoutStyle();
		virtual void load();

		PanelLayoutStyle& topStyle();
		PanelLayoutStyle& leftStyle();
		PanelLayoutStyle& bottomStyle();
		PanelLayoutStyle& rightStyle();

		int leftInnerMargin() const;
		int rightInnerMargin() const;
		int topInnerMargin() const;
		int bottomInnerMargin() const;
		void setInnerMargins(int marginSize);
		void setInnerMargins(int left, int right, int top, int bottom);

		static PanelBorderLayoutStyle* getDefault();
};

inline PanelLayoutStyle& PanelBorderLayoutStyle::topStyle() { return topStyle_; }
inline PanelLayoutStyle& PanelBorderLayoutStyle::leftStyle() { return leftStyle_; }
inline PanelLayoutStyle& PanelBorderLayoutStyle::bottomStyle() { return bottomStyle_; }
inline PanelLayoutStyle& PanelBorderLayoutStyle::rightStyle() { return rightStyle_; }
inline int PanelBorderLayoutStyle::leftInnerMargin() const { return leftInnerMargin_; }
inline int PanelBorderLayoutStyle::rightInnerMargin() const { return rightInnerMargin_; }
inline int PanelBorderLayoutStyle::topInnerMargin() const { return topInnerMargin_; }
inline int PanelBorderLayoutStyle::bottomInnerMargin() const {return bottomInnerMargin_; }
}

#endif /* PANELBORDERLAYOUTSTYLE_H_ */
