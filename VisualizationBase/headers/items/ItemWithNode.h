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
#include "../ModelRenderer.h"

namespace Visualization {

class Item;

template <class Super, class ContainedNode = Model::Node>
class ItemWithNode : public Super
{
	public:
		typedef ContainedNode NodeType;
		ItemWithNode(Item* parent, ContainedNode* node, const typename Super::StyleType* style = NULL);

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
}

template <class Super, class ContainedNode> bool ItemWithNode<Super,ContainedNode>::hasNode() const { return true; }
template <class Super, class ContainedNode> ContainedNode* ItemWithNode<Super,ContainedNode>::node() const { return node_; }
template <class Super, class ContainedNode> int ItemWithNode<Super,ContainedNode>::revision() const { return revision_; }
template <class Super, class ContainedNode> void ItemWithNode<Super,ContainedNode>::setRevision(int newRevision) { revision_ = newRevision; }

}

#endif /* ITEMWITHNODE_H_ */
