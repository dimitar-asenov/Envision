/***********************************************************************************************************************
 * PositionLayout.h
 *
 *  Created on: Feb 2, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef POSITIONLAYOUT_H_
#define POSITIONLAYOUT_H_

#include "../visualizationbase_api.h"
#include "../node_extensions/Position.h"

#include "../items/ModelItem.h"
#include "Layout.h"
#include "PositionLayoutStyle.h"

namespace  Visualization {

class ModelRenderer;

class VISUALIZATIONBASE_API PositionLayout : public Layout
{
	ITEM_COMMON(PositionLayout, Layout)

	private:
		QVector<ModelItem*> items;
		QVector<const Position*> positions;

		void swap(int i, int j);

	public:
		PositionLayout(Item* parent, const PositionLayoutStyle* style = Styles::layout<PositionLayout>("default"));
		~PositionLayout();

		int length() const;

		/**
		 * Inserts the specified item into the Layout.
		 *
		 *	@param item
		 *					The Item to insert. This item must implement ModelItem that represents a node derived from
		 *					Model::ExtendableNode. Furthermore the represented node must have the Position Extension.
		 */
		void insert(Item* item);

		void remove(int index, bool deleteItem = true);
		void removeAll(Item* item, bool deleteItem = true);
		void clear(bool deleteItems = true);
		bool hasNode(Model::Node* node);

		template <class T> T* at(int index);
		template <class T> T* at(int index) const;

		void synchronizeWithNodes(const QList<Model::Node*>& nodes, ModelRenderer* renderer);

		virtual void updateGeometry(int availableWidth, int availableHeight);

		int focusedElementIndex() const;
		virtual bool focusChild(FocusTarget location);

		int toGrid(const int& pos) const;
};

template <class T> T* PositionLayout::at(int index) { return static_cast<T*> (items[index]); }
template <class T> T* PositionLayout::at(int index) const { return static_cast<T*> (items[index]); }

}

#endif /* POSITIONLAYOUT_H_ */
