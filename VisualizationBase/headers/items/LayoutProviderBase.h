/***********************************************************************************************************************
 * LayoutProviderBase.h
 *
 *  Created on: Feb 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LAYOUTPROVIDERBASE_H_
#define LAYOUTPROVIDERBASE_H_

#include "../visualizationbase_api.h"

#include "Item.h"
#include "../layouts/Layout.h"
#include "ModelBase/headers/nodes/Node.h"

namespace Visualization {

class VISUALIZATIONBASE_API LayoutProviderBase : public Item
{
	ITEM_COMMON_CUSTOM_STYLENAME(LayoutProviderBase, ItemStyle)

	public:
		LayoutProviderBase(Item* parent, const StyleType *style, Layout* layout);
		virtual ~LayoutProviderBase();

		virtual bool focusChild(FocusTarget location);
		virtual bool sizeDependsOnParent() const;
		virtual bool isEmpty() const;

		Layout* layout() const;

	protected:
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Layout* layout_;
};

inline Layout* LayoutProviderBase::layout() const { return layout_; }

}

#endif /* LAYOUTPROVIDERBASE_H_ */
