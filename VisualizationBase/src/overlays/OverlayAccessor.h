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

	protected:
		OverlayAccessor() = default;
};

template <typename OverlayType>
class OverlayAccessorTemplate : public OverlayAccessor
{
	public:
		OverlayAccessorTemplate(OverlayType* overlayItem) : overlayItem_{overlayItem}{}
		virtual ~OverlayAccessorTemplate() { SAFE_DELETE_ITEM(overlayItem_); }

		virtual OverlayType* overlayItem() const override { return overlayItem_; }
		virtual const QList<Item*>& associatedItems() const  override { return overlayItem_->associatedItems(); }

	private:
		OverlayType* overlayItem_{};
};

template <typename OverlayType, typename Deleter>
class OverlayAccessorTemplateWithDeleter : public OverlayAccessor
{
	public:
		OverlayAccessorTemplateWithDeleter(OverlayType* overlayItem, Deleter deleter)
			: overlayItem_{overlayItem}, deleter_{deleter}{}
		virtual ~OverlayAccessorTemplateWithDeleter() { deleter_(overlayItem_); }

		virtual OverlayType* overlayItem() const override { return overlayItem_; }
		virtual const QList<Item*>& associatedItems() const  override { return overlayItem_->associatedItems(); }

	private:
		OverlayType* overlayItem_{};
		Deleter deleter_{};
};

}

// Purposefully outside the namespace for easy creation
template<typename OverlayType>
inline Visualization::OverlayAccessorTemplate<OverlayType>* makeOverlay(OverlayType* overlayItem)
{
	return new Visualization::OverlayAccessorTemplate<OverlayType>{overlayItem};
}

template<typename OverlayType, typename Deleter>
inline Visualization::OverlayAccessorTemplateWithDeleter<OverlayType, Deleter>* makeOverlay(OverlayType* overlayItem,
																								Deleter deleter)
{
	return new Visualization::OverlayAccessorTemplateWithDeleter<OverlayType, Deleter>{overlayItem, deleter};
}
