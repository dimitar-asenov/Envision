/***********************************************************************************************************************
 * VList.h
 *
 *  Created on: Jan 7, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VLIST_H_
#define VLIST_H_

#include "../visualizationbase_api.h"

#include "ModelItem.h"
#include "VListStyle.h"
#include "../Styles.h"
#include "../layouts/SequentialLayout.h"
#include "ModelBase/headers/nodes/List.h"

namespace Visualization {

class VISUALIZATIONBASE_API VList: public ModelItem
{
	ITEM_COMMON(VList, ModelItem)

	private:
		SequentialLayout items_;

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	public:
		VList(Item* parent, Model::List* node, const VListStyle* style = Styles::item<VList>("default"));
		virtual ~VList();

		int length() const;
		template <class T> T* at(int index);

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

inline int VList::length() const { return items_.length(); }
template <class T> T* VList::at(int index) { return items_.at<T>(index); }

}

#endif /* VLIST_H_ */
