/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "AttributeChain.h"
#include "../Node.h"
#include "../../ModelException.h"

namespace Model {

/**
 * The CompositeNode class provides convenient functionality that simplifies the creation of new nodes and allows
 * existing node types to be extended by plug-ins thereby adding new attributes to them.
 *
 * Here we will explain with an example the implementation details of this mechanism. The implementation is a variant of
 * the Properties pattern which is described here:
 *
 * http://steve-yegge.blogspot.com/2008/10/universal-design-pattern.html.
 *
 * CompositeNode and each class deriving from it contains a static member structure that contains meta data. This meta
 * data describes exactly what attributes a class has. Attributes are child nodes and are specified by a name, a node
 * type and three flags - optional, persisted and partially loaded.
 *
 * When a class Foo derives from CompositeNode it should specify what attributes it has by registering them with its
 * associated meta data object. When a particular instance of Foo is created, the new instance’s constructor does not do
 * anything. Instead the base constructor of CompositeNode initializes the object. The object contains a table (vector
 * of vectors) of child nodes which is initialized according to the meta information of all derived classes. Once the
 * object is initialized, it can be used just like any other object by calling methods directly on the instance.
 *
 * This scheme also allows for arbitrary extensions to be registered for a node type. For example it is possible to
 * register the extension Bar for all nodes of type Foo. This will simply insert additional attributes in the meta
 * information object for class Foo. When new instances of that class are created they will automatically have these new
 * attributes. Thus an extension should only be registered before any instances of an object exist. To use an extension
 * the programmer can simply call the extension method on any object deriving from CompositeNode. This will return a
 * pointer to an extension instance object which can be used to access the attributes defined by the extension. If the
 * requested extension was not registered with the target node type, nullptr will be returned.
 *
 * The \file nodeMacros.h file contains many convenience macros which simplify the use of this extension mechanism.
 */
class MODELBASE_API CompositeNode: public Super<Node>
{
	DECLARE_TYPE_ID

	public:

		CompositeNode(Node *parent, AttributeChain& metaData);
		CompositeNode(Node *parent, PersistentStore &store, bool partialHint, AttributeChain& metaData);

		CompositeNode(Node *parent = nullptr);
		CompositeNode(Node *parent, PersistentStore &store, bool partialHint);

		virtual ~CompositeNode();

		Node* get(const CompositeIndex &attributeIndex) const;
		Node* get(const QString &attributeName) const;
		CompositeIndex indexOf(Node* node) const;

		virtual QList<Node*> children();
		virtual bool replaceChild(Node* child, Node* replacement);

		void set(const CompositeIndex &attributeIndex, Node* node);

		void removeOptional(const CompositeIndex &attributeIndex);

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);

		bool hasAttribute(const QString& attributeName);

		QList< QPair<QString, Node*> > getAllAttributes(bool includeNullValues = false);

		static AttributeChain& getMetaData();

		static int registerExtensionId();

		template <class T> T* extension();

	protected:
		static CompositeIndex registerNewAttribute(AttributeChain& metaData, const QString &attributeName,
				const QString &attributeType, bool canBePartiallyLoaded, bool isOptional, bool isPersistent);

		static CompositeIndex registerNewAttribute(AttributeChain& metaData, const Attribute& attribute);

		virtual AttributeChain& topLevelMeta();

	private:
		AttributeChain& meta;
		QVector<QVector<Node*> > subnodes;

		void removeAllNodes();
		void verifyHasAllMandatoryAttributes();

		static int nextExtensionId_;
};

inline Node* CompositeNode::get(const CompositeIndex &attributeIndex) const
{
	return subnodes[attributeIndex.level()][attributeIndex.index()];
}

inline int CompositeNode::registerExtensionId() { return nextExtensionId_++; }

template <class T> T* CompositeNode::extension()
{
	AttributeChain& topMeta = topLevelMeta();
	if (topMeta.hasExtensionInHierarchy(T::extensionId())) return new T(this, topMeta.extension(T::extensionId()));
	else return nullptr;
}

}
