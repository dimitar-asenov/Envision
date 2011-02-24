/***********************************************************************************************************************
 * VListCF.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/VListCF.h"

using namespace Visualization;
using namespace Model;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS( VListCF )

VListCF::VListCF(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode< ControlFlowItem, List>(parent, node, style)
{

}

VListCF::~VListCF()
{
	for (int i = 0; i<items_.size(); i++) SAFE_DELETE_ITEM( items_[i]);
}

bool VListCF::isEmpty() const
{
	return items_.empty();
}

void VListCF::determineChildren()
{
	//TODO this
	// Inserts elements that are not yet visualized and adjusts the order to match that in 'nodes'.
	for (int i = 0; i < node()->size(); ++i)
	{
		if (i >= lengthSingle() ) appendSingle( renderer->render(NULL, node()->at<Node>(i)));	// This node is new
		else if ( atSingle(i)->node() == node()->at<Node>(i) )	continue;	// This node is already there
		else
		{
			// This node might appear somewhere ahead, we should look for it
			bool found = false;
			for (int k = i + 1; k<lengthSingle(); ++k)
			{
				if ( atSingle(k)->node() == node()->at<Node>(i) )
				{
					// We found this node, swap the visualizations
					swapSingle(i, k);
					found = true;
					break;
				}
			}

			// The node was not found, insert a visualization here
			if (!found ) insertSingle( renderer->render(NULL, node()->at<Node>(i)), i);
		}
	}

	// Remove excess items
	while (lengthSingle() > node()->size()) removeLastSingle();
}

void VListCF::updateGeometry(int, int)
{
	breaks_.clear();
	continues_.clear();

	//TODO this
}

void VListCF::appendSingle( Visualization::Item* item)
{
	insertSingle(item, lengthSingle() );
}

void VListCF::insertSingle( Visualization::Item* item, int pos);
void VListCF::swapSingle(int i, int j);
void VListCF::removeLastSingle();

Visualization::Item* VListCF::atSingle(int pos)
{
	int index = pos;

	for (int i = 0; i<items_.size(); ++i)
	{
		SequentialLayout* seq = dynamic_cast<SequentialLayout*> (items_[i]);
		if (seq)
		{
			if (index < seq->length()) return seq->at<Item>(index);
			else index -= seq->length();
		}
		else
		{
			if (index == 0) return items_[i];
			else --index;
		}
	}

	return NULL;
}

int VListCF::lengthSingle()
{
	int len = 0;
	for (int i = 0; i<items_.size(); ++i)
	{
		SequentialLayout* seq = dynamic_cast<SequentialLayout*> (items_[i]);
		if (seq) len += seq->length(); // This is a sequnce of Items which are not ControlFlowItems.
		else ++len; // This is a single ControlFlowItem
	}
}

bool VListCF::focusChild(FocusTarget /*location*/)
{
	//TODO implement this
	return false;
}


}
