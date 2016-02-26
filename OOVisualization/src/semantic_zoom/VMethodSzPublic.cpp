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

#include "VMethodSzPublic.h"
#include "../elements/VStatementItemList.h"
#include "../OOVisualizationException.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VMethodSzPublic, "item")

VMethodSzPublic::VMethodSzPublic(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{}

void VMethodSzPublic::initializeForms()
{
	auto headerElement = (new GridLayoutFormElement{})
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
						return &v->style()->nameStaticPublic();
					else return &v->style()->namePublic();
				}))
		->put(3, 0, item<VList>(&I::typeArguments_, [](I* v){return v->node()->typeArguments();},
											[](I* v){return &v->style()->typeArguments();}))
		->put(4, 0, item<VList>(&I::arguments_, [](I* v){return v->node()->arguments();},
											[](I* v){return &v->style()->arguments();}));

	auto shapeElement = new ShapeFormElement{};

	addForm((new AnchorLayoutFormElement{})
			->put(TheLeftOf, shapeElement, AtLeftOf, headerElement)
			->put(TheTopOf, shapeElement, AtCenterOf, headerElement)
			->put(TheRightOf, shapeElement, 10, FromRightOf, headerElement)
			->put(TheBottomOf, shapeElement, 10, FromBottomOf, headerElement));
}

}
