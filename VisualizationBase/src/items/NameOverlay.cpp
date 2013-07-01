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

#include "NameOverlay.h"
#include "shapes/Shape.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(NameOverlay, "item")

NameOverlay::NameOverlay(Item* bottomItem, const StyleType* style) :
Super(nullptr, style), bottomItem_(bottomItem)
{
	setFlags(0);
	setAcceptedMouseButtons(0);
	setZValue(LAYER_NAME_OVERLAY_Z);
	setItemCategory(Scene::SelectionItemCategory);
}

NameOverlay::~NameOverlay()
{
	bottomItem_ = nullptr;
}

void NameOverlay::setBottomItem(Item* item)
{
	bottomItem_ = item;
}

Item::UpdateType NameOverlay::needsUpdate()
{
	return FullUpdate;
}

void NameOverlay::determineChildren()
{}

void NameOverlay::updateGeometry(int, int)
{
	if (hasShape())
	{
		getShape()->setInnerSize(bottomItem_->width(), bottomItem_->height());
		QPointF pos = QPointF( getShape()->contentLeft(), getShape()->contentTop() );

		setPos(bottomItem_->mapToScene(0,0) - pos);
	}
	else
	{
		setSize(bottomItem_->boundingRect().size().toSize());
		setPos(bottomItem_->mapToScene(0,0));
	}
}

const QString& NameOverlay::overlayText() const
{
	Q_ASSERT(bottomItem_->node());
	Q_ASSERT(bottomItem_->node()->definesSymbol());
	return bottomItem_->node()->symbolName();
}

void NameOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Super::paint(painter, option, widget);

	qreal scaleFactor = painter->worldTransform().m11();
	if (scaleFactor <= 1.0)
	{

		auto font = style()->font();
		font.setPixelSize(font.pixelSize()/scaleFactor);
		auto text = overlayText();
		if (fitsInBoundingRect(text, font))
		{
			painter->setPen(style()->pen());
			painter->setFont(font);
			painter->drawText(boundingRect(), Qt::AlignCenter | Qt::TextWrapAnywhere, text);
		}
	}
}

bool NameOverlay::fitsInBoundingRect(const QString& text, const QFont& font) const
{
	QFontMetrics qfm(font);
	auto bound = boundingRect();
	auto needed = qfm.boundingRect(bound.toRect(), Qt::AlignCenter | Qt::TextWrapAnywhere, text);
	return needed.width() < bound.width() && needed.height() < bound.height();
}

}
