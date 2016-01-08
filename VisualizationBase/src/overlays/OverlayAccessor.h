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

// Inspired by: http://channel9.msdn.com/Events/GoingNative/2013/Inheritance-Is-The-Base-Class-of-Evil

class VISUALIZATIONBASE_API OverlayAccessor {
	public:
		OverlayAccessor(const OverlayAccessor& other) = delete;
		OverlayAccessor& operator=(const OverlayAccessor& other) = delete;
		virtual ~OverlayAccessor() {};

		virtual Item* overlayItem() const = 0;
		virtual const QList<Item*>& associatedItems() const = 0;

		// This is required from SAFE_DELETE_ITEM, called from Item::synchronizeCollections.
		// Essentially we want this to look like an Item for that call.
		void removeFromScene() const {};

	protected:
		OverlayAccessor() = default;
};

template <typename T>
class OverlayAccessorTemplate : public OverlayAccessor
{
	public:
		OverlayAccessorTemplate(T* overlayItem) : overlayItem_{overlayItem}{}
		virtual ~OverlayAccessorTemplate() { SAFE_DELETE_ITEM(overlayItem_); }

		virtual T* overlayItem() const override { return overlayItem_; }
		virtual const QList<Item*>& associatedItems() const  override { return overlayItem_->associatedItems(); }

	private:
		T* overlayItem_{};
};

}

// Purposefully outside the namespace for easy creation
template<typename T>
inline Visualization::OverlayAccessorTemplate<T>* makeOverlay(T* overlayItem)
{
	return new Visualization::OverlayAccessorTemplate<T>(overlayItem);
}
