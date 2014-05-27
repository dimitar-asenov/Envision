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

#pragma once

#include "../visualizationbase_api.h"

namespace Visualization {

/**
 * The possible directions in which an item can expand the area it uses.
 *
 * This is used in the arrangement algorithm and only used in the QList
 * ArrangementAlgorithmItem::expandingDirections.
 */
enum ExpandingDirection {LeftUp, RightUp, RightDown, LeftDown};

class Item;

/**
 * This struct groups all information used by the arrangement algorithm associated with an item.
 *
 * It is supposed to be used only by the arrangement algorithm inside PositionLayout.
 */
struct VISUALIZATIONBASE_API ArrangementAlgorithmItem
{
	/**
	 * We use a QVector of ArrangmentAlgorithmItems in the arrangement algorithm. Therefore this
	 * struct has to have a default constructor.
	 */
	ArrangementAlgorithmItem();

	/**
	 * A pointer to the item associated with this ArrangmentAlgorithmItem.
	 */
	Item* item;

	/**
	 * The calculated scale of the item.
	 */
	qreal scale;

	/**
	 * The calculated area of the item.
	 */
	QRectF area;

	/**
	 * A queue of the directions the item can still try to expand to.
	 */
	QList<ExpandingDirection> expandingDirections;

	/**
	 * Stores the width-height-ratio of the original item area.
	 *
	 * This field is used for optimization only.
	 */
	qreal widthHeightRatio;

	/**
	 * Associates an Item to this ArrangementAlgorithmItem object.
	 *
	 * This class is only used by the arrangement algorithm inside PositionLayout. There it is used as a type parameter
	 * of a QVector and therefore we require it to have a parameterless constructor. To prevent reinitialization this
	 * method is called after the default construction through QVector to actually associate an item with this
	 * ArrangementAlgorithmItem object.
	 */
	void setItem(Item* item);

	/**
	 * Expands the area associated to this ArrangementAlgorithmItem.
	 */
	void expandArea(qreal left, qreal up, qreal right, qreal down);

	/**
	 * Shrinks the area associated to this ArrangementAlgorithmItem.
	 */
	void shrinkArea(qreal left, qreal up, qreal right, qreal down);

	/**
	 * Returns whether the receiver ArrangementAlgorithmItem collides with any of the others.

	 * If the receiver object is contained in others it will be ignored during the calculation.
	 */
	bool collidesWithAny(QVector<ArrangementAlgorithmItem>& others);
};

inline void ArrangementAlgorithmItem::expandArea(qreal left, qreal up, qreal right, qreal down)
{
	area.setLeft(area.left() - left);
	area.setTop(area.top() - up);
	area.setRight(area.right() + right);
	area.setBottom(area.bottom() + down);
}

inline void ArrangementAlgorithmItem::shrinkArea(qreal left, qreal up, qreal right, qreal down)
{
	expandArea(-left, -up, -right, -down);
}

}
