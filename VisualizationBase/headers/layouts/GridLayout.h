/***********************************************************************************************************************
 * GridLayout.h
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef GRIDLAYOUT_H_
#define GRIDLAYOUT_H_

#include "../visualizationbase_api.h"

#include "Layout.h"
#include "GridLayoutStyle.h"

#include <QtCore/QList>

namespace Model { class Node; }

namespace Visualization {

class ModelRenderer;

class VISUALIZATIONBASE_API GridLayout: public Layout
{
	ITEM_COMMON(GridLayout)

	public:
		GridLayout(Item* parent, const StyleType* style = itemStyles().get());
		~GridLayout();

		virtual bool isEmpty() const;

		virtual void updateGeometry(int availableWidth, int availableHeight);

		QPoint focusedElementIndex() const;
		virtual bool focusChild(FocusTarget location);

		void setGridSize(int sizeX, int sizeY, bool deleteExtraItems);
		QSize gridSize() const;

		void set(Item* item, int x, int y, bool deleteOldItem);
		bool isEmpty(int x, int y) const;
		void swap(int x1, int y1, int x2, int y2);

		void remove(int x, int y, bool deleteItem = true);
		void remove(Item* item, bool deleteItem = true);
		void clear(bool deleteItems = true);

		template <class T> T* at(int x, int y);
		template <class T> T* at(int x, int y) const;

		/**
		 * Updates the content of the grid to match 'nodes'.
		 *
		 * The elements of the grid are updated so that they contain visualizations of the provided nodes. The grid will
		 * be resized if necessary.
		 *
		 * @param nodes
		 * 				A list of rows. The first row in the list is the topmost one. Each row is a list of nodes starting
		 * 				from the leftmost node.
		 *
		 * @param renderer
		 * 				The renderer to use when visualizing the nodes.
		 */
		void synchronizeWithNodes(const QList< QList<Model::Node*> >& nodes, ModelRenderer* renderer);

		void synchronize(Item*& item, Model::Node* node, int x, int y);
		template <class T> void synchronize(T*& item, bool present, const typename T::StyleType* style, int x, int y);
		template <class T> void synchronize(T*& item, typename T::NodeType* node, const typename T::StyleType* style, int x, int y);

	private:
		QVector< QVector<Item*> > items_;
		int sizeX_;
		int sizeY_;

		Item* findFirstVertical(bool startFromTop);
		Item* findFirstHorizontal(bool startFromLeft);
		Item* findNext(int dx, int dy);
};

inline QSize GridLayout::gridSize() const { return QSize(sizeX_, sizeY_); }

template <class T> inline T* GridLayout::at(int x, int y) { return static_cast<T*> (items_[x][y]); }
template <class T> inline T* GridLayout::at(int x, int y) const { return static_cast<T*> (items_[x][y]); }

template <class T> void GridLayout::synchronize(T*& item, bool present, const typename T::StyleType* style, int x, int y)
{
	if (item && !present)
	{
		remove(item);
		item = NULL;
	}

	if (!item && present)
	{
		if (style) item = new T(NULL, style);
		else item = new T(NULL);

		set(item, x, y, true);
	}
}

template <class T> void GridLayout::synchronize(T*& item, typename T::NodeType* node, const typename T::StyleType* style, int x, int y)
{
	if (item && item->node() != node)
	{
		remove(item);
		item = NULL;
	}

	if (!item && node)
	{
		if (style) item = new T(NULL, node, style);
		else item = new T(NULL, node);

		set(item, x, y, true);
	}
}

}

#endif /* GRIDLAYOUT_H_ */
