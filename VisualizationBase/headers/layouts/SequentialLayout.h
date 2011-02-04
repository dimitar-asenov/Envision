/***********************************************************************************************************************
 * SequentialLayout.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SEQUENTIALLAYOUT_H_
#define SEQUENTIALLAYOUT_H_

#include "../visualizationbase_api.h"

#include "Layout.h"
#include "SequentialLayoutStyle.h"

#include <QtCore/QList>

namespace Model { class Node; }

namespace Visualization {

class ModelRenderer;

class VISUALIZATIONBASE_API SequentialLayout: public Layout
{
	ITEM_COMMON(SequentialLayout, Layout)

	private:
		QVector<Item*> items;

	public:
		SequentialLayout(Item* parent, const SequentialLayoutStyle* style = Styles::layout<SequentialLayout>("default"));
		~SequentialLayout();

		int length() const;

		void append(Item* item);
		void prepend(Item* item);
		void insert(Item* item, int position);
		void swap(int i, int j);

		void remove(int index, bool deleteItem = true);
		void removeAll(Item* item, bool deleteItem = true);
		void clear(bool deleteItems = true);

		template <class T> T* at(int index);
		template <class T> T* at(int index) const;

		void synchronizeWithNodes(const QList<Model::Node*>& nodes, ModelRenderer* renderer);

		virtual void updateGeometry(int availableWidth, int availableHeight);

		int focusedElementIndex() const;
		virtual bool focusChild(FocusTarget location);

};

template <class T> T* SequentialLayout::at(int index) { return static_cast<T*> (items[index]); }
template <class T> T* SequentialLayout::at(int index) const { return static_cast<T*> (items[index]); }

}

#endif /* SEQUENTIALLAYOUT_H_ */
