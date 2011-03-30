/***********************************************************************************************************************
 * StaticStyle.cpp
 *
 *  Created on: Mar 30, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/StaticStyle.h"
#include "items/Static.h"

namespace Visualization {

StaticStyle::StaticStyle() :
	itemStyle_(NULL)
{

}

StaticStyle::~StaticStyle()
{
}

void StaticStyle::load(StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("itemClass", itemClass_);

	if (! itemClass_.isEmpty())
	{
		itemStyle_ = QSharedPointer<ItemStyle>(Static::constructStyle(itemClass_));
		if (itemStyle_) itemStyle_->load(sl);
	}
}

}
