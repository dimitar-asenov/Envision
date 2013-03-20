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

#ifndef VisualizationBase_SEQUENTIALLAYOUTELEMENT_H_
#define VisualizationBase_SEQUENTIALLAYOUTELEMENT_H_

#include "../visualizationbase_api.h"
#include "LayoutElement.h"
#include "../layouts/LayoutStyle.h"

namespace Model {
class Node;
class List;
}

namespace Visualization {

class Element;

class VISUALIZATIONBASE_API SequentialLayoutElement : public LayoutElement
{
		FLUENT_ELEMENT_INTERFACE(SequentialLayoutElement);

	public:
		using ListNodeGetterFunction = std::function<Model::List*()>;
		using ListOfNodesGetterFunction = std::function<QList<Model::Node*>()>;
		using ListOfItemsGetterFunction = std::function<QList<Item*>()>;

		// Functions executable on element definition
		SequentialLayoutElement();
		virtual ~SequentialLayoutElement();

		SequentialLayoutElement* setListNode(ListNodeGetterFunction listNodeGetter);
		SequentialLayoutElement* setListOfNodes(ListOfNodesGetterFunction nodeListGetter);
		SequentialLayoutElement* setListOfItems(ListOfItemsGetterFunction itemListGetter);
		SequentialLayoutElement* setSpaceBetweenElements(int space);
		SequentialLayoutElement* setSpaceBetweenElements(std::function<int()> spaceBetweenElementsGetter);
		SequentialLayoutElement* setOrientation(Qt::Orientation o);
		SequentialLayoutElement* setHorizontal();
		SequentialLayoutElement* setVertical();
		SequentialLayoutElement* setAlignment(LayoutStyle::Alignment a);
		SequentialLayoutElement* setForward(bool forward);
		SequentialLayoutElement* setMinWidth(int minWidth);
		SequentialLayoutElement* setMinHeight(int minHeight);

		// Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual void setItemPositions(Item* item, int parentX, int parentY) override;
		virtual void synchronizeWithItem(Item* item) override;
		virtual bool sizeDependsOnParent(const Item* item) const override;

		// Recursive item destruction
		virtual void destroyChildItems(Item* item) override;

	private:
		ListNodeGetterFunction listNodeGetter_{};
		ListOfNodesGetterFunction nodeListGetter_{};
		ListOfItemsGetterFunction itemListGetter_{};

		std::function<int()> spaceBetweenElementsGetter_;

		int defaultSpaceBetweenElements_{0};
		Qt::Orientation orientation_{Qt::Horizontal};
		LayoutStyle::Alignment alignment_{LayoutStyle::Alignment::Bottom};
		bool forward_{true};
		int minWidth_{};
		int minHeight_{};

		mutable QHash<const Item*, QList<Item*>*> itemListMap_{};

		QList<Item*>& listForItem(const Item* item) const;
		int spaceBetweenElements();
		void synchronizeWithNodes(Item* item, const QList<Model::Node*>& nodes);
		void synchronizeWithItems(Item* item, const QList<Item*>& items);
		void swap(Item* item, int i, int j);
};

inline SequentialLayoutElement* SequentialLayoutElement::setListNode(ListNodeGetterFunction listNodeGetter)
{
	Q_ASSERT(!nodeListGetter_ && !itemListGetter_);
	listNodeGetter_ = listNodeGetter;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setListOfNodes(ListOfNodesGetterFunction nodeListGetter)
{
	Q_ASSERT(!listNodeGetter_ && !itemListGetter_);
	nodeListGetter_ = nodeListGetter;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setListOfItems(ListOfItemsGetterFunction itemListGetter)
{
	Q_ASSERT(!nodeListGetter_ && !listNodeGetter_);
	itemListGetter_ = itemListGetter;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setSpaceBetweenElements(int space)
{
	defaultSpaceBetweenElements_ = space;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setSpaceBetweenElements(
		std::function<int()> spaceBetweenElementsGetter)
{
	spaceBetweenElementsGetter_ = spaceBetweenElementsGetter;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setOrientation(Qt::Orientation o)
{
	orientation_ = o;
	if (orientation_ == Qt::Horizontal)
		alignment_ = LayoutStyle::Alignment::Bottom;
	else
		alignment_ = LayoutStyle::Alignment::Left;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setHorizontal()
{
	orientation_ = Qt::Horizontal;
	alignment_ = LayoutStyle::Alignment::Bottom;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setVertical()
{
	orientation_ = Qt::Vertical;
	alignment_ = LayoutStyle::Alignment::Left;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setAlignment(LayoutStyle::Alignment a)
{
	alignment_ = a;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setForward(bool forward)
{
	forward_ = forward;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setMinWidth(int minWidht)
{
	minWidth_ = minWidht;
	return this;
}
inline SequentialLayoutElement* SequentialLayoutElement::setMinHeight(int minHeight)
{
	minHeight_ = minHeight;
	return this;
}

}

#endif /* VisualizationBase_SEQUENTIALLAYOUTELEMENT_H_ */
