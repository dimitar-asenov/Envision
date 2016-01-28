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

namespace Visualization {

class Item;
class Cursor;

class VISUALIZATIONBASE_API ItemRegion {
	public:
		ItemRegion(const QRect& region = QRect());
		virtual ~ItemRegion();

		void setRegion(const QRect& region);
		void setItem(Item* item);
		void setCursor(Cursor* cursor);

		QRect& region();
		Item* item() const;
		Cursor* cursor() const;

		qreal distanceTo(const QPointF& point);

		enum PositionConstraint {
			NoConstraints = 0x0,
			Below = 0x1,
			Above = 0x2,
			LeftOf = 0x4,
			RightOf = 0x8,
			Overlap = 0x10
		};
		using PositionConstraints = QFlags<PositionConstraint>;

		/**
		 * \brief Returns all position constraints with respect to the point \a point satisfied by this region.
		 *
		 * The returned constraints are from the region's point of view, e.g. a constraint \a LeftOf means that the region
		 * is left of the specified point.
		 */
		PositionConstraints satisfiedPositionConstraints(const QPoint& point) const;

	private:
		QRect region_;
		Item* item_;
		Cursor* cursor_;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ItemRegion::PositionConstraints)

inline void ItemRegion::setRegion(const QRect& region) { region_ = region; }
inline void ItemRegion::setItem(Item* item) { item_ = item; }
inline void ItemRegion::setCursor(Cursor* cursor) { cursor_ = cursor; }
inline QRect& ItemRegion::region() { return region_; }
inline Item* ItemRegion::item() const { return item_; }
inline Cursor* ItemRegion::cursor() const { return cursor_; }

}
