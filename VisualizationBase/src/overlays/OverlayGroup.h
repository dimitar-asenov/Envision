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

namespace Visualization {

class OverlayAccessor;
class Scene;
class Item;

class VISUALIZATIONBASE_API OverlayGroup {
	public:
		~OverlayGroup();

		const QString& name() const;
		Scene* scene() const;

		void hide();
		void show();
		void toggle();

		// Methods for manually adding overlays
		void addOverlay(OverlayAccessor* overlay);
		bool removeOverlay(OverlayAccessor* overlay);
		bool removeOverlay(Item* overlay);
		bool removeOverlayOf(Item* itemWithOverlay);
		void clear();

		// Methods for dynamically constructing overlays from items
		using ItemGetter1Item = std::function<QList<Item*>()>;
		void setDynamic1Item(ItemGetter1Item getter);
		using ItemGetter2Items = std::function<QList<QPair<Item*, Item*>>()>;
		void setDynamic2Items(ItemGetter2Items getter);
		using OverlayConstructor1Arg = std::function<OverlayAccessor* (Item* item)>;
		void setOverlayConstructor1Arg(OverlayConstructor1Arg constructor);
		using OverlayConstructor2Args = std::function<OverlayAccessor* (Item* item1, Item* item2)>;
		void setOverlayConstructor2Args(OverlayConstructor2Args constructor);
		void addOverlayFor(Item* item);
		void addOverlayFor(Item* item1, Item* item2);
		using PostUpdateFunction = std::function<void (OverlayGroup& group)>;
		void setPostUpdateFunction(PostUpdateFunction function);

		const QList<OverlayAccessor*>& overlays() const;
		QList<OverlayAccessor*> overlaysForItem(const Item* item) const;

	private:
		friend class Scene;
		OverlayGroup(Scene* scene, QString name);

		Scene* scene_{};
		QString name_;
		bool hidden_{};

		QList<OverlayAccessor*> overlays_;

		ItemGetter1Item itemGetterFunction1_;
		ItemGetter2Items itemGetterFunction2_;
		OverlayConstructor1Arg constructorFunction1_;
		OverlayConstructor2Args constructorFunction2_;
		PostUpdateFunction postUpdateFunction_;

		bool removeOverlay(QList<OverlayAccessor*>::iterator it);

		/**
		 * Called by scene to update all Overlay items.
		 */
		void update();
};

inline const QString& OverlayGroup::name() const {return name_;}
inline Scene* OverlayGroup::scene() const {return scene_;}
inline const QList<OverlayAccessor*>& OverlayGroup::overlays() const { return overlays_; }

} /* namespace Visualization */
