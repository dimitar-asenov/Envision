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
#include "ZoomLabelOverlay.h"
#include "../shapes/Shape.h"
#include "../declarative/DeclarativeItem.hpp"
#include "../icons/Icon.h"
#include "../icons/IconStyle.h"
#include "../items/StaticStyle.h"
#include "../items/Text.h"
#include "../items/Static.h"
#include "../items/TextStyle.h"
#include "../items/RootItem.h"
#include "OverlayAccessor.h"
#include "../views/MainView.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/items/ViewItem.h"

namespace Visualization {

DEFINE_ITEM_COMMON(ZoomLabelOverlay, "item")

QHash<Item*, ZoomLabelOverlay*>& ZoomLabelOverlay::itemToOverlay()
{
	static QHash<Item*, ZoomLabelOverlay*> map;
	return map;
}

ZoomLabelOverlay::ZoomLabelOverlay(Item* itemWithLabel, const StyleType* style) : Super{{itemWithLabel}, style}
{
	Q_ASSERT(itemWithLabel->node());
	Q_ASSERT(itemWithLabel->node()->definesSymbol());
	iconStyle_ = associatedItemIconStyle();
	itemToOverlay().insert(itemWithLabel, this);
}

void ZoomLabelOverlay::determineChildren()
{
	Super::determineChildren();
	text_->setText(associatedItemText());
}

void ZoomLabelOverlay::updateGeometry(int availableWidth, int availableHeight)
{
	Super::updateGeometry(availableWidth, availableHeight);

	setPos(associatedItem()->scenePos());
	setScale(computeScaleToUse());
}

bool ZoomLabelOverlay::isSensitiveToScale() const
{
	return true;
}

int ZoomLabelOverlay::determineForm()
{
	return iconStyle_ ? 1 : 0;
}

void ZoomLabelOverlay::initializeForms()
{
	addForm(item<Text>(&I::text_, [](I* v){return v->associatedItemTextStyle();}));

	addForm(grid({{item<Static>(&I::icon_, [](I* v){ return v->iconStyle_;}),
		item<Text>(&I::text_, [](I* v){return v->associatedItemTextStyle();})}})
			  ->setVerticalAlignment(LayoutStyle::Alignment::Center));
}

const StaticStyle* ZoomLabelOverlay::associatedItemIconStyle() const
{
	const StaticStyle* iconStyle{};
	for (auto child : associatedItem()->childItems())
	{
		if ( auto childStaticStyle = dynamic_cast<const StaticStyle*>(child->style()) )
			if (dynamic_cast<const IconStyle*> (&childStaticStyle->itemStyle()))
		{
			if (!iconStyle) iconStyle = childStaticStyle;
			else return nullptr; // We found at least 2 icons => ambiguous
		}

	}

	return iconStyle; // Could be nullptr to indicate no icon;
}

const QString& ZoomLabelOverlay::associatedItemText() const
{
	return associatedItem()->node()->symbolName();
}

const TextStyle* ZoomLabelOverlay::associatedItemTextStyle() const
{
	const TextStyle* textStyle{};
	for (auto child : associatedItem()->childItems())
		if ( auto textChild = DCast<TextRenderer>(child) )
		{
			if (textChild->text() == associatedItemText())
			{
				if (!textStyle) textStyle = textChild->style();
				else
				{
					//we have found more than one text item representing the same string, ambiguous
					textStyle = nullptr;
					break;
				}
			}
		}

	return textStyle ? textStyle : Text::itemStyles().get();
}

QList<Item*> ZoomLabelOverlay::itemsThatShouldHaveZoomLabel(Scene* scene)
{
	// TODO at the moment only one viewItem is used for diffs, maybe changes in future
	// disable zoom labels for diff views
	if (Visualization::VisualizationManager::instance().mainScene()->currentViewItem()->name().contains("DiffView"))
		return {};

	QList<Item*> result;

	const double OVERLAY_SCALE_TRESHOLD = 0.5;
	auto scalingFactor = scene->mainViewScalingFactor();

	if (scalingFactor >= OVERLAY_SCALE_TRESHOLD)
	{
		itemToOverlay().clear();
		return result;
	}

	View* mainView{};
	for (auto view : scene->views())
		if (auto mv = dynamic_cast<MainView*>(view))
		{
			mainView = mv;
			break;
		}
	Q_ASSERT(mainView);

	QList<Item*> stack = scene->topLevelItems();
	while (!stack.isEmpty())
	{
		auto item = stack.takeLast();
		if (!item->isVisible())
			continue;

		auto scaledWidth = item->widthInParent() * scalingFactor;
		auto scaledHeight = item->heightInParent() * scalingFactor;
		if (scaledWidth < OVERLAY_MIN_WIDTH || scaledHeight < OVERLAY_MIN_HEIGHT) continue; // Do not explore children
		if (!mainView->visibleRect().intersects(item->sceneBoundingRect())) continue; // Do not explore children

		stack.append(item->childItems());

		bool definesSymbol = !DCast<RootItem>(item) && item->node() && item->node()->definesSymbol();
		if (definesSymbol) result.append(item);
	}

	return result;
}

void ZoomLabelOverlay::setItemPositionsAndHideOverlapped(OverlayGroup& group)
{
	static int postUpdateRevision{};
	++postUpdateRevision;

	for (auto accessor : group.overlays())
		static_cast<ZoomLabelOverlay*>(accessor->overlayItem())->postUpdate(postUpdateRevision);
}

void ZoomLabelOverlay::postUpdate(int revision)
{
	// If already updated, do nothing
	if (postUpdateRevision_ == revision) return;
	postUpdateRevision_ = revision;

	// Make sure all overlays above this one are updated
	auto item = associatedItem()->parent();
	while (item)
	{
		auto overlayIt = itemToOverlay().find(item);
		if (overlayIt != itemToOverlay().end())
		{
			overlayIt.value()->postUpdate(revision);
			break;
		}

		item = item->parent();
	}

	// Finally adjust the position of this overlay so that it does not overlap or if that's impossible, hide it.
	adjustPositionOrHide();
}

inline void ZoomLabelOverlay::reduceRect(QRect& rectToReduce, const QRect& rectToExclude)
{
	QRect intersected = rectToReduce.intersected(rectToExclude);
	if (intersected.isEmpty()) return; // No intersection

	// Now we will grow the intersected rect until it has at least three sides equal to the sides of rectToReduce
	// Then we'll just take what's left

	// Make sure only one of top/bottom is in the middle
	int distanceToTop = intersected.top() - rectToReduce.top();
	int distanceToBottom = rectToReduce.bottom() - intersected.bottom();
	if (distanceToTop > distanceToBottom) intersected.setBottom(rectToReduce.bottom());
	else intersected.setTop(rectToReduce.top());

	Q_ASSERT(rectToReduce.top() == intersected.top() || rectToReduce.bottom() == intersected.bottom());

	// Make sure only one of left/right is in the middle
	int distanceToLeft = intersected.left() - rectToReduce.left();
	int distanceToRight = rectToReduce.right() - intersected.right();
	if (distanceToLeft > distanceToRight) intersected.setRight(rectToReduce.right());
	else intersected.setLeft(rectToReduce.left());

	Q_ASSERT(rectToReduce.left() == intersected.left() || rectToReduce.right() == intersected.right());

	// At this point we might still have an intersecting rect in a corner. In that case take away the chunk that has
	// the smaller area.
	int horizontalArea = intersected.height()* rectToReduce.width();
	int verticalArea = intersected.width()* rectToReduce.height();
	if (horizontalArea < verticalArea)
	{
		intersected.setLeft( rectToReduce.left() );
		intersected.setRight( rectToReduce.right() );
	}
	else
	{
		intersected.setTop( rectToReduce.top() );
		intersected.setBottom( rectToReduce.bottom() );
	}

	Q_ASSERT( (rectToReduce.top() == intersected.top() && rectToReduce.bottom() == intersected.bottom()) ||
				 (rectToReduce.left() == intersected.left() && rectToReduce.right() == intersected.right()));

	// At this point we can just do rectToReduce = rectToReduce - intersected
	if (rectToReduce.top() != intersected.top()) rectToReduce.setBottom(intersected.top());
	else if (rectToReduce.bottom() != intersected.bottom()) rectToReduce.setTop(intersected.bottom()+1);
	else if (rectToReduce.left() != intersected.left()) rectToReduce.setRight(intersected.left());
	else if (rectToReduce.right() != intersected.right()) rectToReduce.setLeft(intersected.right()+1);
	else // complete overlap
	{
		rectToReduce.setWidth(0);
		rectToReduce.setHeight(0);
	}
}


void ZoomLabelOverlay::adjustPositionOrHide()
{
	auto availableRect = associatedItem()->sceneBoundingRect().toRect();

	auto item = associatedItem()->parent();
	while (item)
	{
		auto overlayIt = itemToOverlay().find(item);
		if (overlayIt != itemToOverlay().end() && overlayIt.value()->isVisible())
			reduceRect(availableRect, overlayIt.value()->sceneBoundingRect().toRect());

		item = item->parent();
	}

	// At this point we know in what amount of space we're supposed to fit.

	// If the space is too small don't bother trying to compute a scale
	bool visible = true;
	auto scalingFactor = scene()->mainViewScalingFactor();
	if (availableRect.width() * scalingFactor < OVERLAY_MIN_WIDTH
			|| availableRect.height() * scalingFactor < OVERLAY_MIN_HEIGHT) visible = false;

	// If the original item's header is actually visible, then don't show this one
	if (scalingFactor * associatedItemTextStyle()->font().pixelSize() >= SHOW_OVERLAY_IF_ITEM_TEXT_SMALLER_THAN)
		visible = false;

	// If there might be enough space, then try to fit in
	if (visible)
	{
		setPos(availableRect.topLeft());
		setScale(computeScaleToUse());
	}

	if (visible != isVisible()) setVisible(visible);
}

qreal ZoomLabelOverlay::computeScaleToUse() const
{
	auto maxScale = 1/mainViewScalingFactor();
	auto widthScale = associatedItem()->widthInScene() / (double) widthInLocal();
	auto heightScale = associatedItem()->heightInScene() / (double) heightInLocal();
	auto scaleToUse = maxScale;
	if (widthScale < scaleToUse) scaleToUse = widthScale;
	if (heightScale < scaleToUse) scaleToUse = heightScale;

	if (scaleToUse * heightInLocal() * mainViewScalingFactor() > OVERLAY_MAX_HEIGHT)
		scaleToUse = OVERLAY_MAX_HEIGHT / ( mainViewScalingFactor() * heightInLocal() );

	return scaleToUse;
}

}
