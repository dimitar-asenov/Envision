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
 * List.h
 *
 *  Created on: Dec 7, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LIST_H_
#define LIST_H_

#include "../modelbase_api.h"

#include "Node.h"
#include "nodeMacros.h"
#include "../Model.h"
#include "../commands/ListInsert.h"
#include "../ModelException.h"
#include "../persistence/ClipboardStore.h"

#include <QtCore/QVector>

namespace Model {

class Text;

class MODELBASE_API List: public Node
{
	NODE_DECLARE_STANDARD_METHODS(List)

	public:
		virtual ~List();

		virtual void save(PersistentStore &store) const;
		virtual void load(PersistentStore &store);
		virtual void loadFully(PersistentStore &store);

		int size();
		template <class T> T* first();
		template <class T> T* last();
		template <class T> T* at(int i);

		int indexOf(const Node* item) const;

		void append(Node* node);
		void prepend(Node* node);
		void insert(int position, Node* node);
		void paste(ClipboardStore& clipboard, int position);

		void remove(int index);
		void remove(Node* instance);
		void clear();

		const QVector<Node*>& nodes();

		Node* findFirstSymbolDefinition(const QString& symbol);
		QList<Node*> findAllSymbolDefinitions(const QString& symbol);

		virtual bool replaceChild(Node* child, Node* replacement);

	private:
		QVector<Node*> nodes_;

		void loadSubNodes(QList<LoadedNode>& nodeList);
};

template <class T> T* List::first()
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	if ( nodes_.isEmpty() ) throw ModelException("Trying to access the first element of an empty list.");
	return static_cast<T*> (nodes_.first());
}

template <class T> T* List::last()
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	if ( nodes_.isEmpty() ) throw ModelException("Trying to access the last element of an empty list.");
	return static_cast<T*> (nodes_.last());
}

template <class T> T* List::at(int i)
{
	if (!fullyLoaded) loadFully(* (model()->store()));

	return static_cast<T*> (nodes_[i]);
}

inline const QVector<Node*>& List::nodes() { return nodes_; }
inline void List::append(Node* node) { insert(nodes_.size(), node); }
inline void List::prepend(Node* node) { insert(0, node); }

}

#endif /* LIST_H_ */
