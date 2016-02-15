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

#include "../modelbase_api.h"

#include "Node.h"
#include "nodeMacros.h"
#include "../model/TreeManager.h"
#include "../commands/ListInsert.h"
#include "../ModelException.h"

namespace Model {

class ClipboardStore;

class MODELBASE_API List: public Super<Node>
{
	NODE_DECLARE_STANDARD_METHODS(List)

	public:
		List(const List& other);
		virtual ~List();

		/**
		 * TODO Document this somewhere useful. Like in the Persistent store interface.
		 * If the node is partially loaded the Store will automatically fill in the missing fields by taking the old
		 * version of every subnode whose name is not specified here.
		 */
		virtual void save(PersistentStore &store) const override;
		virtual void load(PersistentStore &store) override;

		virtual QList<Node*> children() const override;

		int size() const;
		bool isEmpty() const;
		template <typename T = Node> T* first() const;
		template <typename T = Node> T* last() const;
		template <typename T = Node> T* at(int i) const;

		int indexOf(const Node* item) const;
		int indexToSubnode(const Node* node) const;
		bool contains(const Node* node) const;

		void append(Node* node);
		void prepend(Node* node);
		void insert(int position, Node* node);
		void paste(ClipboardStore& clipboard, int position);

		/**
		 * \brief Removes the child node at position \a index.
		 */
		void remove(int index);

		/**
		 * \brief Removes the child \a instance.
		 */
		void remove(Node* instance);
		void clear();

		using iterator = QVector<Node*>::Iterator;
		using const_iterator = QVector<Node*>::ConstIterator;

		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;
		iterator end();
		const_iterator end() const;
		const_iterator cend() const;

		const QVector<Node*>& nodes();

		bool isTransparentForNameResolution() const override;

		virtual bool findSymbols(QSet<Node*>& result, const SymbolMatcher& matcher, const Node* source,
				FindSymbolDirection direction, SymbolTypes symbolTypes, bool exhaustAllScopes) const override;

		virtual bool replaceChild(Node* child, Node* replacement) override;

		/**
		 * Creates a new Node that is suitable for inserting in the list. The newly created node is not directly inserted
		 * into the list.
		 *
		 * The default implementation returns nullptr. This behavior is suitable for lists that are not managed by
		 * standard interaction handlers or where the creation of item in standard ways is not necessary.
		 *
		 * Reimplement this method in derived classes to control what Node is created.
		 *
		 * This method is used by higher level abstractions and in TypedList to facilitate the automatic creation of list
		 * elements.
		 */
		virtual Node* createDefaultElement();

		virtual int lowerTypeBoundForElements() const;

	private:
		QVector<Node*> nodes_;

		void loadSubNodes(QList<LoadedNode>& nodeList);
};

inline List::iterator List::begin() {return nodes_.begin();}
inline List::const_iterator List::begin() const {return nodes_.begin();}
inline List::const_iterator List::cbegin() const {return nodes_.constBegin();}
inline List::iterator List::end() {return nodes_.end();}
inline List::const_iterator List::end() const {return nodes_.end();}
inline List::const_iterator List::cend() const {return nodes_.constEnd();}

template <typename T> T* List::first() const
{
	if ( nodes_.isEmpty() ) throw ModelException{"Trying to access the first element of an empty list."};
	return static_cast<T*> (nodes_.first());
}

template <typename T> T* List::last() const
{
	if ( nodes_.isEmpty() ) throw ModelException{"Trying to access the last element of an empty list."};
	return static_cast<T*> (nodes_.last());
}

template <typename T> T* List::at(int i) const
{
	return static_cast<T*> (nodes_[i]);
}

inline const QVector<Node*>& List::nodes() { return nodes_; }
inline void List::append(Node* node) { insert(nodes_.size(), node); }
inline void List::prepend(Node* node) { insert(0, node); }
inline bool List::contains(const Node* node) const {return nodes_.contains(const_cast<Node*>(node));}
inline int List::size() const { return nodes_.size(); }
inline bool List::isEmpty() const { return nodes_.isEmpty(); }

}
