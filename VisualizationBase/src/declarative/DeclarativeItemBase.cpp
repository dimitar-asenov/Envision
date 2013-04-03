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

#include "DeclarativeItemBase.h"
#include "Element.h"
#include "shapes/Shape.h"
#include "shapes/ShapeStyle.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(DeclarativeItemBase, "item")

DeclarativeItemBase::DeclarativeItemBase(Item* parent, const StyleType* style) :
		Item(parent, style)
{}

int DeclarativeItemBase::determineForm()
{
	return 0;
}

void DeclarativeItemBase::determineChildren()
{
	currentFormIndex_ = determineForm();
	forms().at(currentFormIndex_)->synchronizeWithItem(this);
}

void DeclarativeItemBase::updateGeometry(int availableWidth, int availableHeight)
{
	Element* currentForm = forms().at(currentFormIndex_);

	if (hasShape())
	{
		getShape()->setOffset(0, 0);

		if ( sizeDependsOnParent() && (availableWidth > 0 || availableHeight > 0) )
		{
			QSize inner = getShape()->innerSize(availableWidth, availableHeight);
			currentForm->computeSize(this, inner.width(), inner.height());

			if (currentForm->width(this) > inner.width()) inner.setWidth(currentForm->width(this));
			if (currentForm->height(this) > inner.height()) inner.setHeight(currentForm->height(this));

			getShape()->setInnerSize(inner.width(), inner.height());
		}
		else
		{
			currentForm->computeSize(this, 0, 0);
			getShape()->setInnerSize(currentForm->width(this), currentForm->height(this));
		}

		currentForm->setPos(this, QPoint(getShape()->contentLeft(), getShape()->contentTop()));
		currentForm->setItemPositions(this);
	}
	else
	{
		if (sizeDependsOnParent() && (availableWidth > 0 || availableHeight > 0))
			currentForm->computeSize(this, availableWidth, availableHeight);
		else currentForm->computeSize(this, 0, 0);
		currentForm->setItemPositions(this);
		setSize(currentForm->size(this));
	}
}

bool DeclarativeItemBase::sizeDependsOnParent() const
{
	return forms().at(currentFormIndex_)->sizeDependsOnParent(this);
}

QList<ItemRegion> DeclarativeItemBase::regions()
{
	Element* currentForm = forms().at(currentFormIndex_);
	return currentForm->regions(this);
}

}


