/***********************************************************************************************************************
 * AttributeChain.cpp
 *
 *  Created on: Nov 26, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Extendable/AttributeChain.h"

namespace Model {

AttributeChain::AttributeChain() :
	parent_(NULL), numLevels_(1)
{
}

void AttributeChain::setParent(AttributeChain* newParent)
{
	parent_ = newParent;

	// Compute the number of levels
	numLevels_ = 1;
	AttributeChain* ac = parent_;
	while ( ac )
	{
		numLevels_++;
		ac = ac->parent_;
	}
}

AttributeChain* AttributeChain::parent()
{
	return parent_;
}

int AttributeChain::recursiveSize() const
{
	const AttributeChain* ac = this;

	int result = 0;
	while ( ac )
	{
		result += ac->size();
		ac = ac->parent_;
	}

	return result;
}

int AttributeChain::numLevels() const
{
	return numLevels_;
}

AttributeChain* AttributeChain::level(int level)
{
	int goBack = numLevels_ - 1 - level;

	AttributeChain* ac = this;
	while ( goBack > 0 )
	{
		ac = ac->parent_;
		goBack--;
	}

	return ac;
}

ExtendableIndex AttributeChain::indexForAttribute(const QString &name) const
{
	const AttributeChain* ac = this;
	int level = numLevels_ - 1;
	while ( ac )
	{
		int index = ac->indexOf(name);
		if ( index >= 0 ) return ExtendableIndex(level, index);

		--level;
		ac = ac->parent_;
	}

	return ExtendableIndex();
}

const Attribute& AttributeChain::attribute(const ExtendableIndex &index)
{
	return level(index.level())->at(index.index());
}

bool AttributeChain::hasAttribute(const QString &name) const
{
	const AttributeChain* ac = this;
	while ( ac )
	{
		int index = ac->indexOf(name);
		if ( index >= 0 ) return true;

		ac = ac->parent_;
	}

	return false;

}

}
