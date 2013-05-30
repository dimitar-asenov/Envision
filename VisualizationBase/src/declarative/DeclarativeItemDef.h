/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#pragma once
#include "DeclarativeItem.h"

#include "GridLayoutFormElement.h"
#include "AnchorLayoutFormElement.h"
#include "SequentialLayoutFormElement.h"
#include "ShapeFormElement.h"

namespace Visualization {

template <class VisualizationType>
DeclarativeItem<VisualizationType>::DeclarativeItem(Item* parent, const StyleType* style) :
		DeclarativeItemBase(parent, style)
{
		static bool initialized = false;
		if(!initialized)
		{
			VisualizationType::initializeForms();
			for (auto f : forms())
				shapeElementsStatic().append(f->shapeElements());
			for (auto s : shapeElementsStatic())
			initialized = true;
		}
}

template <class VisualizationType>
DeclarativeItem<VisualizationType>::~DeclarativeItem()
{
	for(FormElement* form : formsStatic())
		form->destroyChildItems(this);
}

template <class VisualizationType>
QList<FormElement*>& DeclarativeItem<VisualizationType>::formsStatic()
{
	static QList<FormElement*> list;
	return list;
}

template <class VisualizationType>
QList<FormElement*>& DeclarativeItem<VisualizationType>::forms() const
{
	return formsStatic();
}

template <class VisualizationType>
QList<QList<FormElement*>>& DeclarativeItem<VisualizationType>::shapeElementsStatic()
{
	static QList<QList<FormElement*>> list;
	return list;
}

template <class VisualizationType>
QList<QList<FormElement*>>& DeclarativeItem<VisualizationType>::shapeElements() const
{
	return shapeElementsStatic();
}

template <class VisualizationType>
template <class ElementType>
ElementType* DeclarativeItem<VisualizationType>::addForm(ElementType* element)
{
	formsStatic().append(element);
	return element;
}

template <class VisualizationType>
	NodeItemWrapperFormElement<VisualizationType>*
	DeclarativeItem<VisualizationType>::item(Item* VisualizationType::* item,
			std::function<Model::Node* (VisualizationType* v)> nodeGetter)
{
	return new NodeItemWrapperFormElement<VisualizationType>(item, nodeGetter);
}

template <class VisualizationType>
template <class ChildItemVisualizationType>
	VisualizationItemWrapperFormElement<VisualizationType, ChildItemVisualizationType>*
	DeclarativeItem<VisualizationType>::item(ChildItemVisualizationType* VisualizationType::* item,
										std::function<const
											typename ChildItemVisualizationType::StyleType* (VisualizationType* v)> styleGetter)
{
	return new VisualizationItemWrapperFormElement<VisualizationType, ChildItemVisualizationType>(item, styleGetter);
}

template <class VisualizationType>
template <class ChildItemVisualizationType>
	NodeWithVisualizationItemWrapperFormElement<VisualizationType, ChildItemVisualizationType>*
	DeclarativeItem<VisualizationType>::item(ChildItemVisualizationType* VisualizationType::* item,
			std::function<typename ChildItemVisualizationType::NodeType* (VisualizationType* v)> nodeGetter,
			std::function<const typename ChildItemVisualizationType::StyleType* (VisualizationType* v)> styleGetter)
{
	return new NodeWithVisualizationItemWrapperFormElement<VisualizationType, ChildItemVisualizationType>
		(item, nodeGetter, styleGetter);
}

}
