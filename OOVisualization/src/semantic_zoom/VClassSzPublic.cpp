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

#include "VClassSzPublic.h"
#include "../OOVisualizationException.h"

#include "../elements/VStatementItemList.h"

#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/views/MainView.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"
#include "VisualizationBase/src/items/EmptyItem.h"

#include "ModelBase/src/nodes/Node.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VClassSzPublic, "item")

VClassSzPublic::VClassSzPublic(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{}

void VClassSzPublic::determineChildren()
{
	Super::determineChildren();
	setDefaultMoveCursorProxy(name_);
}

void VClassSzPublic::initializeForms()
{
	auto headerElement = (new GridLayoutFormElement{})
				->setHorizontalSpacing(3)->setColumnStretchFactor(3, 1)
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
				->put(0, 0, item<Static>(&I::icon_, [](I* v) -> const Visualization::StaticStyle* {
						switch (v->node()->constructKind())
						{
							case OOModel::Class::ConstructKind::Class : return &v->style()->classIcon();
							case OOModel::Class::ConstructKind::Interface : return &v->style()->interfaceIcon();
							case OOModel::Class::ConstructKind::Struct : return &v->style()->structIcon();
							case OOModel::Class::ConstructKind::Union : return &v->style()->unionIcon();
							case OOModel::Class::ConstructKind::Enum : return &v->style()->enumIcon();
							case OOModel::Class::ConstructKind::EnumClass : return &v->style()->enumIcon();
							case OOModel::Class::ConstructKind::Annotation : return &v->style()->annotationIcon();
							default: return &v->style()->classIcon();
						}
					}))
				->put(1, 0, item<VText>(&I::name_, [](I* v){return v->node()->nameNode();}, [](I* v)
						{
							// return the correct name style, depending on the classes visibility
							auto modifiers = v->node()->modifiers();
							if (modifiers->isSet(Modifier::Public)) return &v->style()->namePublic();
							else if (modifiers->isSet(Modifier::Private)) return &v->style()->namePrivate();
							else if (modifiers->isSet(Modifier::Protected)) return &v->style()->nameProtected();
							else return &v->style()->nameDefault();
						}))
				->put(2, 0, item<VList>(&I::typeArguments_, [](I* v){return v->node()->typeArguments();},
																				[](I* v){return &v->style()->typeArguments();}))
				->put(3, 0, item<VList>(&I::baseClasses_, [](I* v){return v->node()->baseClasses();},
																			[](I* v){return &v->style()->baseClasses();}));

	auto contentElement = (new GridLayoutFormElement{})
				->setSpacing(3)->setColumnStretchFactor(1, 1)
				->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
				->put(1, 0, (new DynamicGridFormElement{})->setSpacing(10, 10)->setMargins(10)
						->setMajorAxis(Visualization::GridLayouter::ColumnMajor)
						->setNodesGetter(
						[](Item* v)->QVector<QVector<Model::Node*>>{
						auto self = static_cast<I*>(v);
						auto bodyNodes = self->node()->classes()->nodes();
						for (auto method : *self->node()->methods())
							if (method->modifiers()->isSet(Modifier::Public)) bodyNodes.append(method);
						return Visualization::GridLayouter::arrange(bodyNodes, Visualization::GridLayouter::ColumnMajor);
					}));

	auto fieldContainerElement = (new GridLayoutFormElement{})
				->setVerticalSpacing(3)
				->put(0, 0, (new SequentialLayoutFormElement{})->setVertical()
								->setListOfNodes([](Item* i){return (static_cast<VClassSzPublic*>(i))->publicFields_;}));

	auto shapeElement = new ShapeFormElement{};
	auto backgroundElement = item<EmptyItem>(&I::fieldBackground_, &StyleType::fieldContainer);

	// Form 0: with field nodes
	addForm((new AnchorLayoutFormElement{})
				// place the top left corner of the field container element
				->put(TheLeftOf, fieldContainerElement, 10, FromLeftOf, headerElement)
				->put(TheTopOf, fieldContainerElement, 5, FromBottomOf, headerElement)
				// place the top left corner of the content element
				->put(TheLeftOf, contentElement, 10, FromRightOf, fieldContainerElement)
				->put(TheTopOf, contentElement, AtBottomOf, headerElement)
				// align content and header on their right side
				->put(TheRightOf, contentElement, AtRightOf, headerElement)
				// put the shape element at the right place
				->put(TheTopOf, shapeElement, AtCenterOf, headerElement)
				->put(TheLeftOf, shapeElement, AtLeftOf, headerElement)
				->put(TheBottomOf, shapeElement, 10, FromBottomOf, contentElement)
				->put(TheRightOf, shapeElement, 10, FromRightOf, headerElement)
				// put the background element around the field container element
				->put(TheLeftOf, backgroundElement, 3, FromLeftOf, fieldContainerElement)
				->put(TheRightOf, backgroundElement, 3, FromRightOf, fieldContainerElement)
				->put(TheTopOf, backgroundElement, 3, FromTopOf, fieldContainerElement)
				->put(TheBottomOf, backgroundElement, 3, FromBottomOf, fieldContainerElement));

	// Form 1: without field nodes
	addForm((new AnchorLayoutFormElement{})
				// place the top left corner of the content element
				->put(TheLeftOf, headerElement, 10, FromLeftOf, contentElement)
				->put(TheTopOf, contentElement, AtBottomOf, headerElement)
				// align content and header on their right side
				->put(TheRightOf, contentElement, AtRightOf, headerElement)
				// put the shape element at the right place
				->put(TheTopOf, shapeElement, AtCenterOf, headerElement)
				->put(TheLeftOf, shapeElement, AtLeftOf, headerElement)
				->put(TheBottomOf, shapeElement, 10, FromBottomOf, contentElement)
				->put(TheRightOf, shapeElement, 10, FromRightOf, headerElement));
}

int VClassSzPublic::determineForm()
{
	// Update Field Nodes
	publicFields_.clear();
	for (int i = 0; i< node()->fields()->size(); ++i)
	{
		if (node()->fields()->at(i)->modifiers()->isSet(Modifier::Public))
			publicFields_.append(node()->fields()->at(i));
	}

	if (node()->fields()->size() > 0) return 0;
	else return 1;
}

}
