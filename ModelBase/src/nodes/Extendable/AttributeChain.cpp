/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

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
	parent_(nullptr), numLevels_(1)
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
