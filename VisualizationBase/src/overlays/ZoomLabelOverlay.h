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

#pragma once

#include "../visualizationbase_api.h"
#include "../declarative/DeclarativeItem.h"
#include "../declarative/DeclarativeItemBaseStyle.h"
#include "../overlays/Overlay.h"

namespace Visualization {

class Static;
class Text;
class TextStyle;
class StaticStyle;
class OverlayGroup;

class VISUALIZATIONBASE_API ZoomLabelOverlay : public Super<Overlay<DeclarativeItem<ZoomLabelOverlay>>>
{
	ITEM_COMMON_CUSTOM_STYLENAME(ZoomLabelOverlay, DeclarativeItemBaseStyle)

	public:
		ZoomLabelOverlay(Item* itemWithLabel, const StyleType* style = itemStyles().get());

		static void initializeForms();
		int determineForm() override;

		virtual bool isSensitiveToScale() const override;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

	protected:
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;

	private:
		friend class Scene;

		Static* icon_{};
		Text* text_{};

		const StaticStyle* iconStyle_{};
		const TextStyle* textStyle_{};
		QBrush backgroundBrush_;

		int postUpdateRevision_{};
		bool mayBeHiddenIfChildrenHaveOverlays_{};

		static QHash<Item*, ZoomLabelOverlay*>& itemToOverlay();

		const QString& associatedItemText() const;

		const StaticStyle* associatedItemIconStyle() const;
		const TextStyle* associatedItemTextStyle() const;
		QBrush associatedItemLabelBackground() const;

		static QList<Item*> itemsThatShouldHaveZoomLabel(Scene* scene);
		static void setItemPositionsAndHideOverlapped(OverlayGroup& group);
		void postUpdate(int revision);
		void adjustPositionOrHide();
		static void reduceRect(QRect& rectToReduce, const QRect& rectToExclude);

		qreal computeScaleToUse() const;

		void resetAfterBeingUnused();

		static constexpr double OVERLAY_MIN_WIDTH = 30;
		static constexpr double OVERLAY_MIN_HEIGHT = 15;
		static constexpr double ITEM_MAX_WIDTH = 500;
		static constexpr double ITEM_MAX_HEIGHT = 500;
		static constexpr double OVERLAY_MAX_HEIGHT = 40;
		static constexpr double SHOW_OVERLAY_IF_ITEM_TEXT_SMALLER_THAN = 8;
};

inline void ZoomLabelOverlay::resetAfterBeingUnused()
{
	postUpdateRevision_ = 0;
	mayBeHiddenIfChildrenHaveOverlays_ = false;
}

}
