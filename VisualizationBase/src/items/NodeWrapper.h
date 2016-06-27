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

#include "Item.h"
#include "ItemWithNode.h"
#include "ItemMacros.h"
#include "ItemStyle.h"

#include "ModelBase/src/nodes/Node.h"

namespace Visualization {

/**
 * This class can be used to display nodes with their default style, but wrapped in an item, which can be given
 * an ItemStyle.
 * This is useful, e.g. to give the node a background shape.
 */
class VISUALIZATIONBASE_API NodeWrapper : public Super<ItemWithNode<NodeWrapper, Item, Model::Node, false>>
{
	ITEM_COMMON_CUSTOM_STYLENAME(NodeWrapper, ItemStyle)

	public:
		NodeWrapper(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~NodeWrapper();

		/**
		 * Sets if the item is a special \a stretchable item. This means if its node is nullptr, it is not empty and
		 * stretches as far as it has space to.
		 */
		void setStretchable(bool stretchable);
		virtual bool isEmpty() const override;
		virtual bool sizeDependsOnParent() const override;
		// TODO: should not need to redefine this
		virtual bool hasNode() const override;

		Item* wrappedItem() const;
		void refreshWrappedItem();

	protected:
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;


	private:
		Item* wrappedItem_{};
		bool stretchable_{false};
};

inline void NodeWrapper::setStretchable(bool stretchable) {stretchable_ = stretchable;}
inline Item* NodeWrapper::wrappedItem() const { return wrappedItem_; }

}
