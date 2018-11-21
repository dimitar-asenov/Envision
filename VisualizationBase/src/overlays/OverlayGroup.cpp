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
#include "OverlayGroup.h"

#include "OverlayAccessor.h"
#include "../items/Item.h"
#include "../Scene.h"

namespace Visualization {

OverlayGroup::~OverlayGroup()
{
	for (auto & overlay : overlays_) SAFE_DELETE(overlay);
}

OverlayGroup::OverlayGroup(Scene* scene, QString name) : scene_{scene}, name_{name}
{
	Q_ASSERT(scene);
	Q_ASSERT(!name.isEmpty());
}

void OverlayGroup::hide()
{
	hidden_ = true;
	for (auto& o : overlays_) o->overlayItem()->hide();
}

void OverlayGroup::show()
{
	hidden_ = false;
	for (auto& o : overlays_) o->overlayItem()->show();
}

void OverlayGroup::toggle()
{
	if (hidden_) show();
	else hide();
}

void OverlayGroup::addOverlay(OverlayAccessor* overlay)
{
	overlays_.append(overlay);
	overlay->overlayItem()->setUpdateNeeded(Item::StandardUpdate);
	if (hidden_) overlay->overlayItem()->hide();

	scene_->addItem(overlay->overlayItem());
	scene_->scheduleUpdate();
}

bool OverlayGroup::removeOverlay(OverlayAccessor* overlay)
{
	return removeOverlay(std::find(overlays_.begin(), overlays_.end(), overlay));
}

bool OverlayGroup::removeOverlay(Item* overlay)
{
	return removeOverlay( std::find_if(overlays_.begin(), overlays_.end(),
			[=](OverlayAccessor* o){return o->overlayItem() == overlay;}));
}

bool OverlayGroup::removeOverlayOf(Item* itemWithOverlay)
{
	return removeOverlay( std::find_if(overlays_.begin(), overlays_.end(),
			[=](OverlayAccessor* o){return o->associatedItems().contains(itemWithOverlay);}));
}

bool OverlayGroup::removeOverlay(QList<OverlayAccessor*>::iterator it)
{
	if (it != overlays_.end())
	{
		SAFE_DELETE(*it);
		overlays_.erase(it);
		scene_->scheduleUpdate();
		return true;
	}
	else return false;
}

void OverlayGroup::clear()
{
	for (auto& overlay : overlays_) SAFE_DELETE(overlay);
	overlays_.clear();
}

void OverlayGroup::setDynamic1Item(ItemGetter1Item getter)
{
	itemGetterFunction1_ = getter;
}

void OverlayGroup::setDynamic2Items(ItemGetter2Items getter)
{
	itemGetterFunction2_ = getter;
}

void OverlayGroup::setOverlayConstructor1Arg(OverlayConstructor1Arg constructor)
{
	constructorFunction1_ = constructor;
}

void OverlayGroup::setOverlayConstructor2Args(OverlayConstructor2Args constructor)
{
	constructorFunction2_ = constructor;
}

void OverlayGroup::setPostUpdateFunction(PostUpdateFunction function)
{
	postUpdateFunction_ = function;
}

void OverlayGroup::addOverlayFor(Item* item)
{
	addOverlay(constructorFunction1_(item));
}

void OverlayGroup::addOverlayFor(Item* item1, Item* item2)
{
	addOverlay(constructorFunction2_(item1, item2));
}

QList<OverlayAccessor*> OverlayGroup::overlaysForItem(const Item* item) const
{
	Q_ASSERT(item);
	QList<OverlayAccessor*> result;
	for (auto accessor: overlays_)
		if (accessor->associatedItems().contains(const_cast<Item*>(item))) // TODO: is this really the best way?
			result << accessor;
	return result;
}

void OverlayGroup::update()
{
	if (!hidden_)
	{
		if (itemGetterFunction1_)
		{
			Q_ASSERT(constructorFunction1_);
			Item::synchronizeCollections(nullptr, itemGetterFunction1_(), overlays_,
					[](Item* itemToOverlay, OverlayAccessor* overlay)
					{
						return itemToOverlay == overlay->associatedItems().first();
					},
					[this](Item*, Item* itemToOverlay)
					{
						auto overlay = constructorFunction1_(itemToOverlay);
						Q_ASSERT(overlay);
						scene_->addItem(overlay->overlayItem());
						return overlay;
					},
					[](Item*, Item*, OverlayAccessor*) {return false;},
					[](OverlayAccessor* accessor) { SAFE_DELETE(accessor); }
					);
		}

		if (itemGetterFunction2_)
		{
			Q_ASSERT(constructorFunction2_);
			Item::synchronizeCollections(nullptr, itemGetterFunction2_(), overlays_,
					[](const QPair<Item*, Item*>& pair, OverlayAccessor* overlay)
					{
						return overlay->associatedItems().size() == 2
								&& pair.first == overlay->associatedItems().first()
								&& pair.second == overlay->associatedItems().at(1);
					},
					[this](Item*, const QPair<Item*, Item*>& pair)
					{
						auto overlay = constructorFunction2_(pair.first, pair.second);
						Q_ASSERT(overlay);
						scene_->addItem(overlay->overlayItem());
						return overlay;
					},
					[](Item*, const QPair<Item*, Item*>&, OverlayAccessor*){return false;},
					[](OverlayAccessor* accessor) { SAFE_DELETE(accessor); }
					);
		}

		for (auto& overlay : overlays_)
		{
			// Hide/show overlays depending on whether their associated items are visible (e.g. when changing views)
			bool allVisible = std::all_of(overlay->associatedItems().begin(), overlay->associatedItems().end(),
													[](Item* item) {return item->isVisible();});
			if (allVisible != overlay->overlayItem()->isVisible())
				overlay->overlayItem()->setVisible(allVisible);

			overlay->overlayItem()->setUpdateNeeded(Item::StandardUpdate);
			overlay->overlayItem()->updateSubtree();
		}

		if (postUpdateFunction_) postUpdateFunction_(*this);
	}
}

}
