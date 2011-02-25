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
	// Get the single items from the list of composite items.
	QList< Item* > items = extractSingleItems();

	// Inserts elements that are not yet visualized and adjusts the order to match that in 'nodes'.
	for (int i = 0; i < node()->size(); ++i)
	{
		if (i >= items.size() ) items.append( renderer()->render(NULL, node()->at<Node>(i)));	// This node is new
		else if ( items[i]->node() == node()->at<Node>(i) )	continue;	// This node is already there
		else
		{
			// This node might appear somewhere ahead, we should look for it
			bool found = false;
			for (int k = i + 1; k<items.size(); ++k)
			{
				if ( items[k]->node() == node()->at<Node>(i) )
				{
					// We found this node, swap the visualizations
					items.swap(i, k);
					found = true;
					break;
				}
			}

			// The node was not found, insert a visualization here
			if (!found ) items.insert(i, renderer()->render(NULL, node()->at<Node>(i)));
		}
	}

	// Remove excess items
	while (items.size() > node()->size()) items.removeLast();

	// Convert the single items list back to a list of composite items.
	buildCompositeItems(items);
}

void VListCF::updateGeometry(int, int)
{
	breaks_.clear();
	continues_.clear();

	QList< QPoint > pos;
	for(int i = 0; i<items_.size(); ++i) pos.append( QPoint() );

	QPoint location;
	QPoint topLeft;
	QPoint bottomRight;

	// Begin placing the elements in a virtual plane.
	for(int i = 0; i < items_.size(); ++i)
	{
		ControlFlowItem* cfi = dynamic_cast<ControlFlowItem*> (items_[i]);
		if (cfi)
		{
			pos[i] = QPoint( location.x() - cfi->entrance().x(), location.y());
			if (cfi->exit().isNull()) exit_ = QPoint(0,0);
			else
			{
				location.rx() += cfi->exit().x() - cfi->entrance().x();
				exit_ = location;
			}


			// Process breaks and continues
			for (int k = 0; k < cfi->breaks().size(); ++k)
			{
				QPoint br( cfi->breaks().at(k) + pos[i] );
				if ( cfi->breaks().at(k).x() == 0) br.setX(0);
				else br.setX(1);
				breaks_.append( br);
			}
			for (int k = 0; k < cfi->continues().size(); ++k)
			{
				QPoint cnt( cfi->continues().at(k) + pos[i] );
				if ( cfi->continues().at(k).x() == 0) cnt.setX(0);
				else cnt.setX(1);
				continues_.append( cnt );
			}
		}
		else
		{
			location.ry() += style()->pinLength(); // There is a pin on top.
			pos[i] = QPoint( location.x() - items_[i]->width()/2, location.y());
			location.ry() += style()->pinLength(); // There is a pin on the bottom.
			exit_ = location;
		}

		int pinSpace = cfi ? 0 : style()->pinLength();
		if ( pos[i].x() - pinSpace < topLeft.x()) topLeft.setX( pos[i].x() - pinSpace );
		if ( pos[i].x() + items_[i]->width() + pinSpace > bottomRight.y())  bottomRight.setY(pos[i].x() + items_[i]->width() + pinSpace);

		location.ry() += items_[i]->height();
	}

	bottomRight.setY(location.y());

	// Set the size
	if ( hasShape() ) getShape()->setInnerSize(bottomRight.x() - topLeft.x(), bottomRight.y());
	else setSize(bottomRight.x() - topLeft.x(), bottomRight.y());

	// Set the positions of all elements
	for (int i = 0; i < items_.size(); ++i) items_[i]->setPos(pos[i] - topLeft);
}

QList< Item* > VListCF::extractSingleItems()
{
	QList< Item* > result;

	for (int i = items_.size() - 1; i>=0; --i)
	{
		SequentialLayout* seq = dynamic_cast<SequentialLayout*> (items_[i]);
		if (seq)
		{
			for (int k = seq->length()-1; k>=0; --k) result.prepend(seq->at<Item>(k));
			seq->clear(false);
			SAFE_DELETE_ITEM(seq);
		}
		else result.prepend(items_[i]);
	}

	items_.clear();
	return result;
}

void VListCF::buildCompositeItems( QList< Item* >& singleItems )
{
	SequentialLayout* seq = NULL;

	for(int i = 0; i<singleItems.size(); ++i)
	{
		ControlFlowItem* cfi = dynamic_cast<ControlFlowItem*> (singleItems[i]);
		if (cfi)
		{
			if (seq) seq = NULL;

			cfi->setParentItem(this);
			items_.append(cfi);
			cfi->setPreferredExit( preferredExit_ );
		}
		else
		{
			if (!seq)
			{
				seq = new SequentialLayout(this, &style()->sequence());
				items_.append(seq);
			}

			seq->append( singleItems[i]);
		}
	}
}

bool VListCF::focusChild(FocusTarget /*location*/)
{
	//TODO implement this
	return false;
}


}
