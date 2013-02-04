/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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
		enum class Orientation : int { Horizontal, Vertical};
		enum class Alignment : int { Center, Left, Right, Top, Bottom};
		enum class Direction : int { LeftToRight, RightToLeft, TopToBottom, BottomToTop};

	private:
		int leftMargin_;
		int rightMargin_;
		int topMargin_;
		int bottomMargin_;

	public:
		LayoutStyle(int leftMargin = 0, int rightMargin = 0, int topMargin = 0, int bottomMargin = 0);
		virtual void load(StyleLoader& sl);

		int leftMargin() const;
		int rightMargin() const;
		int topMargin() const;
		int bottomMargin() const;

		void setMargins(int marginSize);
		void setMargins(int left, int right, int top, int bottom);
};

inline int LayoutStyle::leftMargin() const { return leftMargin_; }
inline int LayoutStyle::rightMargin() const { return rightMargin_; }
inline int LayoutStyle::topMargin() const { return topMargin_; }
inline int LayoutStyle::bottomMargin() const {return bottomMargin_; }

}

#endif /* LAYOUTSTYLE_H_ */
