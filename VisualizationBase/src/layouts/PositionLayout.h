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

#include "../visualizationbase_api.h"
#include "../node_extensions/Position.h"

#include "../items/ItemWithNode.h"
#include "Layout.h"
#include "PositionLayoutStyle.h"

namespace  Visualization {

class ModelRenderer;

class VISUALIZATIONBASE_API PositionLayout : public Super<Layout>
{
	ITEM_COMMON(PositionLayout)

	public:
		PositionLayout(Item* parent, const StyleType* style = itemStyles().get());
		~PositionLayout();

		int length() const;

		/**
		 * Inserts the specified item into the Layout.
		 *
		 * @param item
		 * 				The Item to insert. This item must implement an Item that represents a node derived from
		 * 				Model::CompositeNode. Furthermore the represented node must have the Position and the
		 * 				FullDetailSize extensions.
		 */
		void insert(Item* item);

		void remove(int index, bool deleteItem = true);
		void removeAll(Item* item, bool deleteItem = true);
		void clear(bool deleteItems = true);
		bool containsNode(Model::Node* node);

		template <typename T> T* at(int index);
		template <typename T> T* at(int index) const;

		int toGrid(const int& pos) const;

		void synchronizeWithNodes(const QList<Model::Node*>& nodes);

		virtual bool isEmpty() const override;
		virtual bool isSensitiveToScale() const override;

		int focusedElementIndex() const;

	protected:
		virtual void updateGeometry(int availableWidth, int availableHeight) override;
		virtual void determineChildren() override;

	private:
		QVector<Item*> items;
		bool allNodesLackPositionInfo{};

		void swap(int i, int j);

		/**
		 * Returns true if the semantic zoom level of at least one child item was adjusted.
		 */
		bool adjustChildrenSemanticZoom();
		static std::unique_ptr<Position> positionOf(Item* item);
		static void ensureItemHasCompositeNode(const Item* item);
};

template <typename T> T* PositionLayout::at(int index) { return static_cast<T*> (items[index]); }
template <typename T> T* PositionLayout::at(int index) const { return static_cast<T*> (items[index]); }
inline std::unique_ptr<Position> PositionLayout::positionOf(Item* item)
{ return (static_cast<Model::CompositeNode*>(item->node()))->extension<Position>();	}

inline void PositionLayout::ensureItemHasCompositeNode(const Item* item)
{
	Q_ASSERT(item->node());
	Q_ASSERT(DCast<Model::CompositeNode> (item->node()));
}

}
