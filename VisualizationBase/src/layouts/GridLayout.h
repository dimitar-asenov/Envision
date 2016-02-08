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

#include "Layout.h"
#include "GridLayoutStyle.h"

namespace Model { class Node; }

namespace Visualization {

class ModelRenderer;

class VISUALIZATIONBASE_API GridLayout: public Super<Layout>
{
	ITEM_COMMON(GridLayout)

	public:
		GridLayout(Item* parent, const StyleType* style = itemStyles().get());
		~GridLayout();

		virtual bool isEmpty() const override;

		virtual void updateGeometry(int availableWidth, int availableHeight) override;
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

		QPoint focusedElementIndex() const;

		void setGridSize(int sizeX, int sizeY, bool deleteExtraItems);
		QSize gridSize() const;

		void set(Item* item, int x, int y, bool deleteOldItem);
		bool isEmpty(int x, int y) const;
		void swap(int x1, int y1, int x2, int y2);

		void remove(int x, int y, bool deleteItem = true);
		void remove(Item* item, bool deleteItem = true);
		void clear(bool deleteItems = true);

		template <typename T> T* at(int x, int y);
		template <typename T> T* at(int x, int y) const;

		/**
		 * Updates the content of the grid to match 'nodes'.
		 *
		 * The elements of the grid are updated so that they contain visualizations of the provided nodes. The grid will
		 * be resized if necessary.
		 *
		 * @param nodes
		 * 				A list of rows. The first row in the list is the topmost one. Each row is a list of nodes starting
		 * 				from the leftmost node.
		 */
		void synchronizeWithNodes(const QList< QList<Model::Node*> >& nodes);

	private:
		QVector< QVector<Item*> > items_;
		int sizeX_;
		int sizeY_;

		Item* findFirstVertical(bool startFromTop);
		Item* findFirstHorizontal(bool startFromLeft);
		Item* findNext(int dx, int dy);
};

inline QSize GridLayout::gridSize() const { return QSize{sizeX_, sizeY_}; }

template <typename T> inline T* GridLayout::at(int x, int y) { return static_cast<T*> (items_[x][y]); }
template <typename T> inline T* GridLayout::at(int x, int y) const { return static_cast<T*> (items_[x][y]); }
}
