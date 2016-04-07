/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "HighlightOverlayStyle.h"

#include "../items/Text.h"
#include "../overlays/SelectionOverlay.h"
#include "../declarative/DeclarativeItem.h"

namespace Visualization {
	class EmptyItem;
}

namespace Visualization {

class VISUALIZATIONBASE_API HighlightOverlay
		: public Super<Visualization::Overlay<Visualization::DeclarativeItem<HighlightOverlay>>>
{
	ITEM_COMMON(HighlightOverlay)

	public:
		HighlightOverlay(Item* selectedItem, const StyleType* style = itemStyles().get());

		static void initializeForms();
		int determineForm() override;

		void setText(const QString& text);

		virtual QColor customShapeColor() const override;
		void setColor(const QColor& color);

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	protected:
		virtual void updateGeometry(int availableWidth, int availableHeight) override;

	private:
		QColor color_;
		Visualization::Text* info_{};
		Visualization::EmptyItem* background_{};
};

inline void HighlightOverlay::setText(const QString& text) { info_->setText(text); }
inline QColor HighlightOverlay::customShapeColor() const { return color_; }
inline void HighlightOverlay::setColor(const QColor& color) { color_ = color; }

}
