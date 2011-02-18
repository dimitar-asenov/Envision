/***********************************************************************************************************************
 * VList.h
 *
 *  Created on: Jan 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLIST_H_
#define VLIST_H_

#include "../visualizationbase_api.h"

#include "ItemWithNode.h"
#include "LayoutProvider.h"
#include "VListStyle.h"
#include "../Styles.h"
#include "../layouts/SequentialLayout.h"
#include "ModelBase/headers/nodes/List.h"

namespace Visualization {

class VISUALIZATIONBASE_API VList: public ItemWithNode< LayoutProvider<SequentialLayout>, Model::List>
{
	ITEM_COMMON(VList)

	protected:
		virtual void determineChildren();

	public:
		VList(Item* parent, NodeType* node, const StyleType* style = Styles::item<VList>("default"));

		int length() const;
		template <class T> T* at(int index);
		int focusedElementIndex() const;
};

inline int VList::length() const { return layout()->length(); }
inline int VList::focusedElementIndex() const { return layout()->focusedElementIndex(); }
template <class T> T* VList::at(int index) { return layout()->at<T>(index); }

}

#endif /* VLIST_H_ */
