/***********************************************************************************************************************
 * PanelLayout.h
 *
 *  Created on: Dec 14, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef PANELLAYOUT_H_
#define PANELLAYOUT_H_

#include "../visualizationbase_api.h"

#include "Layout.h"
#include "PanelLayoutStyle.h"
#include "SequentialLayout.h"

namespace Visualization {

class VISUALIZATIONBASE_API PanelLayout: public Layout
{
	ITEM_COMMON(PanelLayout, Layout)

	private:
		Item* first_;
		Item* middle_;
		Item* last_;

		void setItem(Item* item, Item*& position, bool deleteOldItem);

	public:
		PanelLayout(Item* parent, const PanelLayoutStyle* style = Styles::layout<PanelLayout>("default"));
		~PanelLayout();

		void setFirst(Item* item, bool deleteOldItem = true);
		void setMiddle(Item* item, bool deleteOldItem = true);
		void setLast(Item* item, bool deleteOldItem = true);

		template <class T> T* first();
		template <class T> T* middle();
		template <class T> T* last();

		virtual bool isEmpty() const;

		virtual bool sizeDependsOnParent() const;
		virtual void updateGeometry(int availableWidth, int availableHeight);
		virtual bool focusChild(FocusTarget location);
};

inline void PanelLayout::setFirst(Item* item, bool deleteOldItem) { setItem(item, first_, deleteOldItem); }
inline void PanelLayout::setMiddle(Item* item, bool deleteOldItem) { setItem(item, middle_, deleteOldItem); }
inline void PanelLayout::setLast(Item* item, bool deleteOldItem) { setItem(item, last_, deleteOldItem); }

template <class T> inline T* PanelLayout::first() { return static_cast<T*> (first_); };
template <class T> inline T* PanelLayout::middle() { return static_cast<T*> (middle_); };
template <class T> inline T* PanelLayout::last() { return static_cast<T*> (last_); };

}

#endif /* PANELLAYOUT_H_ */
