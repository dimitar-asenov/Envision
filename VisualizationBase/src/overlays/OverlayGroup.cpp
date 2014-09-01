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

#include "Overlay.h"
#include "../Scene.h"
#include "../items/ItemStyle.h"

namespace Visualization {

OverlayGroup::~OverlayGroup()
{
	for (auto & overlay : overlays_) SAFE_DELETE_ITEM(overlay);
}

OverlayGroup::OverlayGroup(Scene* scene, QString name) : scene_{scene}, name_{name}
{
	Q_ASSERT(scene);
	Q_ASSERT(!name.isEmpty());
}

void OverlayGroup::hide()
{
	hidden_ = true;
	for (auto& o : overlays_) o->hide();
}

void OverlayGroup::show()
{
	hidden_ = false;
	for (auto& o : overlays_) o->show();
}

void OverlayGroup::addOverlay(Overlay* overlay)
{
	overlays_.append(overlay);
	overlay->setUpdateNeeded(Item::StandardUpdate);
	if (hidden_) overlay->hide();

	scene_->addItem(overlay);
	scene_->scheduleUpdate();
}

void OverlayGroup::removeOverlay(Overlay* overlay)
{
	removeOverlay(std::find(overlays_.begin(), overlays_.end(), overlay));
}


void OverlayGroup::removeOverlayOf(Item* itemWithOverlay)
{
	removeOverlay( std::find_if(overlays_.begin(), overlays_.end(),
			[=](Overlay* o){return o->allItems().contains(itemWithOverlay);}));
}

void OverlayGroup::removeOverlay(QList<Overlay*>::iterator it)
{
	if (it != overlays_.end())
	{
		SAFE_DELETE_ITEM(*it);
		overlays_.erase(it);
		scene_->scheduleUpdate();
	}
}

void OverlayGroup::clear()
{
	for (auto& overlay : overlays_) SAFE_DELETE_ITEM(overlay);
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

void OverlayGroup::addOverlayFor(Item* item)
{
	addOverlay(constructorFunction1_(item));
}

void OverlayGroup::addOverlayFor(Item* item1, Item* item2)
{
	addOverlay(constructorFunction2_(item1, item2));
}

void OverlayGroup::update()
{
	if (!hidden_)
	{
		if (itemGetterFunction1_)
		{
			Q_ASSERT(constructorFunction1_);
			Item::synchronizeCollections(nullptr, itemGetterFunction1_(), overlays_,
					[](Item*, Overlay*){return true;}, // Always match and reuse/sync visualizations
					[this](Item*, Item* itemToOverlay)
					{
						auto overlay = constructorFunction1_(itemToOverlay);
						Q_ASSERT(overlay);
						scene_->addItem(overlay);
						return overlay;
					},
					[](Item*, Item* itemToOverlay, Overlay* overlay)
					{
						if (itemToOverlay == overlay->item()) return false;
						overlay->setItems({itemToOverlay});
						return true;
					});
		}

		if (itemGetterFunction2_)
		{
			Q_ASSERT(constructorFunction2_);
			Item::synchronizeCollections(nullptr, itemGetterFunction2_(), overlays_,
					[](const QPair<Item*, Item*>&, Overlay*){return true;}, // Always match and reuse/sync visualizations
					[this](Item*, const QPair<Item*, Item*>& pair)
					{
						auto overlay = constructorFunction2_(pair.first, pair.second);
						Q_ASSERT(overlay);
						scene_->addItem(overlay);
						return overlay;
					},
					[](Item*, const QPair<Item*, Item*>& pair, Overlay* overlay)
					{
						if ( pair.first == overlay->first() && pair.second == overlay->second()) return false;
						overlay->setItems({pair.first, pair.second});
						return true;
					});
		}

		for (auto& overlay : overlays_)
			overlay->updateSubtree();
	}
}

} /* namespace Visualization */
