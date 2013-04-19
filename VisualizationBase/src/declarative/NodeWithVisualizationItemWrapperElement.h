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

#include "ItemWrapperElement.h"

namespace Visualization {

/**
 * This is an item wrapper element, with a specifiable style for the wrapped item, and a node to create the item with.
 */
template <class ParentType, class VisualizationType>
class NodeWithVisualizationItemWrapperElement : public ItemWrapperElement<ParentType, VisualizationType> {
		FLUENT_ELEMENT_INTERFACE(NodeWithVisualizationItemWrapperElement);

	public:
		using ChildItem = typename ItemWrapperElement<ParentType, VisualizationType>::ChildItem;
		using GetStyleTypeFunction = std::function<const typename VisualizationType::StyleType* (ParentType* v)>;
		using GetNodeTypeFunction = std::function<typename VisualizationType::NodeType* (ParentType* v)>;

		NodeWithVisualizationItemWrapperElement(ChildItem item, GetNodeTypeFunction nodeGetter,
																GetStyleTypeFunction styleGetter);
		virtual ~NodeWithVisualizationItemWrapperElement() {};
		virtual void synchronizeWithItem(Item* item) override;

		/**
		 * Sets if a wrapped item is created, even if there is no node to \a create. It is false by default.
		 */
		NodeWithVisualizationItemWrapperElement<ParentType,VisualizationType>* setCreateIfNoNode(bool create);

	private:
		GetNodeTypeFunction nodeGetter_;
		GetStyleTypeFunction styleGetter_;
		bool createIfNoNode_{false};
};

template <class ParentType, class VisualizationType>
NodeWithVisualizationItemWrapperElement<ParentType, VisualizationType>::NodeWithVisualizationItemWrapperElement(
		ChildItem item, GetNodeTypeFunction nodeGetter, GetStyleTypeFunction styleGetter)
: ItemWrapperElement<ParentType, VisualizationType>{item}, nodeGetter_{nodeGetter}, styleGetter_{styleGetter}
{}

template <class ParentType, class VisualizationType>
void NodeWithVisualizationItemWrapperElement<ParentType, VisualizationType>::synchronizeWithItem(Item* item)
{
	auto& childItem = (static_cast<ParentType*>(item))->*this->item();
	auto node = nodeGetter_(static_cast<ParentType*>(item));
	auto style = styleGetter_(static_cast<ParentType*>(item));

	if (childItem && childItem->node() != node )
	{
		SAFE_DELETE_ITEM(childItem);
		item->setUpdateNeeded(Item::StandardUpdate);
	}

	if (!childItem && (node || createIfNoNode_))
	{
		if (style) childItem = new VisualizationType(item, node, style);
		else childItem = new VisualizationType(item, node);

		item->setUpdateNeeded(Item::StandardUpdate);
	}

	if(childItem) childItem->setStyle(style);
}

template <class ParentType, class VisualizationType>
inline NodeWithVisualizationItemWrapperElement<ParentType, VisualizationType>*
NodeWithVisualizationItemWrapperElement<ParentType, VisualizationType>::setCreateIfNoNode(bool create)
{
	createIfNoNode_ = create;
	return this;
}

} /* namespace Visualization */
