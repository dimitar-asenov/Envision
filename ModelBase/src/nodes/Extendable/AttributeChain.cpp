/***********************************************************************************************************************
 * AttributeChain.cpp
 *
 *  Created on: Nov 26, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "nodes/Extendable/AttributeChain.h"
#include "nodes/Extendable/ExtendableNode.h"

namespace Model {

AttributeChain::AttributeChain() :
	parent_(NULL), numLevels_(1)
{
}

void AttributeChain::setParent(AttributeChain* newParent)
{
	if (newParent == this) return;
	if (newParent == &ExtendableNode::getMetaData() ) return;

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

bool AttributeChain::hasExtensionInHierarchy(int extensionId) const
{
	const AttributeChain* ac = this;
	while ( ac )
	{
		if (ac->extensions_.contains(extensionId)) return true;
		ac = ac->parent_;
	}

	return false;
}

QVector<ExtendableIndex>& AttributeChain::addExtension(int extensionId)
{
	if (hasExtensionInHierarchy(extensionId)) throw ModelException("Adding an extension which has already exists in the type hierarchy");
	extensions_.insert(extensionId, QVector<ExtendableIndex>());
	return extensions_[extensionId];
}

const QVector<ExtendableIndex>& AttributeChain::extension(int extensionId) const
{
	// We need the cast here since otherwise ac->extentions_ is const below, which leads QMap<>::[] to give back a
	// temporary copy not a const reference
	AttributeChain* ac = const_cast<AttributeChain*> (this);

	while ( ac )
	{
		if (ac->extensions_.contains(extensionId)) return ac->extensions_[extensionId];
		ac = ac->parent_;
	}

	throw ModelException("Unknown extension requested. Extension Id is: " + QString::number(extensionId));
}

}
