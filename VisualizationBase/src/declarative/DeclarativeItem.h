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
#include "DeclarativeItemBase.h"
#include "VisualizationItemWrapperFormElement.h"
#include "NodeItemWrapperFormElement.h"
#include "NodeWithVisualizationItemWrapperFormElement.h"

namespace Visualization {

class Item;

/**
 * The DeclarativeItem class is the base class for all visualization items that use the declarative API for visualizations.
 *
 * It is a template class, such that every descendant visualization type has its own static list of available forms.
 *
 * To use the declarative API, one needs to inherit from DeclarativeItem, and implement a static method initializeForms
 * in which the list of forms and their properties is set up.
 */
template <class VisualizationType>
class DeclarativeItem : public DeclarativeItemBase
{
	public:
		using I = VisualizationType;
		DeclarativeItem(Item* parent, const StyleType* style = itemStyles().get());
		virtual ~DeclarativeItem();
		virtual QList<FormElement*>& forms() const override;
		virtual QList<QList<FormElement*>>& shapeElements() const override;

	protected:

		/**
		 * Static vertion of the method forms, returning the list of forms for this declarative item type.
		 */
		static QList<FormElement*>& formsStatic();

		/**
		 * Static version of the method shapeElements, returning the list of shape element lists for each form.
		 */
		static QList<QList<FormElement*>>& shapeElementsStatic();

		/**
		 * Add an \a element to the list of forms. All the forms should be added via this method inside the static
		 * method initializeForms.
		 * Returns a pointer to the added element.
		 */
		template <class ElementType> static ElementType* addForm(ElementType* element);

		/**
		 * A factory method to get an item wrapper element with a node only, it takes a pointer to member, where the
		 * visualization \a item should be stored, and a method to get the node (\a nodeGetter) with which the \a item
		 * should be associated. The node gets visualized with its default style.
		 */
		static NodeItemWrapperFormElement<VisualizationType>*
			item(Item* VisualizationType::* item, std::function<Model::Node* (VisualizationType* v)> nodeGetter);

		/**
		 * A factory method to get an item wrapper element with a visualization only, it takes a pointer to member, where
		 * the visualization \a item should be stored, and a method to get the style (\a styleGetter) with which the
		 * \a item should be created. To call it, one has to supply the visualization type and the parent type (which is
		 * always 'I') as the template arguments.
		 */
		template <class ChildItemVisualizationType>
			static VisualizationItemWrapperFormElement<VisualizationType, ChildItemVisualizationType>*
			item(ChildItemVisualizationType* VisualizationType::* item,
					std::function<const typename ChildItemVisualizationType::StyleType* (VisualizationType* v)> styleGetter);

		/**
		 * A factory method to get an item wrapper element with a node and a visualization, it takes a pointer to member,
		 * where the visualization \a item should be stored, a method to get the style (\a styleGetter), and a method to
		 * get the node (\a nodeGetter). The node then gets visualized using the given visualization and style. To call
		 * it, one has to supply the visualization type and the parent type (which is always 'I') as template arguments.
		 */
		template <class ChildItemVisualizationType>
			static NodeWithVisualizationItemWrapperFormElement<VisualizationType, ChildItemVisualizationType>*
			item(ChildItemVisualizationType* VisualizationType::* item,
					std::function<typename ChildItemVisualizationType::NodeType* (VisualizationType* v)> nodeGetter,
					std::function<const typename ChildItemVisualizationType::StyleType* (VisualizationType* v)> styleGetter);
};

}
