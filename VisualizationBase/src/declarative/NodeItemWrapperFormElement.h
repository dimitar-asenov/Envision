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

#include "ModelBase/src/nodes/Node.h"
#include "../renderer/ModelRenderer.h"

namespace Visualization {

/**
 * This is an item wrapper element, with a specifiable node to create the item with.
 */
template <class ParentType>
class NodeItemWrapperFormElement : public ItemWrapperFormElement<ParentType> {
		FLUENT_ELEMENT_INTERFACE(NodeItemWrapperFormElement);

	public:
		using ChildItem = typename ItemWrapperFormElement<ParentType>::ChildItem;
		using GetNodeFunction = std::function<Model::Node* (ParentType* v)>;

		NodeItemWrapperFormElement(ChildItem item, GetNodeFunction nodeGetter);
		virtual ~NodeItemWrapperFormElement() {};
		virtual void synchronizeWithItem(Item* item) override;

		/**
		 * Sets if a wrapped item is created, even if there is no node to \a create. It is false by default.
		 */
		NodeItemWrapperFormElement<ParentType>* setCreateIfNoNode(bool create);

	private:
		GetNodeFunction nodeGetter_{};
		bool createIfNoNode_{false};
};

template <class ParentType>
NodeItemWrapperFormElement<ParentType>::NodeItemWrapperFormElement(ChildItem item, GetNodeFunction nodeGetter)
: ItemWrapperFormElement<ParentType>{item}, nodeGetter_{nodeGetter}
{}

template <class ParentType>
void NodeItemWrapperFormElement<ParentType>::synchronizeWithItem(Item* item)
{
	auto& childItem = (static_cast<ParentType*>(item))->*this->item();
	auto node = nodeGetter_(static_cast<ParentType*>(item));

	if(childItem && childItem->node() != node)
	{
		SAFE_DELETE_ITEM(childItem);
		item->setUpdateNeeded(Item::StandardUpdate);
	}

	if(!childItem && (node || createIfNoNode_))
	{
		childItem = item->renderer()->render(item, node);
		item->setUpdateNeeded(Item::StandardUpdate);
	}
}

template <class ParentType>
inline NodeItemWrapperFormElement<ParentType>* NodeItemWrapperFormElement<ParentType>::setCreateIfNoNode(bool create)
{
	createIfNoNode_ = create;
	return this;
}

} /* namespace Visualization */
