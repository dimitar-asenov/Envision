/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#ifndef VisualizationBase_ITEMWRAPPERELEMENT_H_
#define VisualizationBase_ITEMWRAPPERELEMENT_H_

#include "../visualizationbase_api.h"
#include "Element.h"
#include "../items/Text.h"

#include "ModelBase/src/nodes/Node.h"
#include "../renderer/ModelRenderer.h"

namespace Visualization {

// We need a default that can be constructed. The default is never used.
template <class ParentType, class VisualizationType = Text>
class VISUALIZATIONBASE_API ItemWrapperElement : public Element
{
	public: // Methods executable on element definition
		using ChildItem = Item* ParentType::*;
		using ChildStyle = const typename VisualizationType::StyleType*;
		using GetNodeFunction = std::function<Model::Node* (ParentType* v)>;
		ItemWrapperElement(ChildItem item, ChildStyle style);
		ItemWrapperElement(ChildItem item, GetNodeFunction nodeGetter);

	public: // Methods executable when items need to be rendered
		virtual void synchronizeWithItem(Item* item) override;

	private:
		ChildItem item_{};
		GetNodeFunction nodeGetter_{};
		ChildStyle style_{};
};

template <class ParentType, class VisualizationType>
ItemWrapperElement<ParentType, VisualizationType>::ItemWrapperElement(ChildItem item, ChildStyle style)
: item_{item}, style_{style}
{}

template <class ParentType, class VisualizationType>
ItemWrapperElement<ParentType, VisualizationType>::ItemWrapperElement(ChildItem item, GetNodeFunction nodeGetter)
: item_{item}, nodeGetter_{nodeGetter}
{}

template <class ParentType, class VisualizationType>
void ItemWrapperElement<ParentType, VisualizationType>::synchronizeWithItem(Item* item)
{
	// TODO: more special cases (node, ...)
	if(!nodeGetter_ && style_)
	{
		auto& childItem = (static_cast<ParentType*>(item))->*item_;
		if(!childItem) childItem = new VisualizationType(item, style_);
		childItem->setStyle(style_);
	}
	else if(!style_ && nodeGetter_)
	{
		auto& childItem = (static_cast<ParentType*>(item))->*item_;
		auto node = nodeGetter_(static_cast<ParentType*>(item));

		if(childItem && childItem->node() != node)
		{
			SAFE_DELETE_ITEM(childItem);
			item->setUpdateNeeded(Item::StandardUpdate);
		}

		if(!childItem)
		{
			childItem = item->renderer()->render(item, node);
			item->setUpdateNeeded(Item::StandardUpdate);
		}
	}
}

}

#endif /* VisualizationBase_ITEMWRAPPERELEMENT_H_ */
