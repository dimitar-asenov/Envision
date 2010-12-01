/***********************************************************************************************************************
 * ExtendableIndex.cpp
 *
 *  Created on: Nov 26, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Extendable/ExtendableIndex.h"

namespace Model {

ExtendableIndex::ExtendableIndex() :
	level_(-1), index_(-1)
{
}

ExtendableIndex::ExtendableIndex(int level, int index) :
	level_(level), index_(index)
{
}

int ExtendableIndex::level() const
{
	return level_;
}

int ExtendableIndex::index() const
{
	return index_;
}

bool ExtendableIndex::isValid() const
{
	return level_ >= 0 && index_ >=0;
}

}
