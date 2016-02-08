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
#include "FormElement.h"
#include "../items/Item.h"

namespace Visualization {

/**
 * Base class for an element wrapping an item.
 */
template <typename ParentType, typename ChildItemType = Item>
class ItemWrapperFormElement : public FormElement
{
	public: // Methods executable on element definition
		using ChildItem = ChildItemType* ParentType::*;

		ItemWrapperFormElement(ChildItem item);
		ItemWrapperFormElement() = delete;
		ItemWrapperFormElement(const ItemWrapperFormElement<ParentType, ChildItemType>&) = default;
		ItemWrapperFormElement<ParentType, ChildItemType>&
			operator=(const ItemWrapperFormElement<ParentType, ChildItemType>&) = delete;
		virtual ~ItemWrapperFormElement() {};

	public: // Methods executable when items need to be rendered
		virtual void computeSize(Item* item, int availableWidth, int availableHeight) override;
		virtual void setItemPositions(Item* item, int parentX, int parentY) override;
		virtual bool sizeDependsOnParent(const Item* item) const override;
		virtual QList<ItemRegion> regions(DeclarativeItemBase* item, int parentX, int parentY) override;
		bool elementOrChildHasFocus(Item* item) const override;
		bool isEmpty(const Item* item) const override;

	public: // Recursive item destruction
		virtual void destroyChildItems(Item* item,
				QList<const Item* const DeclarativeItemBase::*> handledChildren) override;

		virtual QList<const Item* const DeclarativeItemBase::*> allHandledChildPointers() override;

	protected:
		ChildItem item() const;

	private:

		// Do not forget to update the copy constructor if adding new members.
		ChildItem item_{};
};

template <typename ParentType, typename ChildItemType>
ItemWrapperFormElement<ParentType, ChildItemType>::ItemWrapperFormElement(ChildItem item)
: item_{item}
{}

template <typename ParentType, typename ChildItemType>
void ItemWrapperFormElement<ParentType, ChildItemType>::computeSize(Item* item, int availableWidth, int availableHeight)
{
	auto& childItem = (static_cast<ParentType*>(item))->*this->item();
	if (childItem)
	{
		int width = childItem->widthInParent() + leftMargin() + rightMargin();
		int height = childItem->heightInParent() + topMargin() + bottomMargin();
		if (availableWidth > width) width = availableWidth;
		if (availableHeight > height) height = availableHeight;
		if (availableWidth > 0 || availableHeight > 0)
			childItem->changeGeometry(width - leftMargin() - rightMargin(), height - topMargin() - bottomMargin());
		setSize(item, QSize{width, height});
	}
	else setSize(item, QSize{0, 0});
}

template <typename ParentType, typename ChildItemType>
void ItemWrapperFormElement<ParentType, ChildItemType>::setItemPositions(Item* item, int parentX, int parentY)
{
	auto& childItem = (static_cast<ParentType*>(item))->*this->item();
	if (childItem) childItem->setPos(parentX + x(item) + leftMargin(), parentY + y(item) + topMargin());
}

template <typename ParentType, typename ChildItemType>
bool ItemWrapperFormElement<ParentType, ChildItemType>::sizeDependsOnParent(const Item* item) const
{
	auto& childItem = (static_cast<const ParentType*>(item))->*this->item();
	if (childItem) return childItem->sizeDependsOnParent();
	else return false;
}

template <typename ParentType, typename ChildItemType>
QList<ItemRegion> ItemWrapperFormElement<ParentType, ChildItemType>::regions(DeclarativeItemBase* item, int, int)
{
	auto& childItem = (static_cast<const ParentType*>(item))->*this->item();

	if (childItem) {
		QRect rect = QRect{QPoint{0, 0}, childItem->sizeInParent().toSize()};
		rect.translate(childItem->pos().toPoint());
		QList<ItemRegion> regs;
		regs.append(ItemRegion{rect});
		regs.last().setItem(childItem);
		return regs;
	}
	else return QList<ItemRegion>();
}

template <typename ParentType, typename ChildItemType>
void ItemWrapperFormElement<ParentType, ChildItemType>::destroyChildItems(Item* item,
		QList<const Item* const DeclarativeItemBase::*> handledChildren)
{
	FormElement::destroyChildItems(item, handledChildren);

	auto childPointer = reinterpret_cast<const Item* const DeclarativeItemBase::*> (this->item());
	if (! handledChildren.contains(childPointer))
	{
		auto& childItem = (static_cast<ParentType*>(item))->*this->item();
		SAFE_DELETE_ITEM(childItem);
	}
}

template <typename ParentType, typename ChildItemType>
QList<const Item* const DeclarativeItemBase::*>
ItemWrapperFormElement<ParentType, ChildItemType>::allHandledChildPointers()
{
	return {reinterpret_cast<const Item* const DeclarativeItemBase::*> (this->item())};
}

template <typename ParentType, typename ChildItemType>
typename ItemWrapperFormElement<ParentType, ChildItemType>::ChildItem
ItemWrapperFormElement<ParentType, ChildItemType>::item() const
{
	return item_;
}

template <typename ParentType, typename ChildItemType>
bool ItemWrapperFormElement<ParentType, ChildItemType>::elementOrChildHasFocus(Item* item) const
{
	if (FormElement::elementOrChildHasFocus(item))
		return true;
	auto& childItem = (static_cast<ParentType*>(item))->*this->item();
	if (childItem) return childItem->itemOrChildHasFocus();
	else return false;
}

template <typename ParentType, typename ChildItemType>
bool ItemWrapperFormElement<ParentType, ChildItemType>::isEmpty(const Item* item) const
{
	auto& childItem = (static_cast<const ParentType*>(item))->*this->item();
	if (childItem) return childItem->isEmpty();
	else return true;
}

}
