/***********************************************************************************************************************
 * AttributeChain.cpp
 *
 *  Created on: Nov 26, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "AttributeChain.h"

namespace Model {

AttributeChain::AttributeChain() :
	parent(NULL), numLevels(1)
{
}

void AttributeChain::setParent(AttributeChain* newParent)
{
	parent = newParent;

	// Compute the number of levels
	numLevels = 1;
	AttributeChain* ac = parent;
	while ( ac )
	{
		numLevels++;
		ac = ac->parent;
	}
}

AttributeChain* AttributeChain::getParent()
{
	return parent;
}

int AttributeChain::getRecursiveSize() const
{
	const AttributeChain* ac = this;

	int result = 0;
	while ( ac )
	{
		result += size();
		ac = ac->parent;
	}

	return result;
}

int AttributeChain::getNumLevels() const
{
	return numLevels;
}

AttributeChain* AttributeChain::getLevel(int level)
{
	int goBack = numLevels - 1 - level;

	AttributeChain* ac = this;
	while ( goBack > 0 )
	{
		ac = ac->parent;
		goBack--;
	}

	return ac;
}

ExtendableIndex AttributeChain::getIndexForAttribute(const QString &name) const
{
	const AttributeChain* ac = this;
	int level = numLevels - 1;
	while ( ac )
	{
		int index = ac->indexOf(name);
		if ( index >= 0 ) return ExtendableIndex(level, index);

		--level;
		ac = ac->parent;
	}

	return ExtendableIndex();
}

const Attribute& AttributeChain::getAttribute(const ExtendableIndex &index)
{
	return getLevel(index.level())->at(index.index());
}

bool AttributeChain::hasAttribute(const QString &name) const
{
	const AttributeChain* ac = this;
	while ( ac )
	{
		int index = ac->indexOf(name);
		if ( index >= 0 ) return true;

		ac = ac->parent;
	}

	return false;

}

}
