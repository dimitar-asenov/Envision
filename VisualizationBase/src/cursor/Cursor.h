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

class CursorData;
class Item;

class VISUALIZATIONBASE_API Cursor {
	public:

		enum CursorType {VerticalCursor, HorizontalCursor, BoxCursor};

		Cursor(Item* owner, CursorType type, Item* visualization = nullptr);
		Cursor(const Cursor& other);
		Cursor& operator= (const Cursor& other) = delete;
		virtual ~Cursor();
		virtual Cursor* clone() const;

		/**
		 * Called on every scene update to update the cursor.
		 *
		 * The default implementation checks if this is a BoxCursor and if so it assumes it is a whole item cursor to
		 * update its position and size. Otherwise it does nothing.
		 *
		 * Reimplement this in derived classes to update the cursor's position and size appropriately.
		 */
		virtual void update();

		virtual Item* owner() const;

		/**
		 * Returns the position of this cursor in corrdinates local to owner()
		 */
		const QPoint& position();

		/**
		 * Returns the region of this cursor in corrdinates local to owner()
		 */
		const QRect& region();

		/**
		 * Returns the region of this cursor in scene coordinates
		 */
		QRect sceneRegion();

		Item* visualization();

		void setPosition(const QPoint& pos);
		void setRegion(const QRect& region);

		/**
		 * \brief Returns true if this cursor is the same as the cursor \a other.
		 *
		 * Two cursors are the same if they have the same owner and if they represent the same location within that owner.
		 */
		virtual bool isSame(Cursor* other);

		/**
		 * \brief Returns true if this cursor is equivalent with respect to its location to the cursor \a c.
		 *
		 * Here are the conditions for two cursors to be location equivalent:
		 * - None of the cursors is set to be not location equivalent.
		 * - The type of both cursors is the same. Furthermore the type can not be BoxCursor.
		 * - The owners of the two cursors are in a child-parent relationship. Two cursors from peer entities are not
		 *   location equivalent. Two cursors from the same owner can not be location equivalent
		 * - At least one of the cursors is a boundary cursor.
		 * - The inner item's style allows for cursors equivalent through a boundary.
		 */
		virtual bool isLocationEquivalent(Cursor* c);
		virtual bool isLocationEquivalent(bool otherNotLocationEquivalent, CursorType otherType, bool otherIsAtBoundary,
													 Item* otherOwner);

		bool notLocationEquivalent() const;
		void setNotLocationEquivalent(bool notEquivalent);

		virtual bool isAtBoundary() const;

		CursorType type() const;

	protected:
		void setVisualization(Item* visualization);

	private:
		QPoint position_;
		QRect region_;
		Item* owner_{};
		Item* visualization_{};
		CursorType type_;
		bool notLocationEquivalent_;

		bool allowEquivalentCursorsAcrossBoundaries(Item* parent, Item* child);
};

inline void Cursor::setPosition(const QPoint& pos) { position_ = pos; }
inline void Cursor::setRegion(const QRect& region) { region_ = region; }
inline const QPoint& Cursor::position() { return position_; }
inline const QRect& Cursor::region() { return region_; }
inline Item* Cursor::visualization() { return visualization_; }
inline Cursor::CursorType Cursor::type() const {return type_;}

inline bool Cursor::notLocationEquivalent() const { return notLocationEquivalent_;}
inline void Cursor::setNotLocationEquivalent(bool notEquivalent) { notLocationEquivalent_ = notEquivalent; }

}
