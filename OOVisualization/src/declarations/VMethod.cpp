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

#include "VMethod.h"
#include "../elements/VStatementItemList.h"
#include "../OOVisualizationException.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"
#include "VisualizationBase/src/items/EmptyItem.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VMethod, "item")

VMethod::VMethod(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{}

void VMethod::determineChildren()
{
	Super::determineChildren();
	setDefaultMoveCursorProxy(icon_);
}

void VMethod::initializeForms()
{
	auto titleElement = grid({{
	  item<Static>(&I::icon_, [](I* v) -> const Visualization::StaticStyle* {
		  switch (v->node()->methodKind())
		  {
			  case Method::MethodKind::Default: return &v->style()->defaultIcon();
			  case Method::MethodKind::Constructor: return &v->style()->constructorIcon();
			  case Method::MethodKind::Destructor: return &v->style()->destructorIcon();
			  case Method::MethodKind::Conversion: return &v->style()->conversionIcon();
			  case Method::MethodKind::OperatorOverload: return &v->style()->operatorOverloadIcon();
			  default: return &v->style()->defaultIcon();
		  }
	}), item<VText>(&I::name_, [](I* v){return v->node()->nameNode();}, &StyleType::name)}})
		->setNoBoundaryCursors([](Item*){return true;})
		->setNoInnerCursors([](Item*){return true;})
		->setVerticalAlignment(LayoutStyle::Alignment::Center)
		->setHorizontalSpacing(5);

	auto titleBackgroundElement = item<EmptyItem>(&I::titleBackground_, &StyleType::titleBackground);

	auto headerElement = (new GridLayoutFormElement{})
		->setHorizontalSpacing(5)->setVerticalAlignment(LayoutStyle::Alignment::Center)->setColumnStretchFactor(3, 1)
		->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
		->put(0, 0, item<VList>(&I::typeArguments_, [](I* v){return v->node()->typeArguments();},
											[](I* v){return &v->style()->typeArguments();}))
		->put(1, 0, item<VList>(&I::arguments_, [](I* v){return v->node()->arguments();},
											[](I* v){return &v->style()->arguments();}))
		->put(2, 0, item<VList>(&I::results_, [](I* v){return v->node()->results();},
										[](I* v){return &v->style()->results();}));

	auto addonsElement = (new SequentialLayoutFormElement{})
								->setVertical()
								->setListOfItems([](Item* i){return (static_cast<VMethod*>(i))->addOnItems().values();});

	auto annotationsElement = item<VStatementItemList>(&I::annotations_, [](I* v)
											{return v->node()->annotations()->size() > 0 ? v->node()->annotations() : nullptr;},
											[](I* v){return &v->style()->annotations();});
	auto memberInitializersElement = item<VList>(&I::memberInitializers_, [](I* v)
											{return v->node()->memberInitializers()->size() > 0 ?
													v->node()->memberInitializers() : nullptr;},
											[](I* v){return &v->style()->memberInitializers();});
	auto throwsElement = item<VList>(&I::throws_,
												[](I* v){return v->node()->throws()->size() > 0 ? v->node()->throws() : nullptr;},
												[](I* v){return &v->style()->throws();});

	auto comment = item(&I::comment_, [](I* v){return v->node()->comment();});

	auto signatureLineElement = item<Line>(&I::signatureLine_, [](I* v){return &v->style()->signatureLine();});

	auto bodyElement = item(&I::body_, [](I* v){return v->node()->items();});

	auto metaCallsElement = item<VList>(&I::metaCalls_, [](I* v){return v->node()->metaCalls();},
			[](I* v){return &v->style()->metaCalls();});

	auto contentElement = (new GridLayoutFormElement{})
			->setVerticalSpacing(3)->setColumnStretchFactors(1)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
			->put(0, 0, addonsElement)
			->put(0, 1, (new GridLayoutFormElement{})->setColumnStretchFactor(1, 1)->setHorizontalSpacing(3)
					->put(0, 0, item<Static>(&I::annotationsIcon_,
							 [](I* v){return &v->style()->annotationsIcon();})->setEnabled(
									 [](I* v){return v->node()->annotations()->size() > 0;}))
					->put(1, 0, annotationsElement)
					->put(0, 1, item<Static>(&I::throwsIcon_,
							[](I* v){return &v->style()->throwsIcon();})->setEnabled(
									 [](I* v){return v->node()->throws()->size() > 0;}))
					->put(1, 1, throwsElement))
			->put(0, 3, comment)
			->put(0, 4, signatureLineElement)
			->put(0, 5, memberInitializersElement)
			->put(0, 6, metaCallsElement)
			->put(0, 7, bodyElement);

	auto shapeElement = new ShapeFormElement{};

	addForm((new AnchorLayoutFormElement{})
			// place the title background
			->put(TheLeftOf, titleBackgroundElement, 3, FromLeftOf, titleElement)
			->put(TheRightOf, titleBackgroundElement, 3, FromRightOf, titleElement)
			->put(TheTopOf, titleBackgroundElement, 3, FromTopOf, titleElement)
			->put(TheBottomOf, titleBackgroundElement, 3, FromBottomOf, titleElement)

			->put(TheLeftOf, headerElement, 10, FromRightOf, titleBackgroundElement)
			->put(TheVCenterOf, headerElement, AtVCenterOf, titleBackgroundElement)

			->put(TheLeftOf, shapeElement, AtLeftOf, titleBackgroundElement)
			->put(TheTopOf, shapeElement, -1, FromVCenterOf, titleBackgroundElement)
			->put(TheLeftOf, shapeElement, 10, FromLeftOf, contentElement)
			->put(TheRightOf, contentElement, AtRightOf, headerElement)
			->put(TheTopOf, contentElement, 10, FromBottomOf, titleBackgroundElement)
			->put(TheRightOf, shapeElement, 10, FromRightOf, contentElement)
			->put(TheBottomOf, shapeElement, 15, FromBottomOf, contentElement));

}

}
