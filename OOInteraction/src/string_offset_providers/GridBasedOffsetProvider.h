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

#pragma once

#include "../oointeraction_api.h"
#include "StringOffsetProvider.h"

namespace OOInteraction {

class Cell;

class OOINTERACTION_API GridBasedOffsetProvider : public StringOffsetProvider {
	public:
		GridBasedOffsetProvider(Visualization::Item* vis);
		~GridBasedOffsetProvider();

		virtual int offset(Qt::Key key) override;
		virtual void setOffset(int newOffset) override;
		virtual bool isIndivisible() override;

		/**
		 * Adds the cell \a cell to the grid of this string provider.
		 *
		 * Every time a new cell is added the size of this provider will be adjusted if necessary to accommodate the new
		 * cell.
		 */
		void add(Cell* cell);

		/**
		 * Sets the size of this grid string provider. It is only necessary to call this method if padding is required.
		 * Otherwise simply using add() is sufficient.
		 */
		void setSize(int width, int height);

		template <class T> static void addGridConstructor(void(*constructor)(GridBasedOffsetProvider* provider, T* item));
		static bool hasGridConstructorFor(Visualization::Item* item);

		void setFilterNullAndEmptyComponents();
		void setIsIndivisible(bool isIndivisible = true);

	protected:
		virtual QStringList components() override;

	private:
		QList<Cell*> cells_;
		QSize size_;
		bool isIndivisible_{};
		bool filterNullAndEmptyComponents_{};

		enum Direction {Left, Up, Right, Down};
		Cell* findCell(const QRect& start, Direction dir) const;

		bool isOnTop(Cell* cell) const;
		bool isOnLeft(Cell* cell) const;
		bool isOnBottom(Cell* cell) const;
		bool isOnRight(Cell* cell) const;

		using ItemTypeRegistrationFunction = void (*)(GridBasedOffsetProvider* provider, Visualization::Item* item);
		static QMap<int, ItemTypeRegistrationFunction>& gridConstructors();
};

template <class T>
inline void GridBasedOffsetProvider::addGridConstructor(void(*constructor)(GridBasedOffsetProvider* provider, T* item))
{
	gridConstructors().insert(T::staticTypeId(), reinterpret_cast<ItemTypeRegistrationFunction>(constructor) );
}

inline void GridBasedOffsetProvider::setFilterNullAndEmptyComponents() { filterNullAndEmptyComponents_ = true;}
inline void GridBasedOffsetProvider::setSize(int width, int height) { size_.setWidth(width); size_.setHeight(height); }
inline void GridBasedOffsetProvider::setIsIndivisible(bool isIndivisible) { isIndivisible_ = isIndivisible;}

} /* namespace OOInteraction */
