/***********************************************************************************************************************
 * PanelBorderLayoutStyle.h
 *
 *  Created on: Dec 15, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PANELBORDERLAYOUTSTYLE_H_
#define PANELBORDERLAYOUTSTYLE_H_

#include "../visualizationbase_api.h"

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

		bool leftProtrusionFixed_;
		int leftProtrusion_;

		bool shapeOnlyOnContent_;

	public:
		PanelBorderLayoutStyle();
		virtual void load();

		const PanelLayoutStyle& topStyle() const;
		const PanelLayoutStyle& leftStyle() const;
		const PanelLayoutStyle& bottomStyle() const;
		const PanelLayoutStyle& rightStyle() const;

		int leftInnerMargin() const;
		int rightInnerMargin() const;
		int topInnerMargin() const;
		int bottomInnerMargin() const;
		void setInnerMargins(int marginSize);
		void setInnerMargins(int left, int right, int top, int bottom);

		bool isLeftProtrusionFixed() const;
		int leftProtrusion() const;

		bool shapeOnlyOnContent() const;

		static PanelBorderLayoutStyle* getDefault();
};

inline const PanelLayoutStyle& PanelBorderLayoutStyle::topStyle() const { return topStyle_; }
inline const PanelLayoutStyle& PanelBorderLayoutStyle::leftStyle() const { return leftStyle_; }
inline const PanelLayoutStyle& PanelBorderLayoutStyle::bottomStyle() const { return bottomStyle_; }
inline const PanelLayoutStyle& PanelBorderLayoutStyle::rightStyle() const { return rightStyle_; }
inline int PanelBorderLayoutStyle::leftInnerMargin() const { return leftInnerMargin_; }
inline int PanelBorderLayoutStyle::rightInnerMargin() const { return rightInnerMargin_; }
inline int PanelBorderLayoutStyle::topInnerMargin() const { return topInnerMargin_; }
inline int PanelBorderLayoutStyle::bottomInnerMargin() const {return bottomInnerMargin_; }

inline bool PanelBorderLayoutStyle::isLeftProtrusionFixed() const { return leftProtrusionFixed_; }
inline int PanelBorderLayoutStyle::leftProtrusion() const { return leftProtrusion_; }

inline bool PanelBorderLayoutStyle::shapeOnlyOnContent() const { return shapeOnlyOnContent_; }
}

#endif /* PANELBORDERLAYOUTSTYLE_H_ */
