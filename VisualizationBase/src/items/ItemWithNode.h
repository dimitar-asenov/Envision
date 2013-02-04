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

/***********************************************************************************************************************
 * ItemWithNode.h
 *
 *  Created on: Feb 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ITEMWITHNODE_H_
#define ITEMWITHNODE_H_

// This is included for convenience. There is a very big chance that whoever uses ItemWithNode will want to use the
// ModelRenderer too.
#include "../renderer/ModelRenderer.h"

namespace Visualization {

class Item;

template <class Super, class ContainedNode = Model::Node>
class ItemWithNode : public Super
{
	public:
		typedef ContainedNode NodeType;
		ItemWithNode(Item* parent, ContainedNode* node, const typename Super::StyleType* style = nullptr);
		~ItemWithNode();

		virtual bool hasNode() const;
		virtual ContainedNode* node() const;
		virtual int revision() const;
		virtual void setRevision(int newRevision);

	private:
		ContainedNode* node_;
		int revision_;
};

template <class Super, class ContainedNode> ItemWithNode<Super,ContainedNode>::ItemWithNode(Item* parent, ContainedNode* node, const typename Super::StyleType* style)
	: Super(parent, style), node_(node), revision_(-1)
{
	Super::nodeItemsMap().insert(node,this);
}

template <class Super, class ContainedNode> ItemWithNode<Super,ContainedNode>::~ItemWithNode()
{
	auto removed = Super::nodeItemsMap().remove(node_,this);
	Q_ASSERT(removed == 1);
}

template <class Super, class ContainedNode> bool ItemWithNode<Super,ContainedNode>::hasNode() const { return true; }
template <class Super, class ContainedNode> ContainedNode* ItemWithNode<Super,ContainedNode>::node() const { return node_; }
template <class Super, class ContainedNode> int ItemWithNode<Super,ContainedNode>::revision() const { return revision_; }
template <class Super, class ContainedNode> void ItemWithNode<Super,ContainedNode>::setRevision(int newRevision) { revision_ = newRevision; }

}

#endif /* ITEMWITHNODE_H_ */
