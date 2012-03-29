/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

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
		virtual void load(StyleLoader& sl);

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
