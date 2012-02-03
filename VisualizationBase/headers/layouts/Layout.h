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
 * Layout.h
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "../visualizationbase_api.h"

#include "LayoutStyle.h"
#include "LayoutRegion.h"
#include "../items/Item.h"
#include "../shapes/Shape.h"

namespace Visualization {

class VISUALIZATIONBASE_API Layout: public Item
{
	ITEM_COMMON(Layout)

	public:
		Layout(Item* parent, const StyleType* style);

		void setInnerSize(int width, int height);

		virtual void determineChildren();

		virtual QList<LayoutRegion> regions();

		virtual bool moveCursor(CursorMoveDirection dir, const QPoint& reference = QPoint());

	protected:
		int xOffset() const;
		int yOffset() const;

		void synchronizeItem(Item*& layoutItem, Item*& externalItem, Model::Node* node);
		template <class T> void synchronizeItem(Item*& layoutItem, T*& externalItem, bool present, const typename T::StyleType* style);
		template <class T> void synchronizeItem(Item*& layoutItem, T*& externalItem, typename T::NodeType* node, const typename T::StyleType* style);
};

template <class T> void Layout::synchronizeItem(Item*& layoutItem, T*& externalItem, bool present, const typename T::StyleType* style)
{
	// When refactoring this method have in mind that layoutItem might point to the same item as externalItem.


	if (externalItem != layoutItem)
	{
		SAFE_DELETE_ITEM(layoutItem);
		setUpdateNeeded();
	}

	if (externalItem && !present )
	{
		SAFE_DELETE_ITEM(externalItem);
		layoutItem = nullptr; // One of the safe deletes above deleted this item
		setUpdateNeeded();
	}

	if (!externalItem && present)
	{
		if (style) externalItem = new T(nullptr, style);
		else externalItem = new T(nullptr);

		externalItem->setParentItem(this);
		layoutItem = externalItem;
		setUpdateNeeded();
	}
}

template <class T> void Layout::synchronizeItem(Item*& layoutItem, T*& externalItem, typename T::NodeType* node, const typename T::StyleType* style)
{
	// When refactoring this method have in mind that layoutItem might point to the same item as externalItem.


	if (externalItem != layoutItem)
	{
		SAFE_DELETE_ITEM(layoutItem);
		setUpdateNeeded();
	}

	if (externalItem && externalItem->node() != node )
	{
		SAFE_DELETE_ITEM(externalItem);
		layoutItem = nullptr; // One of the safe deletes above deleted this item
		setUpdateNeeded();
	}

	if (!externalItem && node)
	{
		if (style) externalItem = new T(nullptr, node, style);
		else externalItem = new T(nullptr, node);

		externalItem->setParentItem(this);
		layoutItem = externalItem;
		setUpdateNeeded();
	}
}

}

#endif /* LAYOUT_H_ */
