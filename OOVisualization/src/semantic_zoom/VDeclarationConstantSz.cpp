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

#include "VDeclarationConstantSz.h"
#include "../OOVisualizationException.h"

#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Project.h"

#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/node_extensions/FullDetailSize.h"

#include "ModelBase/src/nodes/Node.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VDeclarationConstantSz, "item")

VDeclarationConstantSz::VDeclarationConstantSz(Item* parent, NodeType* node, const StyleType* style)
: Super{parent, node, style}
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

void VDeclarationConstantSz::initializeForms()
{
	auto stretchItem = item<EmptyItem>(&I::stretchItem_, [](I* v){return &v->style()->stretchItem();});

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

	addForm((new AnchorLayoutFormElement{})
				->put(TheVCenterOf, content, AtVCenterOf, stretchItem)
				->put(TheHCenterOf, content, AtHCenterOf, stretchItem));
}

void VDeclarationConstantSz::updateGeometry(int availableWidth, int availableHeight)
{
	auto fds = node()->extension<FullDetailSize>();
	if (fds && fds->widthNode() && fds->heightNode())
	{
		qreal totalWidth = fds->width();
		qreal totalHeight = fds->height();

		stretchItem_->setCustomSize(totalWidth, totalHeight);

		qreal localWidth = icon_->widthInLocal() + 5 + name_->widthInLocal() + 10;
		qreal localHeight = icon_->heightInLocal() > name_->heightInLocal() ? icon_->heightInLocal() :
																									 name_->heightInLocal();

		qreal hRatio = totalWidth / localWidth;
		qreal vRatio = totalHeight / localHeight;

		qreal ratio = hRatio > vRatio ? vRatio : hRatio;

		qreal geometricZoomScale = mainViewScalingFactor();
		qreal perceivedScale = geometricZoomScale * ratio * totalScale();
		qreal maxScale = geometricZoomScale < 1 ? 1 : geometricZoomScale;

		qreal scale = (perceivedScale >= maxScale ? maxScale / geometricZoomScale / totalScale() : ratio);

		if (name_->scale() != scale) name_->setScale(scale);
		if (icon_->scale() != scale) icon_->setScale(scale);
	}

	Super::updateGeometry(availableWidth, availableHeight);

	if (!DCast<RootItem>(parent())) return;

	qreal invGeometricZoomScale = 1 / mainViewScalingFactor();

	if (scale() != invGeometricZoomScale) setScale(invGeometricZoomScale);
}

bool VDeclarationConstantSz::isSensitiveToScale() const
{
	return true;
}

}
