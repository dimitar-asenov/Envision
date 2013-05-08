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

#pragma once

#include "../visualizationbase_api.h"

#include "Layout.h"
#include "PanelLayout.h"
#include "PanelBorderLayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API PanelBorderLayout: public Reflect<Layout>
{
	ITEM_COMMON(PanelBorderLayout)

	public:
		PanelBorderLayout(Item* parent, const StyleType* style = itemStyles().get());
		~PanelBorderLayout();

		void setTop(bool enable = true);
		void setLeft(bool enable = true);
		void setBottom(bool enable = true);
		void setRight(bool enable = true);
		void setContent(Item* content, bool deleteOldContent = true);

		void synchronizeContent(Item*& item, Model::Node* node);
		template <class T> void synchronizeContent(T*& item, bool present, const typename T::StyleType* style);
		template <class T> void synchronizeContent(T*& item, typename T::NodeType* node,
				const typename T::StyleType* style);

		PanelLayout* top();
		PanelLayout* left();
		PanelLayout* bottom();
		PanelLayout* right();
		template <class T> T* content();

		virtual bool isEmpty() const;

		virtual void updateGeometry(int availableWidth, int availableHeight);\

	private:
		PanelLayout* top_;
		PanelLayout* left_;
		PanelLayout* bottom_;
		PanelLayout* right_;
		Item* content_;

		void setPanel(bool enable, PanelLayout*& panel, const PanelLayoutStyle& style);
};

inline void PanelBorderLayout::setTop(bool enable) { setPanel(enable, top_, style()->topStyle()); }
inline void PanelBorderLayout::setLeft(bool enable) { setPanel(enable, left_, style()->leftStyle()); }
inline void PanelBorderLayout::setBottom(bool enable) { setPanel(enable, bottom_, style()->bottomStyle()); }
inline void PanelBorderLayout::setRight(bool enable) { setPanel(enable, right_, style()->rightStyle()); }

inline PanelLayout* PanelBorderLayout::top() { return top_; }
inline PanelLayout* PanelBorderLayout::left() { return left_; }
inline PanelLayout* PanelBorderLayout::bottom() { return bottom_; }
inline PanelLayout* PanelBorderLayout::right() { return right_; }
template <class T> inline T* PanelBorderLayout::content() { return content_; }

inline void PanelBorderLayout::synchronizeContent(Item*& item, Model::Node* node)
{
	Layout::synchronizeItem(content_, item, node);
}
template <class T>
inline void PanelBorderLayout::synchronizeContent(T*& item, bool present, const typename T::StyleType* style)
{
	Layout::synchronizeItem(content_, item, present, style);
}
template <class T>
inline void PanelBorderLayout::synchronizeContent(T*& item, typename T::NodeType* node,
		const typename T::StyleType* style)
{
	Layout::synchronizeItem(content_, item, node, style);
}

}
