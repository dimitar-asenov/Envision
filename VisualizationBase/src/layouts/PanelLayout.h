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
#include "PanelLayoutStyle.h"
#include "SequentialLayout.h"

namespace Visualization {

class VISUALIZATIONBASE_API PanelLayout: public Reflect<Layout>
{
	ITEM_COMMON(PanelLayout)

	public:
		PanelLayout(Item* parent, const StyleType* style = itemStyles().get());
		~PanelLayout();

		void setFirst(Item* item, bool deleteOldItem = true);
		void setMiddle(Item* item, bool deleteOldItem = true);
		void setLast(Item* item, bool deleteOldItem = true);

		template <class T> T* first();
		template <class T> T* middle();
		template <class T> T* last();

		void synchronizeFirst(Item*& item, Model::Node* node);
		template <class T> void synchronizeFirst(T*& item, bool present, const typename T::StyleType* style);
		template <class T> void synchronizeFirst(T*& item, typename T::NodeType* node, const typename T::StyleType* style);

		void synchronizeMiddle(Item*& item, Model::Node* node);
		template <class T> void synchronizeMiddle(T*& item, bool present, const typename T::StyleType* style);
		template <class T> void synchronizeMiddle(T*& item, typename T::NodeType* node, const typename T::StyleType* style);

		void synchronizeLast(Item*& item, Model::Node* node);
		template <class T> void synchronizeLast(T*& item, bool present, const typename T::StyleType* style);
		template <class T> void synchronizeLast(T*& item, typename T::NodeType* node, const typename T::StyleType* style);

		virtual bool isEmpty() const;

		virtual bool sizeDependsOnParent() const;
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Item* first_;
		Item* middle_;
		Item* last_;

		void setItem(Item* item, Item*& position, bool deleteOldItem);
};

inline void PanelLayout::setFirst(Item* item, bool deleteOldItem) { setItem(item, first_, deleteOldItem); }
inline void PanelLayout::setMiddle(Item* item, bool deleteOldItem) { setItem(item, middle_, deleteOldItem); }
inline void PanelLayout::setLast(Item* item, bool deleteOldItem) { setItem(item, last_, deleteOldItem); }

template <class T> inline T* PanelLayout::first() { return static_cast<T*> (first_); }
template <class T> inline T* PanelLayout::middle() { return static_cast<T*> (middle_); }
template <class T> inline T* PanelLayout::last() { return static_cast<T*> (last_); }

inline void PanelLayout::synchronizeFirst(Item*& item, Model::Node* node)
{
	Layout::synchronizeItem(first_, item, node);
}
template <class T> inline void PanelLayout::synchronizeFirst(T*& item, bool present, const typename T::StyleType* style)
{
	Layout::synchronizeItem(first_, item, present, style);
}
template <class T> inline void PanelLayout::synchronizeFirst(T*& item, typename T::NodeType* node, const typename T::StyleType* style)
{
	Layout::synchronizeItem(first_, item, node, style);
}

inline void PanelLayout::synchronizeMiddle(Item*& item, Model::Node* node)
{
	Layout::synchronizeItem(middle_, item, node);
}
template <class T> inline void PanelLayout::synchronizeMiddle(T*& item, bool present, const typename T::StyleType* style)
{
	Layout::synchronizeItem(middle_, item, present, style);
}
template <class T> inline void PanelLayout::synchronizeMiddle(T*& item, typename T::NodeType* node, const typename T::StyleType* style)
{
	Layout::synchronizeItem(middle_, item, node, style);
}

inline void PanelLayout::synchronizeLast(Item*& item, Model::Node* node)
{
	Layout::synchronizeItem(last_, item, node);
}
template <class T> inline void PanelLayout::synchronizeLast(T*& item, bool present, const typename T::StyleType* style)
{
	Layout::synchronizeItem(last_, item, present, style);
}
template <class T> inline void PanelLayout::synchronizeLast(T*& item, typename T::NodeType* node, const typename T::StyleType* style)
{
	Layout::synchronizeItem(last_, item, node, style);
}

}
