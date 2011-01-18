/***********************************************************************************************************************
 * PanelBorderLayout.h
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PANELBORDERLAYOUT_H_
#define PANELBORDERLAYOUT_H_

#include "../visualizationbase_api.h"

#include "Layout.h"
#include "PanelLayout.h"
#include "PanelBorderLayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API PanelBorderLayout: public Layout
{
	ITEM_COMMON(PanelBorderLayout, Layout)

	private:
		PanelLayout* top_;
		PanelLayout* left_;
		PanelLayout* bottom_;
		PanelLayout* right_;
		Item* content_;

		void setPanel(bool enable, PanelLayout*& panel, const PanelLayoutStyle& style);

	public:
		PanelBorderLayout(Item* parent, const PanelBorderLayoutStyle* style = Styles::layout<PanelBorderLayout>("default"));
		~PanelBorderLayout();

		void setTop(bool enable = true);
		void setLeft(bool enable = true);
		void setBottom(bool enable = true);
		void setRight(bool enable = true);
		void setContent(Item* content, bool deleteOldContent = true);

		PanelLayout* top();
		PanelLayout* left();
		PanelLayout* bottom();
		PanelLayout* right();
		template <class T> T* content();

		int getXOffsetForExternalShape() const;
		int getYOffsetForExternalShape() const;
		int getOutterWidthForExternalShape() const;
		int getOutterHeightForExternalShape() const;

		virtual void updateGeometry(int availableWidth, int availableHeight);
};

inline void PanelBorderLayout::setTop(bool enable) { setPanel(enable, top_, style()->topStyle()); };
inline void PanelBorderLayout::setLeft(bool enable) { setPanel(enable, left_, style()->leftStyle()); };
inline void PanelBorderLayout::setBottom(bool enable) { setPanel(enable, bottom_, style()->bottomStyle()); };
inline void PanelBorderLayout::setRight(bool enable) { setPanel(enable, right_, style()->rightStyle()); };

inline PanelLayout* PanelBorderLayout::top() { return top_; }
inline PanelLayout* PanelBorderLayout::left() { return left_; }
inline PanelLayout* PanelBorderLayout::bottom() { return bottom_; }
inline PanelLayout* PanelBorderLayout::right() { return right_; }
template <class T> inline T* PanelBorderLayout::content() { return content_; }

}

#endif /* PANELBORDERLAYOUT_H_ */
