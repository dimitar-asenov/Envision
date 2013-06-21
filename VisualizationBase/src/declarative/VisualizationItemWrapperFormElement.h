/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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
template <class ParentType, class VisualizationType>
class VisualizationItemWrapperFormElement : public ItemWrapperFormElement<ParentType, VisualizationType> {
		FLUENT_ELEMENT_INTERFACE(VisualizationItemWrapperFormElement);

	public:
		using ChildItem = typename ItemWrapperFormElement<ParentType, VisualizationType>::ChildItem;
		using GetStyleFunction = std::function<const typename VisualizationType::StyleType* (ParentType* v)>;
		using IsEnabledFunction = std::function<bool (ParentType* v)>;

		VisualizationItemWrapperFormElement(ChildItem item, GetStyleFunction style);
		VisualizationItemWrapperFormElement() = delete;
		VisualizationItemWrapperFormElement(
				const VisualizationItemWrapperFormElement<ParentType,VisualizationType>& other) = default;
		VisualizationItemWrapperFormElement<ParentType,VisualizationType>&
				operator=(const VisualizationItemWrapperFormElement<ParentType,VisualizationType>&) = delete;
		virtual ~VisualizationItemWrapperFormElement() {};

		virtual VisualizationItemWrapperFormElement<ParentType, VisualizationType>* clone() const override;

		virtual void synchronizeWithItem(Item* item) override;

		VisualizationItemWrapperFormElement<ParentType, VisualizationType>* setEnabled(IsEnabledFunction enabled);

	private:

		// Do not forget to update the copy constructor if adding new members.
		GetStyleFunction style_{};
		IsEnabledFunction enabled_{};
};

template <class ParentType, class VisualizationType>
VisualizationItemWrapperFormElement<ParentType, VisualizationType>::VisualizationItemWrapperFormElement(
		ChildItem item, GetStyleFunction style)
: ItemWrapperFormElement<ParentType, VisualizationType>{item}, style_{style}
{}

template <class ParentType, class VisualizationType>
VisualizationItemWrapperFormElement<ParentType, VisualizationType>*
VisualizationItemWrapperFormElement<ParentType, VisualizationType>::clone() const
{
	return new VisualizationItemWrapperFormElement<ParentType, VisualizationType>(*this);
}

template <class ParentType, class VisualizationType>
VisualizationItemWrapperFormElement<ParentType, VisualizationType>*
VisualizationItemWrapperFormElement<ParentType, VisualizationType>::setEnabled(IsEnabledFunction enabled)
{
	enabled_ = enabled;
	return this;
}

template <class ParentType, class VisualizationType>
void VisualizationItemWrapperFormElement<ParentType, VisualizationType>::synchronizeWithItem(Item* item)
{
	auto& childItem = (static_cast<ParentType*>(item))->*this->item();
	auto style = style_(static_cast<ParentType*>(item));
	auto enabled = !enabled_ || enabled_(static_cast<ParentType*>(item));

	if(childItem && !enabled)
	{
		SAFE_DELETE_ITEM(childItem);
		item->setUpdateNeeded(Item::StandardUpdate);
	}

	if(!childItem && enabled)
	{
		childItem = new VisualizationType(item, style);
		item->setUpdateNeeded(Item::StandardUpdate);
	}
	else if (enabled) childItem->setStyle(style);
}

} /* namespace Visualization */
