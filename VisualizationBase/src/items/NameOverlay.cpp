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

#include "NameOverlay.h"
#include "shapes/Shape.h"
#include "views/MainView.h"

namespace Visualization {

class BottomItemNode
{
	public:
		BottomItemNode* parent{};
		Item* item{};
		QList<BottomItemNode*> children{};
		bool hasPaintedTextInHierarchy{};

		BottomItemNode(BottomItemNode* parent = nullptr, Item* item = nullptr) : parent{parent}, item{item}{}
		~BottomItemNode()
		{
			for (auto node : children) SAFE_DELETE(node);
			children.clear();
		}

		void insert(Item* item)
		{
			for (auto node : children)
				if (node->item->isAncestorOf(item))
				{
					node->insert(item);
					return;
				}

			children.append(new BottomItemNode(this, item));
		}

		void resetFlags()
		{
			hasPaintedTextInHierarchy = false;
			for (auto node : children) node->resetFlags();
		}

		void markPainted()
		{
			hasPaintedTextInHierarchy = true;
			auto p = parent;
			while (p) { p->hasPaintedTextInHierarchy = true; p = p->parent;}
		}
};

ITEM_COMMON_DEFINITIONS(NameOverlay, "item")

NameOverlay::NameOverlay(Scene* scene, const StyleType* style) :
Super(nullptr, style)
{
	setFlags(0);
	setAcceptedMouseButtons(0);
	setZValue(LAYER_NAME_OVERLAY_Z);
	setItemCategory(Scene::SelectionItemCategory);
	scene->addItem(this);
}

NameOverlay::~NameOverlay()
{
	SAFE_DELETE(bottomItems_);
}

Item::UpdateType NameOverlay::needsUpdate()
{
	return FullUpdate;
}

void NameOverlay::determineChildren()
{
	const double OVERLAY_SCALE_TRESHOLD = 0.5;

	auto scalingFactor = mainViewScalingFactor();
	if (scalingFactor < OVERLAY_SCALE_TRESHOLD && !bottomItems_)
	{
		// Add the overlays
		bottomItems_ = new BottomItemNode{};

		QList<Item*> stack = scene()->topLevelItems();
		while (!stack.isEmpty())
		{
			auto item = stack.takeLast();

			const double OVERLAY_MIN_WIDTH = 200;
			const double OVERLAY_MIN_HEIGHT = 200;
				if (item->widthInParent() < OVERLAY_MIN_WIDTH
						|| item->heightInParent() < OVERLAY_MIN_HEIGHT)
					continue;

			auto definesSymbol = item->node() && item->node()->definesSymbol();

			if (definesSymbol) bottomItems_->insert(item);
			stack.append(item->childItems());
		}

		// Prepare a more efficient way to explore the nodes when rendering
		dfsOrder_.clear();
		QList<BottomItemNode*> dfsStack = {bottomItems_};
		while (!dfsStack.isEmpty())
		{
			auto last = dfsStack.takeLast();
			dfsOrder_.prepend(last);
			dfsStack.append(last->children);
		}
	}
	else if (scalingFactor >= OVERLAY_SCALE_TRESHOLD && bottomItems_)
		SAFE_DELETE(bottomItems_);

}

void NameOverlay::updateGeometry(int, int)
{
	setSize(scene()->sceneRect().size());
	setPos(scene()->sceneRect().topLeft());
}

const QString& NameOverlay::overlayText(Item* item) const
{
	Q_ASSERT(item->node());
	Q_ASSERT(item->node()->definesSymbol());
	return item->node()->symbolName();
}

void NameOverlay::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Super::paint(painter, option, widget);
	if (!bottomItems_) return;

	// Only paint if zoomed out
	qreal scaleFactor = painter->worldTransform().m11();
	if (!isCategoryHiddenDuringPaint() && scaleFactor <= 1.0)
	{
		// Adjust the font size for a constant on screen text size
		auto font = style()->font();
		font.setPixelSize(font.pixelSize()/scaleFactor);
		QFontMetrics qfm(font);

		// Get the main view, if we're currently rendering on it.
		auto mainView = dynamic_cast<MainView*>(scene()->currentPaintView());

		bottomItems_->resetFlags();
		for (auto node : dfsOrder_)
		{
			if (node->hasPaintedTextInHierarchy) continue;

			auto item = node->item;
			if (!item) continue;

			auto rect = item->sceneBoundingRect().toRect();

			// Only render texts that are inside the view
			auto inView = true;
			if (mainView)
			{
				auto viewRect = mainView->mapFromScene(rect).boundingRect();
				if (viewRect.right() < 0 || viewRect.bottom() < 0) inView = false;
				else if (viewRect.top() > mainView->height() || viewRect.left() > mainView->width() ) inView = false;
			}

			// Do not render texts that are too small
			auto scaledSize = rect.size() * scaleFactor;
			auto tooSmall = scaledSize.width() < 20 || scaledSize.height() < 20;

			auto text = overlayText(item);
			if (inView && !tooSmall && fitsInRect(text, qfm, rect))
			{
				node->markPainted();
				painter->setPen(style()->pen());
				painter->setFont(font);
				painter->drawText(rect, Qt::AlignCenter | Qt::TextWrapAnywhere, text);
			}
		}
	}
}

bool NameOverlay::fitsInRect(const QString& text, const QFontMetrics& qfm, const QRect& rect) const
{
	auto needed = qfm.boundingRect(rect, Qt::AlignCenter | Qt::TextWrapAnywhere, text);
	return needed.width() < rect.width() && needed.height() < rect.height();
}

}
