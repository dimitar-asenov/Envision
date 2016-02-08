/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "VListCF.h"

using namespace Visualization;
using namespace Model;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS( VListCF, "item" )

VListCF::VListCF(Item* parent, NodeType* node, const StyleType* style) :
	Super{parent, node, style}
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
		if (i >= items.size() ) items.append( renderer()->render(this, node()->at<Node>(i)));	// This node is new
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
			if (!found ) items.insert(i, renderer()->render(this, node()->at<Node>(i)));
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
	clearConnectors();

	if (items_.isEmpty())
	{
		if ( hasShape() ) getShape()->setInnerSize(style()->pinLength(), style()->pinLength());
			else setSize(style()->pinLength(), style()->pinLength());

		entrance_ = QPoint{style()->pinLength()/2, 0};
		exit_ = QPoint{style()->pinLength()/2, style()->pinLength()};
		addConnector(entrance_, exit_, false);
		return;
	}

	QList< QPoint > pos;
	for (int i = 0; i<items_.size(); ++i) pos.append( QPoint{} );

	QPoint location;
	QPoint topLeft;
	QPoint bottomRight;

	// Begin placing the elements in a virtual plane.
	for (int i = 0; i < items_.size(); ++i)
	{
		ControlFlowItem* cfi = DCast<ControlFlowItem> (items_[i]);
		if (cfi)
		{
			pos[i] = QPoint{ location.x() - cfi->entrance().x(), location.y()};
			location.ry() += items_[i]->heightInParent();

			if (cfi->exit().isNull()) exit_ = QPoint{0, 0};
			else
			{
				location.rx() += cfi->exit().x() - cfi->entrance().x();
				exit_ = location;
			}


			// Process breaks and continues
			for (int k = 0; k < cfi->breaks().size(); ++k)
			{
				QPoint br{ cfi->breaks().at(k) + pos[i] };
				if ( cfi->breaks().at(k).x() == 0) br.setX(0);
				else br.setX(1);
				breaks_.append( br);
			}
			for (int k = 0; k < cfi->continues().size(); ++k)
			{
				QPoint cnt{ cfi->continues().at(k) + pos[i] };
				if ( cfi->continues().at(k).x() == 0) cnt.setX(0);
				else cnt.setX(1);
				continues_.append( cnt );
			}
		}
		else
		{
			location.ry() += style()->pinLength(); // There is a pin on top.
			pos[i] = QPoint{ location.x() - items_[i]->widthInParent()/2, location.y()};
			location.ry() += items_[i]->heightInParent();
			location.ry() += style()->pinLength(); // There is a pin on the bottom.
			exit_ = location;
		}

		int pinSpace = cfi ? 0 : style()->pinLength();
		if ( pos[i].x() - pinSpace < topLeft.x()) topLeft.setX( pos[i].x() - pinSpace );
		if ( pos[i].x() + items_[i]->widthInParent() + pinSpace > bottomRight.x())
			bottomRight.setX(pos[i].x() + items_[i]->widthInParent() + pinSpace);
	}

	bottomRight.setY(location.y());

	// Set the size
	if ( hasShape() ) getShape()->setInnerSize(bottomRight.x() - topLeft.x(), bottomRight.y());
	else setSize(bottomRight.x() - topLeft.x(), bottomRight.y());

	// Set entrance and exit
	entrance_ = -topLeft;
	if (!exit_.isNull()) exit_ = QPoint{ exit_.x() -topLeft.x(), bottomRight.y()};

	// Set the positions of all elements and create the connectors
	for (int i = 0; i < items_.size(); ++i)
	{
		// Set position
		pos[i] -= topLeft;
		items_[i]->setPos(pos[i]);

		ControlFlowItem* cfi = DCast<ControlFlowItem> (items_[i]);
		if (cfi)
		{
			// Add connectors for breaks and continues
			for (int k = 0; k < cfi->breaks().size(); ++k)
			{
				QPoint br{ cfi->breaks().at(k) + pos[i] };
				if ( cfi->breaks().at(k).x() == 0) addConnector(br, QPoint{0, br.y()}, false);
				else
				{
					br.setX(pos[i].x() + items_[i]->widthInParent());
					addConnector(br, QPoint{widthInLocal(), br.y()}, false);
				}
			}
			for (int k = 0; k < cfi->continues().size(); ++k)
			{
				QPoint cont{ cfi->continues().at(k) + pos[i] };
				if ( cfi->continues().at(k).x() == 0) addConnector(cont, QPoint{0, cont.y()}, false);
				else
				{
					cont.setX(pos[i].x() + items_[i]->widthInParent());
					addConnector(cont, QPoint{widthInLocal(), cont.y()}, false);
				}
			}
		}
		else
		{
			int midPoint = pos[i].x() + items_[i]->widthInParent()/2;
			addConnector(midPoint, pos[i].y() - style()->pinLength(), midPoint, pos[i].y(), true);
			addConnector(midPoint, pos[i].y() + items_[i]->heightInParent(), midPoint,
					pos[i].y() + items_[i]->heightInParent() + style()->pinLength(), false);
		}
	}
}

QList< Item* > VListCF::extractSingleItems()
{
	QList< Item* > result;

	for (int i = items_.size() - 1; i>=0; --i)
	{
		SequentialLayout* seq = DCast<SequentialLayout> (items_[i]);
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
	SequentialLayout* seq = nullptr;

	for (int i = 0; i<singleItems.size(); ++i)
	{
		ControlFlowItem* cfi = DCast<ControlFlowItem> (singleItems[i]);
		if (cfi)
		{
			if (seq) seq = nullptr;

			cfi->setParentItem(this);
			items_.append(cfi);
			cfi->setPreferredContinueExit( preferredContinueExit_ );
			cfi->setPreferredBreakExit( preferredBreakExit_ );
		}
		else
		{
			if (!seq)
			{
				seq = new SequentialLayout{this, &style()->sequence()};
				items_.append(seq);
			}

			seq->append( singleItems[i]);
		}
	}
}


}
