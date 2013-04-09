/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "Element.h"
#include "../items/Item.h"
#include "../cursor/LayoutCursor.h"

namespace Visualization {

Element::~Element()
{
	for (auto i : elementCache_.keys())
		clearCache(i);
	for (auto e : children())
	{
		removeChild(e);
		SAFE_DELETE(e);
	}
}

/**
 * Gets the element property cache of this element for a specific item.
 * @param item The item for which to get the cache
 * @return The element cache for the specified item
 */
ElementCache& Element::getCache(const Item* item) const
{
	if (!elementCache_.contains(item))
		elementCache_.insert(item, new ElementCache());
	return *elementCache_.value(item);
}

/**
 * Clears the element property cache of this element for a specific item.
 * @param item The item for which to clear the cache
 */
void Element::clearCache(const Item* item)
{
	if (elementCache_.contains(item))
	{
		auto e = elementCache_.value(item);
		elementCache_.remove(item);
		SAFE_DELETE(e);
	}
}

/**
 * This method is called while rendering the item.
 *
 * The method is recursively called to set the position of the items correctly relative to its parent Item.
 * @param item The parent Item
 * @param parentX The position on the x-axis of the parent Element relative to the parent Item
 * @param parentY The position on the y-axis of the parent Element relative to the parent Item
 */
void Element::setItemPositions(Item* item, int parentX, int parentY)
{
	for(Element* element : children())
		element->setItemPositions(item, parentX + x(item), parentY + y(item));
}

/**
 * This method is called while rendering the item.
 *
 * The method is recursively called to synchronize the elements with the item. This means if any nodes or child items
 * changed, this needs to be reflected by the item wrappers and the sequential layout.
 * @param item The parent Item
 */
void Element::synchronizeWithItem(Item* item)
{
	for(Element* element : children())
		if (element != nullptr) element->synchronizeWithItem(item);
}

/**
 * This method is called while rendering the item.
 *
 * Method recursively called on all elements to recursively destroy all the information they might have about a specific
 * item.
 * @param item The item for which to destroy the information
 */
void Element::destroyChildItems(Item* item)
{
	clearCache(item);
}

/**
 * This method is called while rendering the item.
 *
 * Recursively determines if this element or any of its children has the focus.
 * @param item The item for which to decide
 * @return Whether this element or any of its children have focus, relative to the specified item
 */
bool Element::elementOrChildHasFocus(Item* item) const
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

}


