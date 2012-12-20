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

#ifndef VisualizationBase_ELEMENT_H_
#define VisualizationBase_ELEMENT_H_

#include "../visualizationbase_api.h"

namespace Visualization {

class Item;

class VISUALIZATIONBASE_API Element
{
	public: // Methods executable on element definition
		virtual ~Element();
		// set margins
		void setMargins(int left, int top, int right, int bottom);
		void setMargins(int margin);
		void setTopMargin(int top);
		void setBottomMargin(int bottom);
		void setLeftMargin(int left);
		void setRightMargin(int right);

	public: // Methods executable when items need to be rendered
		virtual void synchronizeWithItem(Item* item) = 0;
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) = 0;
		QSize size() const;

	protected:
		void setSize(const QSize& size);
		int topMargin();
		int bottomMargin();
		int leftMargin();
		int rightMargin();

	private:
		int marginTop_{};
		int marginBottom_{};
		int marginLeft_{};
		int marginRight_{};
		QSize size_{};
};

inline void Element::setMargins(int left, int top, int right, int bottom)
{
	marginLeft_ = left;
	marginTop_ = top;
	marginRight_ = right;
	marginBottom_ = bottom;
}
inline void Element::setMargins(int margin)
{
	marginLeft_ = margin;
	marginTop_ = margin;
	marginRight_ = margin;
	marginBottom_ = margin;
}
inline void Element::setTopMargin(int top) {marginTop_ = top;}
inline void Element::setBottomMargin(int bottom) {marginBottom_ = bottom;}
inline void Element::setLeftMargin(int left) {marginLeft_ = left;}
inline void Element::setRightMargin(int right) {marginRight_ = right;}

inline QSize Element::size() const {return size_;}
inline void Element::setSize(const QSize& size) {size_ = size;}

inline int Element::topMargin() {return marginTop_;}
inline int Element::bottomMargin() {return marginBottom_;}
inline int Element::leftMargin() {return marginLeft_;}
inline int Element::rightMargin() {return marginRight_;}

}


#endif /* VisualizationBase_ELEMENT_H_ */
