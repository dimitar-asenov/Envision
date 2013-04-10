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

#include "../visualizationbase_api.h"
#include "../items/Item.h"
#include "../items/ItemStyle.h"
#include "VisualizationItemWrapperElement.h"
#include "NodeItemWrapperElement.h"
#include "NodeWithVisualizationItemWrapperElement.h"

namespace Visualization {

class Element;

class VISUALIZATIONBASE_API DeclarativeItemBase: public Item
{
	ITEM_COMMON_CUSTOM_STYLENAME(DeclarativeItemBase, ItemStyle)

	public:
		DeclarativeItemBase(Item* parent, const StyleType* style = itemStyles().get());
		virtual ~DeclarativeItemBase();
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;
		virtual bool sizeDependsOnParent() const override;
		virtual QList<ItemRegion> regions() override;
		virtual QList<Element*>& forms() const = 0; // array or forms
		virtual int determineForm(); // get the appropriate form for the situation

		// factory methods for ItemWrapperElement
		template <class VisualizationType, class ParentType>
			static VisualizationItemWrapperElement<ParentType, VisualizationType>*
			item(VisualizationType* ParentType::* item,
					std::function<const typename VisualizationType::StyleType* (ParentType* v)> styleGetter);
		template <class ParentType>
			static NodeItemWrapperElement<ParentType>*
			item(Item* ParentType::* item, std::function<Model::Node* (ParentType* v)> nodeGetter);
		template <class VisualizationType, class ParentType>
			static NodeWithVisualizationItemWrapperElement<ParentType, VisualizationType>*
			item(VisualizationType* ParentType::* item,
					std::function<typename VisualizationType::NodeType* (ParentType* v)> nodeGetter,
					std::function<const typename VisualizationType::StyleType* (ParentType* v)> styleGetter);

	private:
		typedef Item BaseItemType;
		int currentFormIndex_;
};

/**
 * A factory method to get an item wrapper element with a visualization only. To call it, one has to supply the
 * visualization type and the parent type (which is always 'I') as the template arguments.
 *
 * @param item A pointer to member, where the wrapped item can be stored
 * @param styleGetter A method that, given a pointer to this class, returns a style to display the item with, matching
 * the visualization type specified in the template argument
 * @return A pointer to an item wrapper element
 */
template <class VisualizationType, class ParentType>
	VisualizationItemWrapperElement<ParentType, VisualizationType>*
	DeclarativeItemBase::item(VisualizationType* ParentType::* item,
										std::function<const typename VisualizationType::StyleType* (ParentType* v)> styleGetter)
{
	return new VisualizationItemWrapperElement<ParentType, VisualizationType>(item, styleGetter);
}

/**
 * A factory method to get an item wrapper element with a node only. This means the node gets visualized with its
 * default style. To call it, one has to supply the parent type (which is always 'I') as template argument.
 *
 * @param item A pointer to member, where the wrapped item can be stored
 * @param nodeGetter A method that, given a pointer to this class, returns a node to be displayed
 * @return A pointer to an item wrapper element
 */
template <class ParentType>
	NodeItemWrapperElement<ParentType>*
	DeclarativeItemBase::item(Item* ParentType::* item, std::function<Model::Node* (ParentType* v)> nodeGetter)
{
	return new NodeItemWrapperElement<ParentType>(item, nodeGetter);
}

/**
 * A factory method to get an item wrapper element with a node and a visualization. This means the node gets visualized
 * using the given visualization. To call it, one has to supply the visualization type and the parent type (which is
 * always 'I') as template arguments.
 *
 * @param item A pointer to member, where the wrapped item can be stored
 * @param nodeGetter A method that, given a pointer to this class, returns a node to be displayed, matching the
 * visualization type specified in the template argument
 * @param styleGetter A method that, given a pointer to this class, returns a style to display the item with, matching
 * the visualization type specified in the template argument
 * @return A pointer to an item wrapper element
 */
template <class VisualizationType, class ParentType>
	NodeWithVisualizationItemWrapperElement<ParentType, VisualizationType>*
	DeclarativeItemBase::item(VisualizationType* ParentType::* item,
			std::function<typename VisualizationType::NodeType* (ParentType* v)> nodeGetter,
			std::function<const typename VisualizationType::StyleType* (ParentType* v)> styleGetter)
{
	return new NodeWithVisualizationItemWrapperElement<ParentType, VisualizationType>(item, nodeGetter, styleGetter);
}

} /* namespace Visualization */
