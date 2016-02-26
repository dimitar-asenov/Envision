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

#include "VClass.h"
#include "../OOVisualizationException.h"

#include "../elements/VStatementItemList.h"

#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/items/EmptyItem.h"

#include "ModelBase/src/nodes/Node.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VClass, "item")

VClass::VClass(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{
}

void VClass::determineChildren()
{
	Super::determineChildren();
	setDefaultMoveCursorProxy(name_);
}

void VClass::initializeForms()
{
	auto titleElement = grid({{
	  item<Static>(&I::icon_, [](I* v) -> const Visualization::StaticStyle* {
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
	}), item<VText>(&I::name_, [](I* v){return v->node()->nameNode();}, &StyleType::name)}})
		->setNoBoundaryCursors([](Item*){return true;})
		->setNoInnerCursors([](Item*){return true;})
		->setVerticalAlignment(LayoutStyle::Alignment::Center)
		->setHorizontalSpacing(5);

	auto titleBackgroundElement = item<EmptyItem>(&I::titleBackground_, &StyleType::titleBackground);
	auto stretchAfterTitle = grid({{}})->setColumnStretchFactor(0, 1);

	auto headerElement = (new GridLayoutFormElement{})
				->setHorizontalSpacing(15)
				->setColumnStretchFactor(2, 1)
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
				->put(0, 0, item<VList>(&I::typeArguments_, [](I* v){return v->node()->typeArguments();},
																				[](I* v){return &v->style()->typeArguments();}))
				->put(1, 0, item<VList>(&I::baseClasses_, [](I* v){return v->node()->baseClasses();},
																			[](I* v){return &v->style()->baseClasses();}));

	auto contentElement = (new GridLayoutFormElement{})
				->setSpacing(3)->setColumnStretchFactor(1, 1)
				->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
				->put(1, 0, item(&I::comment_, [](I* v){return v->node()->comment();}))
				->put(1, 1, item<VStatementItemList>(&I::annotations_, [](I* v)
											{return v->node()->annotations()->size() > 0 ? v->node()->annotations() : nullptr;},
								[](I* v){return &v->style()->annotations();}))
				->put(1, 2, item<VList>(&I::enumerators_, [](I* v)
											{return v->node()->enumerators()->size() > 0 ? v->node()->enumerators() : nullptr;},
								[](I* v){return &v->style()->enumerators();}))
				->put(0, 3, item<Static>(&I::friendsSymbol_,
						[](I* v){return &v->style()->friendsSymbol();})->setEnabled(
								[](I* v){return v->node()->friends()->size() > 0;}))
				->put(1, 3, item<VList>(&I::friends_,
						[](I* v) {return v->node()->friends()->size() > 0 ? v->node()->friends() : nullptr;},
						[](I* v){return &v->style()->friends();}))
				->put(1, 4, item<VList>(&I::declarations_,
						[](I* v) {return v->node()->subDeclarations()->size() > 0 ? v->node()->subDeclarations() : nullptr;},
						[](I* v){return &v->style()->declarations();}))
				->put(1, 5, item<VList>(&I::metaCalls_,
						[](I* v) {return v->node()->metaCalls();},
						[](I* v){return &v->style()->metaCalls();}))
				->put(1, 6, (new DynamicGridFormElement{})->setSpacing(20, 20)->setMargins(10)
						->setMajorAxis(Visualization::GridLayouter::ColumnMajor)
						->setNodesGetter(
						[](Item* v)->QVector<QVector<Model::Node*>>{
						auto self = static_cast<I*>(v);
						return Visualization::GridLayouter::arrange(
								self->node()->classes()->nodes() + self->node()->methods()->nodes(),
								Visualization::GridLayouter::ColumnMajor);
					}));


	auto fieldContainerElement = (new GridLayoutFormElement{})
				->setVerticalSpacing(3)
				->put(0, 0, (new SequentialLayoutFormElement{})->setVertical()
								->setListOfNodes([](Item* i){return (static_cast<VClass*>(i))->publicFields_;}))
				->put(0, 1, (new SequentialLayoutFormElement{})->setVertical()
								->setListOfNodes([](Item* i){return (static_cast<VClass*>(i))->privateFields_;}))
				->put(0, 2, (new SequentialLayoutFormElement{})->setVertical()
								->setListOfNodes([](Item* i){return (static_cast<VClass*>(i))->protectedFields_;}))
				->put(0, 3, (new SequentialLayoutFormElement{})->setVertical()
								->setListOfNodes([](Item* i){return (static_cast<VClass*>(i))->defaultFields_;}));

	auto shapeElement = new ShapeFormElement{};
	auto backgroundElement = item<EmptyItem>(&I::fieldBackground_, &StyleType::fieldContainer);
	auto stretchAfterFields = grid({{}})->setColumnStretchFactor(0, 1);

	// Form 0: with field nodes
	addForm((new AnchorLayoutFormElement{})
				// place the title background
				->put(TheLeftOf, titleBackgroundElement, 3, FromLeftOf, titleElement)
				->put(TheRightOf, titleBackgroundElement, 3, FromRightOf, titleElement)
				->put(TheTopOf, titleBackgroundElement, 3, FromTopOf, titleElement)
				->put(TheBottomOf, titleBackgroundElement, 3, FromBottomOf, titleElement)
				->put(TheTopOf, stretchAfterTitle, AtTopOf, titleBackgroundElement)
				->put(TheLeftOf, stretchAfterTitle, 10, FromRightOf, titleBackgroundElement)

				->put(TheLeftOf, headerElement, -20, FromLeftOf, titleBackgroundElement)
				->put(TheTopOf, headerElement, 30, FromBottomOf, titleBackgroundElement)

				->put(TheLeftOf, backgroundElement, -20, FromLeftOf, titleBackgroundElement)
				->put(TheTopOf, backgroundElement, 30, FromBottomOf, headerElement)
				->put(TheTopOf, stretchAfterFields, AtTopOf, backgroundElement)
				->put(TheLeftOf, stretchAfterFields, 10, FromRightOf, backgroundElement)

				// place the top left corner of the content element
				->put(TheLeftOf, contentElement, -20, FromLeftOf, titleBackgroundElement)
				->put(TheTopOf, contentElement, 10, FromBottomOf, backgroundElement)

				// align content and header on their right side
				->put(TheRightOf, contentElement, AtRightOf, headerElement)
				->put(TheRightOf, contentElement, AtRightOf, stretchAfterTitle)
				->put(TheRightOf, contentElement, AtRightOf, stretchAfterFields)

				// put the shape element at the right place
				->put(TheTopOf, shapeElement, -2, FromBottomOf, titleBackgroundElement)
				->put(TheLeftOf, shapeElement, AtLeftOf, titleBackgroundElement)
				->put(TheBottomOf, shapeElement, 15, FromBottomOf, contentElement)
				->put(TheRightOf, shapeElement, 10, FromRightOf, stretchAfterTitle)

				// put the background element around the field container element
				->put(TheLeftOf, backgroundElement, 3, FromLeftOf, fieldContainerElement)
				->put(TheRightOf, backgroundElement, 3, FromRightOf, fieldContainerElement)
				->put(TheTopOf, backgroundElement, 3, FromTopOf, fieldContainerElement)
				->put(TheBottomOf, backgroundElement, 3, FromBottomOf, fieldContainerElement));

	// Form 1: without field nodes
	addForm((new AnchorLayoutFormElement{})
				// place the title background
				->put(TheLeftOf, titleBackgroundElement, 3, FromLeftOf, titleElement)
				->put(TheRightOf, titleBackgroundElement, 3, FromRightOf, titleElement)
				->put(TheTopOf, titleBackgroundElement, 3, FromTopOf, titleElement)
				->put(TheBottomOf, titleBackgroundElement, 3, FromBottomOf, titleElement)
				->put(TheTopOf, stretchAfterTitle, AtTopOf, titleBackgroundElement)

				->put(TheLeftOf, headerElement, -20, FromLeftOf, titleBackgroundElement)
				->put(TheTopOf, headerElement, 30, FromBottomOf, titleBackgroundElement)

				// place the top left corner of the content element
				->put(TheLeftOf, contentElement, -20, FromLeftOf, titleBackgroundElement)
				->put(TheTopOf, contentElement, 10, FromBottomOf, headerElement)

				// align content and header on their right side
				->put(TheRightOf, contentElement, AtRightOf, headerElement)
				->put(TheLeftOf, stretchAfterTitle, 10, FromRightOf, titleBackgroundElement)
				->put(TheRightOf, contentElement, AtRightOf, stretchAfterTitle)

				// put the shape element at the right place
				->put(TheTopOf, shapeElement, -2, FromBottomOf, titleBackgroundElement)
				->put(TheLeftOf, shapeElement, AtLeftOf, titleBackgroundElement)
				->put(TheBottomOf, shapeElement, 15, FromBottomOf, contentElement)
				->put(TheRightOf, shapeElement, 10, FromRightOf, stretchAfterTitle));
}

int VClass::determineForm()
{
	// Update Field Nodes
	publicFields_.clear();
	privateFields_.clear();
	protectedFields_.clear();
	defaultFields_.clear();

	for (int i = 0; i< node()->fields()->size(); ++i)
	{
		if (node()->fields()->at(i)->modifiers()->isSet(Modifier::Public))
			publicFields_.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->modifiers()->isSet(Modifier::Private))
			privateFields_.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->modifiers()->isSet(Modifier::Protected))
			protectedFields_.append(node()->fields()->at(i));
		else defaultFields_.append(node()->fields()->at(i));
	}

	if (node()->fields()->size() > 0) return 0;
	else return 1;
}

}
