/***********************************************************************************************************************
 * SingleLayout.h
 *
 *  Created on: Feb 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SINGLELAYOUT_H_
#define SINGLELAYOUT_H_

#include "../visualizationbase_api.h"

#include "SingleLayoutBase.h"

namespace Visualization {

template <class LayoutType>
class SingleLayout : public SingleLayoutBase
{
	public:
		SingleLayout(Item* parent, const StyleType *style);

		LayoutType* layout() const;
};

template <class LayoutType>
inline LayoutType* SingleLayout<LayoutType>::layout() const { return static_cast<LayoutType*> (SingleLayoutBase::layout()); }

template <class LayoutType>
SingleLayout<LayoutType>::SingleLayout(Item* parent, const StyleType *style) :
	SingleLayoutBase(parent, style, new LayoutType(NULL))
{
	layout()->setParentItem(this);
}

}

#endif /* SINGLELAYOUT_H_ */
