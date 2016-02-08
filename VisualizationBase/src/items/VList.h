/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "ItemWithNode.h"
#include "../declarative/DeclarativeItem.h"
#include "../declarative/SequentialLayoutFormElement.h"
#include "VListStyle.h"
#include "ModelBase/src/nodes/List.h"

namespace Visualization {

class Static;

class VISUALIZATIONBASE_API VList: public Super<ItemWithNode<VList, DeclarativeItem<VList>, Model::List>>
{
	ITEM_COMMON(VList)

	public:
		VList(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());

		int length() const;
		template <typename T> T* itemAt(int itemIndex) const;
		template <typename T> T* itemAtNodeIndex(int nodeIndex) const;
		int focusedItemIndex() const;
		int focusedNodeIndex() const;
		int rangeBegin() const;
		int rangeEnd() const;
		bool showsPartialRange() const;
		int indexOfChildOrSubChild(const Item* item) const;

		bool suppressDefaultRemovalHandler() const;
		void setSuppressDefaultRemovalHandler(bool suppress);
		void setSuppressDefaultCopyPasteHandler(bool suppress);

		bool isShowingEmptyTip();

		static void initializeForms();
		virtual int determineForm() override;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
		virtual bool moveCursor(CursorMoveDirection dir = MoveDefault, QPoint reference = {},
										CursorMoveOptions options = None) override;
		virtual QList<ItemRegion> regions() override;

		virtual bool ignoresCopyAndPaste() override;

	protected:
		/**
		 * Called during determineChildren() in order to determine which nodes should be shown.
		 *
		 * The default implementation sets the range to cover all nodes. Override this in a derived class and call
		 * setRange() in order to control the range of displayed nodes.
		 */
		virtual void determineRange();
		/**
		 * Sets the range of nodes that should be displayed by this list visualization to start with the node at location
		 * \a begin and end before the node at location \a end.
		 *
		 * VList can display whole lists or a continuous subpart of a list. Use this method to set what part of the nodes
		 * should be set.
		 */
		void setRange(int begin, int end);

		virtual void updateGeometry(int availableWidth, int availableHeight) override;

	private:
		bool suppressDefaultRemovalHandler_{};
		bool suppressDefaultCopyPasteHandler_{};
		int rangeBegin_{};
		int rangeEnd_{};
		Visualization::Static* emptyTip_{};
};

inline int VList::length() const
{
	return currentFormIndex() < 2 ? static_cast<SequentialLayoutFormElement*>(currentForm())->length(this) : 0;
}
inline int VList::focusedItemIndex() const
{
	return static_cast<SequentialLayoutFormElement*>(currentForm())->focusedElementIndex(this);
}
inline int VList::focusedNodeIndex() const { return focusedItemIndex() + rangeBegin_; }
template <typename T> inline T* VList::itemAt(int itemIndex) const
{
		return static_cast<SequentialLayoutFormElement*>(currentForm())->itemAt<T>(this, itemIndex);
}
template <typename T> inline T* VList::itemAtNodeIndex(int nodeIndex) const
{
		Q_ASSERT(rangeBegin_ <= nodeIndex);
		Q_ASSERT(nodeIndex  < rangeEnd_);
		return itemAt<T>(nodeIndex - rangeBegin_);
}

inline bool VList::suppressDefaultRemovalHandler() const { return suppressDefaultRemovalHandler_; }
inline void VList::setSuppressDefaultRemovalHandler(bool suppress) { suppressDefaultRemovalHandler_ = suppress; }
inline void VList::setSuppressDefaultCopyPasteHandler(bool suppress) {suppressDefaultCopyPasteHandler_ = suppress; }

inline int VList::rangeBegin() const { return rangeBegin_; }
inline int VList::rangeEnd() const { return rangeEnd_; }
inline bool VList::showsPartialRange() const { return rangeBegin_ != 0 || rangeEnd_ != node()->size();}
inline bool VList::isShowingEmptyTip() { return currentFormIndex() == 2;}

}
