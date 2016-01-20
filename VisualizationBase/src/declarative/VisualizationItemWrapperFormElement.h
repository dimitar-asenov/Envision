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

#include "ItemWrapperFormElement.h"

namespace Visualization {

/**
 * This is an item wrapper element, with a specifiable style for the wrapped item.
 */
template <typename ParentType, typename VisualizationType, bool externalSynchronization>
class VisualizationItemWrapperFormElement : public ItemWrapperFormElement<ParentType, VisualizationType> {
		FLUENT_ELEMENT_INTERFACE(VisualizationItemWrapperFormElement);

	public:
		using ChildItem = typename ItemWrapperFormElement<ParentType, VisualizationType>::ChildItem;
		using GetStyleFunction = std::function<const typename VisualizationType::StyleType* (ParentType* v)>;
		using IsEnabledFunction = std::function<bool (ParentType* v)>;

		VisualizationItemWrapperFormElement(ChildItem item);
		VisualizationItemWrapperFormElement(ChildItem item, GetStyleFunction style);
		VisualizationItemWrapperFormElement() = delete;
		VisualizationItemWrapperFormElement(
				const VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>&)
				= default;
		VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>&
				operator=(const VisualizationItemWrapperFormElement<ParentType, VisualizationType,
							 externalSynchronization>&) = delete;
		virtual ~VisualizationItemWrapperFormElement() {};

		virtual VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>* clone()
			const override;

		virtual void synchronizeWithItem(Item* item) override;

		VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>*
		setEnabled(IsEnabledFunction enabled);

	private:

		// Do not forget to update the copy constructor if adding new members.
		GetStyleFunction style_{};
		IsEnabledFunction enabled_{};
};

template <typename ParentType, typename VisualizationType, bool externalSynchronization>
VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>
::VisualizationItemWrapperFormElement(
		ChildItem item)
: ItemWrapperFormElement<ParentType, VisualizationType>{item}
{}

template <typename ParentType, typename VisualizationType, bool externalSynchronization>
VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>
::VisualizationItemWrapperFormElement(
		ChildItem item, GetStyleFunction style)
: ItemWrapperFormElement<ParentType, VisualizationType>{item}, style_{style}
{}

template <typename ParentType, typename VisualizationType, bool externalSynchronization>
VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>*
VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>::clone() const
{
	return new VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>{*this};
}

template <typename ParentType, typename VisualizationType, bool externalSynchronization>
VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>*
VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>
::setEnabled(IsEnabledFunction enabled)
{
	enabled_ = enabled;
	return this;
}

// Used below
template <typename ChildItem, typename Style, bool use>
struct VisualizationItemWrapperFormElementSyncMethod
{
	inline static void sync(Item* item, ChildItem child, bool enabled, Style style)
	{ item->synchronizeItem(child, enabled, style); }
};

template <typename ChildItem, typename Style>
struct VisualizationItemWrapperFormElementSyncMethod<ChildItem, Style, true>
{
	inline static void sync(Item*, ChildItem, bool, Style){}
};

template <typename ParentType, typename VisualizationType, bool externalSynchronization>
void VisualizationItemWrapperFormElement<ParentType, VisualizationType, externalSynchronization>
::synchronizeWithItem(Item* item)
{
	if (!externalSynchronization) // Only synchronize if a style was given, otherwise the synchronization is external.
	{
		Q_ASSERT(style_);
		auto& childItem = (static_cast<ParentType*>(item))->*this->item();
		auto style = style_(static_cast<ParentType*>(item));
		auto enabled = !enabled_ || enabled_(static_cast<ParentType*>(item));

		// This is defined above
		VisualizationItemWrapperFormElementSyncMethod
		<decltype(childItem), decltype(style), externalSynchronization>::sync(item, childItem, enabled, style);
		static_assert(std::is_reference<decltype(childItem)>::value, "Not a reference type");
	}
}

}
