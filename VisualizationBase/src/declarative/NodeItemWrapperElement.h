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

#ifndef VisualizationBase_NODEITEMWRAPPERELEMENT_H_
#define VisualizationBase_NODEITEMWRAPPERELEMENT_H_

#include "ItemWrapperElement.h"

#include "ModelBase/src/nodes/Node.h"
#include "../renderer/ModelRenderer.h"

namespace Visualization {

template <class ParentType>
class NodeItemWrapperElement : public ItemWrapperElement<ParentType> {
		FLUENT_ELEMENT_INTERFACE(NodeItemWrapperElement);

	public:
		using ChildItem = typename ItemWrapperElement<ParentType>::ChildItem;
		using GetNodeFunction = std::function<Model::Node* (ParentType* v)>;

		NodeItemWrapperElement(ChildItem item, GetNodeFunction nodeGetter);
		virtual ~NodeItemWrapperElement() {};
		virtual void synchronizeWithItem(Item* item) override;

	private:
		GetNodeFunction nodeGetter_{};
};

template <class ParentType>
NodeItemWrapperElement<ParentType>::NodeItemWrapperElement(ChildItem item, GetNodeFunction nodeGetter)
: ItemWrapperElement<ParentType>{item}, nodeGetter_{nodeGetter}
{}

template <class ParentType>
void NodeItemWrapperElement<ParentType>::synchronizeWithItem(Item* item)
{
	auto& childItem = (static_cast<ParentType*>(item))->*this->item();
	auto node = nodeGetter_(static_cast<ParentType*>(item));

	if(childItem && childItem->node() != node)
	{
		SAFE_DELETE_ITEM(childItem);
		item->setUpdateNeeded(Item::StandardUpdate);
	}

	if(!childItem && node)
	{
		childItem = item->renderer()->render(item, node);
		item->setUpdateNeeded(Item::StandardUpdate);
	}
}

} /* namespace Visualization */
#endif /* VisualizationBase_NODEITEMWRAPPERELEMENT_H_ */
