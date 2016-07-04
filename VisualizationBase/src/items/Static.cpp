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

#include "Static.h"
#include "../VisualizationException.h"
#include "../shapes/Shape.h"

namespace Visualization {

DEFINE_ITEM_COMMON(Static, "item")

QMap<QString, Static::staticItemConstructorType> Static::itemClasses_;
QMap<QString, Static::staticItemStyleConstructorType> Static::itemStyles_;

Static::Static(Item* parent, const StyleType *style) :
		Super{parent, style}, item_{nullptr}
{
	// The shape should only show for the contained static item.
	removeShape();
	if (style->zValue() != 0) setZValue(style->zValue());
}

Static::~Static()
{
	SAFE_DELETE_ITEM(item_);
}

bool Static::isEmpty() const
{
	return style()->isEmpty();
}

bool Static::sizeDependsOnParent() const
{
	return style()->alwaysStretchable() || (item_ && item_->sizeDependsOnParent());
}

void Static::updateGeometry(int availableWidth, int availableHeight)
{
	bool stretchUpdate = (availableWidth > 0 || availableHeight > 0);
	if (item_ && ( !stretchUpdate || item_->sizeDependsOnParent()) )
	{
		// Only update the item if it's stretchable or if this is the initial draw.
		Item::updateGeometry(item_, availableWidth, availableHeight);
		return;
	}

	// Either there is no item, or there is an item but it's not stretchable
	int currentWidth = item_ ? widthInLocal() : 0;
	int currentHeight = item_ ? heightInLocal() : 0;

	QSize newSize{ std::max(availableWidth, currentWidth), std::max(availableHeight, currentHeight)};

	if (hasShape())
	{
		getShape()->setOffset(0, 0);
		if (stretchUpdate) getShape()->setOuterSize(newSize.width(), newSize.height());
		else getShape()->setInnerSize(0, 0);
	}
	else setSize(newSize);
}

void Static::determineChildren()
{
	if (style()->isEmpty()) SAFE_DELETE_ITEM(item_);
	else
	{
		if (item_ && item_->style() != &style()->itemStyle())
			SAFE_DELETE_ITEM(item_);

		if (!item_)
		{
			if (itemClasses_.contains(style()->itemClass()))
				item_ = itemClasses_.value(style()->itemClass())(this, &style()->itemStyle());
			else throw VisualizationException{"Invalid item class specified for a Static item"};
		}

		item_->setStyle( &style()->itemStyle());
	}
}

ItemStyle* Static::constructStyle(const QString& itemClass)
{
	if (itemStyles_.contains(itemClass))
		return itemStyles_.value(itemClass)();
	else return nullptr;
}

}
