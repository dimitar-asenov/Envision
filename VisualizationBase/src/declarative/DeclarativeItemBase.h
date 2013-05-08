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
#include "VisualizationItemWrapperFormElement.h"
#include "NodeItemWrapperFormElement.h"
#include "NodeWithVisualizationItemWrapperFormElement.h"

namespace Visualization {

class FormElement;

/**
 * The non-template base class for DeclarativeItem. Implements all not template related functionalities of the class
 * DeclarativeItem.
 */
class VISUALIZATIONBASE_API DeclarativeItemBase: public Super<Item>
{
	ITEM_COMMON_CUSTOM_STYLENAME(DeclarativeItemBase, ItemStyle)

	public:
		DeclarativeItemBase(Item* parent, const StyleType* style = itemStyles().get());
		virtual ~DeclarativeItemBase();
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;
		virtual bool sizeDependsOnParent() const override;
		virtual bool isEmpty() const override;
		virtual QList<ItemRegion> regions() override;
		/**
		 * Returns the list of forms available for this declarative item type.
		 */
		virtual QList<FormElement*>& forms() const = 0;
		/**
		 * Returns the index of the form to use for the current state of the declarative item.
		 *
		 * This method returns 0 by default, which means there is no redefinition needed when there is only one form
		 * available for this declarative item. When there are multiple forms to choose from, this method should be
		 * overridden to return the index to the correct form.
		 *
		 * When overriding this method, one should be aware, that this method is called upon every redraw, and therefore
		 * should not be too computationally complex.
		 *
		 * This method gets called once per redraw. To use the cached result of this call, use currentForm.
		 */
		virtual int determineForm();
		/**
		 * Returns the form at the index from the cached result of determineForm.
		 */
		FormElement* currentForm() const;
		/**
		 * Returns the a list of shape elements per form for this declarative item type. Those lists are determined from
		 * the list of forms.
		 */
		virtual QList<QList<FormElement*>>& shapeElements() const = 0;
		/**
		 * Returns the list of shape elements belonging to the current form (the same index as returned by determineForm).
		 */
		QList<FormElement*> currentShapeElements() const;

		/**
		 * A factory method to get an item wrapper element with a visualization only, it takes a pointer to member, where
		 * the visualization \a item should be stored, and a method to get the style (\a styleGetter) with which the
		 * \a item should be created. To call it, one has to supply the visualization type and the parent type (which is
		 * always 'I') as the template arguments.
		 */
		template <class VisualizationType, class ParentType>
			static VisualizationItemWrapperFormElement<ParentType, VisualizationType>*
			item(VisualizationType* ParentType::* item,
					std::function<const typename VisualizationType::StyleType* (ParentType* v)> styleGetter);
		/**
		 * A factory method to get an item wrapper element with a node only, it takes a pointer to member, where the
		 * visualization \a item should be stored, and a method to get the node (\a nodeGetter) with which the \a item
		 * should be associated. The node gets visualized with its default style. To call it, one has to supply the parent
		 * type (which is always 'I') as template argument.
		 */
		template <class ParentType>
			static NodeItemWrapperFormElement<ParentType>*
			item(Item* ParentType::* item, std::function<Model::Node* (ParentType* v)> nodeGetter);
		/**
		 * A factory method to get an item wrapper element with a node and a visualization, it takes a pointer to member,
		 * where the visualization \a item should be stored, a method to get the style (\a styleGetter), and a method to
		 * get the node (\a nodeGetter). The node then gets visualized using the given visualization and style. To call
		 * it, one has to supply the visualization type and the parent type (which is always 'I') as template arguments.
		 */
		template <class VisualizationType, class ParentType>
			static NodeWithVisualizationItemWrapperFormElement<ParentType, VisualizationType>*
			item(VisualizationType* ParentType::* item,
					std::function<typename VisualizationType::NodeType* (ParentType* v)> nodeGetter,
					std::function<const typename VisualizationType::StyleType* (ParentType* v)> styleGetter);

	private:
		int currentFormIndex_{};
};

inline FormElement* DeclarativeItemBase::currentForm() const {return forms().at(currentFormIndex_);}
inline QList<FormElement*> DeclarativeItemBase::currentShapeElements() const {return shapeElements().at(currentFormIndex_);}

template <class VisualizationType, class ParentType>
	VisualizationItemWrapperFormElement<ParentType, VisualizationType>*
	DeclarativeItemBase::item(VisualizationType* ParentType::* item,
										std::function<const typename VisualizationType::StyleType* (ParentType* v)> styleGetter)
{
	return new VisualizationItemWrapperFormElement<ParentType, VisualizationType>(item, styleGetter);
}

template <class ParentType>
	NodeItemWrapperFormElement<ParentType>*
	DeclarativeItemBase::item(Item* ParentType::* item, std::function<Model::Node* (ParentType* v)> nodeGetter)
{
	return new NodeItemWrapperFormElement<ParentType>(item, nodeGetter);
}

template <class VisualizationType, class ParentType>
	NodeWithVisualizationItemWrapperFormElement<ParentType, VisualizationType>*
	DeclarativeItemBase::item(VisualizationType* ParentType::* item,
			std::function<typename VisualizationType::NodeType* (ParentType* v)> nodeGetter,
			std::function<const typename VisualizationType::StyleType* (ParentType* v)> styleGetter)
{
	return new NodeWithVisualizationItemWrapperFormElement<ParentType, VisualizationType>(item, nodeGetter, styleGetter);
}

} /* namespace Visualization */
