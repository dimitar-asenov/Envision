/***********************************************************************************************************************
 * SequentialLayout.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SEQUENTIALLAYOUT_H_
#define SEQUENTIALLAYOUT_H_

#include "visualizationbase_api.h"

#include "Layout.h"
#include "SequentialLayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API SequentialLayout: public Layout
{
	private:
		SequentialLayoutStyle* style;
		QVector<Item*> items;

	public:
		SequentialLayout(Item* parent, SequentialLayoutStyle* style = SequentialLayoutStyle::getDefault());

		int length() const;

		void append(Item* item);
		void prepend(Item* item);
		void insert(Item* item, int position);
		void remove(int index);

		template <class T> T* at(int index);

		void updateState();

		void setStyle(SequentialLayoutStyle* style = SequentialLayoutStyle::getDefault());
};

template <class T> T* SequentialLayout::at(int index)
{
	return static_cast<T*> (items[index]);
}

}

#endif /* SEQUENTIALLAYOUT_H_ */
