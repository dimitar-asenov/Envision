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
	SAFE_DELETE(itemStyle_);
}

void StaticStyle::load(StyleLoader& sl)
{
	ItemStyle::load(sl);

	sl.load("itemClass", itemClass_);

	if (! itemClass_.isEmpty())
	{
		itemStyle_ = Static::constructStyle(itemClass_);
		if (itemStyle_) itemStyle_->load(sl);
	}
}

}
