/***********************************************************************************************************************
 * SequentialLayout.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SEQUENTIALLAYOUT_H_
#define SEQUENTIALLAYOUT_H_

#include "visualizationbase_api.h"

#include "Item.h"
#include "SequentialLayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API SequentialLayout: public Item
{
	private:
		SequentialLayoutStyle& style;
		QVector<Item*> items;

	public:
		SequentialLayout(Item* parent, SequentialLayoutStyle& style = SequentialLayoutStyle::getDefault());

		int size() const;

		void append(Item* item);
		void prepend(Item* item);
		void insert(Item* item, int position);

		template <class T> T* at(int index);
};

template <class T> T* SequentialLayout::at(int index)
{
	return static_cast<T*> (items[index]);
}

}

#endif /* SEQUENTIALLAYOUT_H_ */
