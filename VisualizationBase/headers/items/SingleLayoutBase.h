/***********************************************************************************************************************
 * SingleLayoutBase.h
 *
 *  Created on: Feb 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SINGLELAYOUTBASE_H_
#define SINGLELAYOUTBASE_H_

#include "../visualizationbase_api.h"

#include "Item.h"
#include "../layouts/Layout.h"
#include "ModelBase/headers/nodes/Node.h"

namespace Visualization {

class VISUALIZATIONBASE_API SingleLayoutBase : public Item
{
	ITEM_COMMON_CUSTOM_STYLENAME(SingleLayoutBase, ItemStyle)

	public:
		SingleLayoutBase(Item* parent, const StyleType *style, Layout* layout);
		virtual ~SingleLayoutBase();

		virtual bool focusChild(FocusTarget location);
		virtual bool sizeDependsOnParent() const;
		virtual bool isEmpty() const;

		Layout* layout() const;

	protected:
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Layout* layout_;
};

inline Layout* SingleLayoutBase::layout() const { return layout_; }

}

#endif /* SINGLELAYOUTBASE_H_ */
