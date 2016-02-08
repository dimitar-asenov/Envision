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

#include "DeclarativeItemBase.h"
#include "FormElement.h"
#include "../shapes/Shape.h"
#include "../shapes/ShapeStyle.h"
#include "GridLayoutFormElement.h"
#include "../cursor/Cursor.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(DeclarativeItemBase, "item")

DeclarativeItemBase::DeclarativeItemBase(Item* parent, const StyleType* style) : Super{parent, style}
{}

DeclarativeItemBase::~DeclarativeItemBase()
{}

int DeclarativeItemBase::determineForm()
{
	return 0;
}

void DeclarativeItemBase::determineChildren()
{
	auto newFormIndex = determineForm();
	Q_ASSERT(newFormIndex >=0 && newFormIndex < forms().size());
	if (newFormIndex != currentFormIndex_)
	{
		if (currentFormIndex_ >= 0)
		{
			// Do not destroy child items on the first run
			auto mc = scene()->mainCursor();
			if (mc && isAncestorOf(mc->owner())) scene()->setMainCursor(nullptr);

			currentForm()->destroyChildItems(this, forms()[newFormIndex]->allHandledChildPointers());
		}
		currentFormIndex_ = newFormIndex;
	}
	currentForm()->synchronizeWithItem(this);
}

void DeclarativeItemBase::updateGeometry(int availableWidth, int availableHeight)
{
	FormElement* form = this->currentForm();

	if (hasShape())
	{
		if (currentShapeElements().isEmpty())
		{
			getShape()->setOffset(0, 0);

			if ( sizeDependsOnParent() && (availableWidth > 0 || availableHeight > 0) )
			{
				QSize inner = getShape()->innerSize(availableWidth, availableHeight);
				form->computeSize(this, inner.width(), inner.height());

				if (form->width(this) > inner.width()) inner.setWidth(form->width(this));
				if (form->height(this) > inner.height()) inner.setHeight(form->height(this));

				getShape()->setInnerSize(inner.width(), inner.height());
			}
			else
			{
				form->computeSize(this, 0, 0);
				getShape()->setInnerSize(form->width(this), form->height(this));
			}
			form->setPos(this, QPoint{getShape()->contentLeft(), getShape()->contentTop()});
		}
		else
		{
			auto currentShape = currentShapeElements().first();
			if (sizeDependsOnParent() && (availableWidth > 0 || availableHeight > 0))
				form->computeSize(this, availableWidth, availableHeight);
			else form->computeSize(this, 0, 0);

			getShape()->setOffset(currentShape->x(this), currentShape->y(this));
			getShape()->setOutterSize(currentShape->width(this), currentShape->height(this));

			form->setPos(this, QPoint{0, 0});
			setSize(form->size(this));
		}
		form->setItemPositions(this);
	}
	else
	{
		if (sizeDependsOnParent() && (availableWidth > 0 || availableHeight > 0))
			form->computeSize(this, availableWidth, availableHeight);
		else form->computeSize(this, 0, 0);
		form->setItemPositions(this);
		setSize(form->size(this));
	}
}

bool DeclarativeItemBase::sizeDependsOnParent() const
{
	return currentForm()->sizeDependsOnParent(this);
}

bool DeclarativeItemBase::isEmpty() const
{
	return currentForm()->isEmpty(this);
}

QList<ItemRegion> DeclarativeItemBase::regions()
{
	if (style()->wholeItemCursor() || style()->noItemRegions()) return Super::regions();
	else return currentForm()->regions(this);
}

GridLayoutFormElement* DeclarativeItemBase::grid(QList<QList<Merge>> elements)
{
	auto grid = new GridLayoutFormElement{};

	for (int y = 0; y < elements.size(); ++y)
	{
		auto& row = elements.at(y);
		for (int x = 0; x < row.size(); ++x)
		{
			auto m = row.at(x);
			if (m.element) grid->put(x, y, m.element);

			Q_ASSERT(m.xSpan > 0);
			Q_ASSERT(m.ySpan > 0);

			if (m.xSpan > 1 || m.ySpan > 1) grid->setCellSpanning(m.xSpan, m.ySpan);
		}
	}

	return grid;
}

}
