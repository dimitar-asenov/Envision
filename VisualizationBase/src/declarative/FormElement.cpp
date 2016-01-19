/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "FormElement.h"
#include "../items/Item.h"
#include "../cursor/LayoutCursor.h"
#include "DeclarativeItemBase.h"

namespace Visualization {

FormElement::~FormElement()
{
	for (auto i : elementCache_.keys())
		clearCache(i);
	for (auto e : children())
	{
		removeChild(e);
		SAFE_DELETE(e);
	}
}


FormElement::FormElement(const FormElement& other) : marginTop_{other.marginTop_}, marginBottom_{other.marginBottom_},
	marginLeft_{other.marginLeft_}, marginRight_{other.marginRight_}
{}

ElementCache& FormElement::getCache(const Item* item) const
{
	if (!elementCache_.contains(item))
		elementCache_.insert(item, new ElementCache{});
	return *elementCache_.value(item);
}

void FormElement::clearCache(const Item* item)
{
	if (elementCache_.contains(item))
	{
		auto e = elementCache_.value(item);
		elementCache_.remove(item);
		SAFE_DELETE(e);
	}
}

QList<FormElement*> FormElement::shapeElements()
{
	auto elements = QList<FormElement*>();
	for (auto c : children())
		elements.append(c->shapeElements());
	return elements;
}

void FormElement::setItemPositions(Item* item, int parentX, int parentY)
{
	for (FormElement* element : children())
		element->setItemPositions(item, parentX + x(item), parentY + y(item));
}

QList<ItemRegion> FormElement::regions(DeclarativeItemBase* item, int parentX, int parentY)
{
	QList<ItemRegion> allRegions;
	for (auto element : children())
		allRegions.append(element->regions(item, x(item) + parentX, y(item) + parentY));
	return allRegions;
}

void FormElement::synchronizeWithItem(Item* item)
{
	for (FormElement* element : children())
		if (element != nullptr) element->synchronizeWithItem(item);
}

void FormElement::destroyChildItems(Item* item, QList<const Item* const DeclarativeItemBase::*> handledChildren)
{
	clearCache(item);
	for (FormElement* element : children()) element->destroyChildItems(item, handledChildren);
}

QList<const Item* const DeclarativeItemBase::*> FormElement::allHandledChildPointers()
{
	QList<const Item* const DeclarativeItemBase::*> ret;
	for (FormElement* element : children()) ret << element->allHandledChildPointers();

	return ret;
}

bool FormElement::isEmpty(const Item* item) const
{
	for (auto e : children())
		if (!e->isEmpty(item))
			return false;
	return true;
}

bool FormElement::elementOrChildHasFocus(Item* item) const
{
	auto cursor = item->scene()->mainCursor();
	if (cursor && cursor->owner() == item)
	{
		auto layoutCursor = dynamic_cast<LayoutCursor*> (cursor);
		if (layoutCursor)
			if (layoutCursor->ownerElement() == this)
				return true;
	}
	for (auto child : children())
	{
		bool ret = child->elementOrChildHasFocus(item);
		if (ret) return true;
	}

	return false;
}

void FormElement::addChild(FormElement* child)
{
	Q_ASSERT(child);
	Q_ASSERT(!child->parent());
	Q_ASSERT(!child->isFormRoot());
	Q_ASSERT(!children_.contains(child));
	children_.append(child);
	child->parent_ = this;
}

void FormElement::removeChild(FormElement* child)
{
	if (child)
		for (int i=0; i<children_.size(); ++i)
			if (child == children_[i]) children_.remove(i);
}

FormElement* FormElement::setFormRoot()
{
	Q_ASSERT(!parent_);
	isFormRoot_ = true;
	return this;
}

FormElement* FormElement::cloneIfAlreadyUsed()
{
	return ( isFormRoot() || parent() ) ? clone() : this;
}

}
