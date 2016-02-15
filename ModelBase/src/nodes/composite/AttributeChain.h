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

#pragma once

#include "../../modelbase_api.h"
#include "Attribute.h"
#include "CompositeIndex.h"

namespace Model {

class MODELBASE_API AttributeChain: public QVector<Attribute>
{
	private:
		AttributeChain* parent_{};

		/**
		 * The number of levels of attributes, including this one.
		 */
		int numLevels_{1};

		QMap< int, QVector<CompositeIndex> > extensions_;

		QString typeName_;

		/**
		 * Only for CompositeNode
		 */
		AttributeChain(const QString& typeName);

	public:
		friend class CompositeNode;

		AttributeChain(const QString& typeName, AttributeChain* parentChain);

		AttributeChain* parent();
		const QString& typeName() const;

		int recursiveSize() const;
		int numLevels() const;

		AttributeChain* level(int level) const;
		CompositeIndex indexForAttribute(const QString &name) const;
		const Attribute& attribute(const CompositeIndex &index) const;
		bool hasAttribute(const QString &name) const;
		bool hasExtensionInHierarchy(int extensionId) const;
		QVector<CompositeIndex>& addExtension(int extensionId);
		const QVector<CompositeIndex>& extension(int extensionId) const;
};

}
