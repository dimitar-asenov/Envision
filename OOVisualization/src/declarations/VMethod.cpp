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
#include "OOVisualizationException.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VMethod, "item")

VMethod::VMethod(Item* parent, NodeType* node, const StyleType* style) : Super(parent, node, style)
{}

void VMethod::initializeForms()
{
	auto headerElement = (new GridLayoutFormElement())
		->setHorizontalSpacing(3)->setVerticalAlignment(LayoutStyle::Alignment::Center)->setColumnStretchFactor(4, 1)
		->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
		->put(0, 0, item<Static>(&I::icon_, [](I* v)
				{
					switch (v->node()->methodKind())
					{
						case Method::MethodKind::Default: return &v->style()->defaultIcon();
						case Method::MethodKind::Constructor: return &v->style()->constructorIcon();
						case Method::MethodKind::Destructor: return &v->style()->destructorIcon();
						default: return &v->style()->defaultIcon();
					}
				}))
		->put(1, 0, item<VList>(&I::results_, [](I* v){return v->node()->results();},
											[](I* v){return &v->style()->results();}))
		->put(2, 0, item<VText>(&I::name_, [](I* v){return v->node()->nameNode();}, [](I* v)
				{
					// return the correct name style according to method's modifiers
					auto modifiers = v->node()->modifiers();
					if (modifiers->isSet(Modifier::Static))
					{
						if (modifiers->isSet(Modifier::Private)) return &v->style()->nameStaticPrivate();
						else if (modifiers->isSet(Modifier::Protected)) return &v->style()->nameStaticProtected();
						else if (modifiers->isSet(Modifier::Public)) return &v->style()->nameStaticPublic();
						else return &v->style()->nameStaticDefault();
					}
					else
					{
						if (modifiers->isSet(Modifier::Private)) return &v->style()->namePrivate();
						else if (modifiers->isSet(Modifier::Protected)) return &v->style()->nameProtected();
						else if (modifiers->isSet(Modifier::Public)) return &v->style()->namePublic();
						else return &v->style()->nameDefault();
					}
				}))
		->put(3, 0, item<VList>(&I::typeArguments_, [](I* v){return v->node()->typeArguments();},
											[](I* v){return &v->style()->typeArguments();}))
		->put(4, 0, item<VList>(&I::arguments_, [](I* v){return v->node()->arguments();},
											[](I* v){return &v->style()->arguments();}));

	auto addonsElement = (new SequentialLayoutFormElement())
								->setVertical()
								->setListOfItems([](Item* i){return (static_cast<VMethod*>(i))->addOnItems().values();});

	auto annotationsElement = item<VStatementItemList>(&I::annotations_, [](I* v)
											{return v->node()->annotations()->size() > 0 ? v->node()->annotations() : nullptr;},
											[](I* v){return &v->style()->annotations();});
	auto memberInitializersElement = item<VList>(&I::memberInitializers_, [](I* v)
											{return v->node()->memberInitializers()->size() > 0 ?
													v->node()->memberInitializers() : nullptr;},
											[](I* v){return &v->style()->memberInitializers();});

	auto signatureLineElement = item<Line>(&I::signatureLine_, [](I* v){return &v->style()->signatureLine();});

	auto bodyElement = item(&I::body_, [](I* v){return v->node()->items();});

	auto contentElement = (new GridLayoutFormElement())
			->setVerticalSpacing(3)->setColumnStretchFactors(1)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
			->put(0, 0, addonsElement)
			->put(0, 1, annotationsElement)
			->put(0, 2, signatureLineElement)
			->put(0, 3, memberInitializersElement)
			->put(0, 4, bodyElement);

	auto shapeElement = new ShapeFormElement();

	addForm((new AnchorLayoutFormElement())
			->put(TheLeftOf, shapeElement, AtLeftOf, headerElement)
			->put(TheTopOf, shapeElement, AtCenterOf, headerElement)
			->put(TheLeftOf, shapeElement, 10, FromLeftOf, contentElement)
			->put(TheRightOf, contentElement, AtRightOf, headerElement)
			->put(TheTopOf, contentElement, 10,FromBottomOf, headerElement)
			->put(TheRightOf, shapeElement, 10, FromRightOf, contentElement)
			->put(TheBottomOf, shapeElement, 10, FromBottomOf, contentElement));
}

}
