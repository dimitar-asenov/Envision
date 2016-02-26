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

#include "VDeclarationSz.h"
#include "../OOVisualizationException.h"

#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Project.h"

#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

#include "ModelBase/src/nodes/Node.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VDeclarationSz, "item")

VDeclarationSz::VDeclarationSz(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{
	if (DCast<Method>(node))
		setStyle(itemStyles().get("method"));
	else if (DCast<Class>(node))
		setStyle(itemStyles().get("class"));
	else if (DCast<Module>(node))
		setStyle(itemStyles().get("module"));
	else if (DCast<Project>(node))
		setStyle(itemStyles().get("project"));

	setDefaultMoveCursorProxy(name_);
}

void VDeclarationSz::initializeForms()
{
	auto content = (new GridLayoutFormElement{})
				->setHorizontalSpacing(5)->setColumnStretchFactor(3, 1)
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
				->put(0, 0, item<Static>(&I::icon_, [](I* v) -> const Visualization::StaticStyle* {
						return &v->style()->icon();
					}))
				->put(1, 0, item<VText>(&I::name_, [](I* v){return v->node()->nameNode();}, [](I* v) {
						return &v->style()->name();
					}));

	auto shapeElement = new ShapeFormElement{};

	addForm((new AnchorLayoutFormElement{})
				->put(TheTopOf, content, 10, FromTopOf, shapeElement)
				->put(TheLeftOf, shapeElement, 10, FromLeftOf, content)
				->put(TheBottomOf, content, 10, FromBottomOf, shapeElement)
				->put(TheRightOf, shapeElement, 0, FromRightOf, content));
}

void VDeclarationSz::updateGeometry(int availableWidth, int availableHeight)
{
	Super::updateGeometry(availableWidth, availableHeight);

	if (!DCast<RootItem>(parent())) return;

	qreal invGeometricZoomScale = 1 / mainViewScalingFactor();

	if (scale() != invGeometricZoomScale) setScale(invGeometricZoomScale);
}

bool VDeclarationSz::isSensitiveToScale() const
{
	return true;
}

}
