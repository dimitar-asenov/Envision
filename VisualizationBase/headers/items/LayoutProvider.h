/***********************************************************************************************************************
 * LayoutProvider.h
 *
 *  Created on: Feb 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LAYOUTPROVIDER_H_
#define LAYOUTPROVIDER_H_

#include "LayoutProviderBase.h"
#include "../layouts/SequentialLayout.h"

namespace Visualization {

template <class LayoutType = SequentialLayout>
class LayoutProvider : public LayoutProviderBase
{
	public:
		LayoutProvider(Item* parent, const StyleType *style);

		LayoutType* layout() const;
};

template <class LayoutType>
inline LayoutType* LayoutProvider<LayoutType>::layout() const { return static_cast<LayoutType*> (LayoutProviderBase::layout()); }

template <class LayoutType>
LayoutProvider<LayoutType>::LayoutProvider(Item* parent, const StyleType *style) :
LayoutProviderBase(parent, style, new LayoutType(NULL))
{
	layout()->setParentItem(this);
}

}

#endif /* LAYOUTPROVIDER_H_ */
