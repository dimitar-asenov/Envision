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
	ITEM_COMMON(PanelLayout)

	private:
		Item* first_;
		Item* middle_;
		Item* last_;

		void setItem(Item* item, Item*& position, bool deleteOldItem);

	public:
		PanelLayout(Item* parent, const StyleType* style = Styles::layout<PanelLayout>("default"));
		~PanelLayout();

		void setFirst(Item* item, bool deleteOldItem = true);
		void setMiddle(Item* item, bool deleteOldItem = true);
		void setLast(Item* item, bool deleteOldItem = true);

		template <class T> T* first();
		template <class T> T* middle();
		template <class T> T* last();

		void synchronizeFirst(Item*& item, Model::Node* node);
		template <class T> void synchronizeFirst(T*& item, bool present, const typename T::StyleType* style);
		template <class T> void synchronizeFirst(T*& item, typename T::NodeType* node, const typename T::StyleType* style);

		void synchronizeMiddle(Item*& item, Model::Node* node);
		template <class T> void synchronizeMiddle(T*& item, bool present, const typename T::StyleType* style);
		template <class T> void synchronizeMiddle(T*& item, typename T::NodeType* node, const typename T::StyleType* style);

		void synchronizeLast(Item*& item, Model::Node* node);
		template <class T> void synchronizeLast(T*& item, bool present, const typename T::StyleType* style);
		template <class T> void synchronizeLast(T*& item, typename T::NodeType* node, const typename T::StyleType* style);

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

inline void PanelLayout::synchronizeFirst(Item*& item, Model::Node* node)
{
	Layout::synchronizeItem(first_, item, node);
}
template <class T> inline void PanelLayout::synchronizeFirst(T*& item, bool present, const typename T::StyleType* style)
{
	Layout::synchronizeItem(first_, item, present, style);
}
template <class T> inline void PanelLayout::synchronizeFirst(T*& item, typename T::NodeType* node, const typename T::StyleType* style)
{
	Layout::synchronizeItem(first_, item, node, style);
}

inline void PanelLayout::synchronizeMiddle(Item*& item, Model::Node* node)
{
	Layout::synchronizeItem(middle_, item, node);
}
template <class T> inline void PanelLayout::synchronizeMiddle(T*& item, bool present, const typename T::StyleType* style)
{
	Layout::synchronizeItem(middle_, item, present, style);
}
template <class T> inline void PanelLayout::synchronizeMiddle(T*& item, typename T::NodeType* node, const typename T::StyleType* style)
{
	Layout::synchronizeItem(middle_, item, node, style);
}

inline void PanelLayout::synchronizeLast(Item*& item, Model::Node* node)
{
	Layout::synchronizeItem(last_, item, node);
}
template <class T> inline void PanelLayout::synchronizeLast(T*& item, bool present, const typename T::StyleType* style)
{
	Layout::synchronizeItem(last_, item, present, style);
}
template <class T> inline void PanelLayout::synchronizeLast(T*& item, typename T::NodeType* node, const typename T::StyleType* style)
{
	Layout::synchronizeItem(last_, item, node, style);
}

}

#endif /* PANELLAYOUT_H_ */
