/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "AttributeChain.h"
#include "CompositeNode.h"

namespace Model {

AttributeChain::AttributeChain(const QString& typeName)
: typeName_{typeName}
{}

AttributeChain::AttributeChain(const QString& typeName, AttributeChain* parentChain)
: typeName_{typeName}
{
	Q_ASSERT(parentChain);
	Q_ASSERT(parentChain != this);

	//By uncommenting the following two lines, CompositeNode will not have attributes which saves memory.
	//if (parentChain == &CompositeNode::getMetaData() )
	//	return; // a null parent indicates direct inheritance from CompositeNode

	parent_ = parentChain;

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

const QString& AttributeChain::typeName() const
{
	return typeName_;
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

AttributeChain* AttributeChain::level(int level) const
{
	int goBack = numLevels_ - 1 - level;

	AttributeChain* ac = const_cast<AttributeChain*>(this);
	while ( goBack > 0 )
	{
		ac = ac->parent_;
		goBack--;
	}

	return ac;
}

CompositeIndex AttributeChain::indexForAttribute(const QString &name) const
{
	const AttributeChain* ac = this;
	int level = numLevels_ - 1;
	while ( ac )
	{
		int index = ac->indexOf(name);
		if ( index >= 0 ) return CompositeIndex{level, index};

		--level;
		ac = ac->parent_;
	}

	return CompositeIndex{};
}

const Attribute& AttributeChain::attribute(const CompositeIndex &index) const
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

QVector<CompositeIndex>& AttributeChain::addExtension(int extensionId)
{
	if (hasExtensionInHierarchy(extensionId))
		throw ModelException{"Adding an extension which has already exists in the type hierarchy"};
	extensions_.insert(extensionId, QVector<CompositeIndex>());
	return extensions_[extensionId];
}

const QVector<CompositeIndex>& AttributeChain::extension(int extensionId) const
{
	// We need the cast here since otherwise ac->extentions_ is const below, which leads QMap<>::[] to give back a
	// temporary copy not a const reference
	AttributeChain* ac = const_cast<AttributeChain*> (this);

	while ( ac )
	{
		if (ac->extensions_.contains(extensionId)) return ac->extensions_[extensionId];
		ac = ac->parent_;
	}

	throw ModelException{"Unknown extension requested. Extension Id is: " + QString::number(extensionId)};
}

}
