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
 * ExtendableNode.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef EXTENDABLENODE_H_
#define EXTENDABLENODE_H_

#include "AttributeChain.h"
#include "../Node.h"
#include "../../ModelException.h"
#include "../../commands/ExtendedNodeChild.h"

namespace Model {

class MODELBASE_API ExtendableNode: public Node
{
	public:

		ExtendableNode(Node *parent, AttributeChain& metaData);
		ExtendableNode(Node *parent, PersistentStore &store, bool partialHint, AttributeChain& metaData);

		ExtendableNode(Node *parent = nullptr);
		ExtendableNode(Node *parent, PersistentStore &store, bool partialHint);

		virtual ~ExtendableNode();

		Node* get(const ExtendableIndex &attributeIndex) const;
		Node* get(const QString &attributeName) const;
		ExtendableIndex indexOf(Node* node) const;

		virtual QList<Node*> children();
		virtual bool replaceChild(Node* child, Node* replacement, bool releaseOldChild = true);

		void set(const ExtendableIndex &attributeIndex, Node* node);

		void removeOptional(const ExtendableIndex &attributeIndex);

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);

		bool hasAttribute(const QString& attributeName);

		QList< QPair<QString, Node*> > getAllAttributes(bool includeNullValues = false);

		static AttributeChain& getMetaData();

		virtual const QString& typeName() const;
		virtual int typeId() const;
		static const QString& typeNameStatic();
		static int typeIdStatic();
		static void registerNodeType();
		static int registerExtensionId();

		template <class T> T* extension();

	protected:
		static ExtendableIndex registerNewAttribute(AttributeChain& metaData, const QString &attributeName,
				const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent);

		static ExtendableIndex registerNewAttribute(AttributeChain& metaData, const Attribute& attribute);

		virtual AttributeChain& topLevelMeta();

	private:
		AttributeChain& meta;
		QVector<QVector<Node*> > subnodes;

		void removeAllNodes();
		void verifyHasAllMandatoryAttributes();

		static int typeId_;
		static int nextExtensionId_;
};

inline Node* ExtendableNode::get(const ExtendableIndex &attributeIndex) const { return subnodes[attributeIndex.level()][attributeIndex.index()]; }
inline int ExtendableNode::typeIdStatic() { return typeId_;}
inline int ExtendableNode::registerExtensionId() { return nextExtensionId_++; }

template <class T> T* ExtendableNode::extension()
{
	AttributeChain& topMeta = topLevelMeta();
	if (topMeta.hasExtensionInHierarchy(T::extensionId())) return new T(this, topMeta.extension(T::extensionId()));
	else return nullptr;
}

}

#endif /* EXTENDABLENODE_H_ */
