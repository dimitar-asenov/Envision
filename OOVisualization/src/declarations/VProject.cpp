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

#include "VProject.h"

#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"


using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VProject, "item")

VProject::VProject(Item* parent, NodeType* node, const StyleType* style) :Super{parent, node, style}
{
}

void VProject::determineChildren()
{
	Super::determineChildren();
	setDefaultMoveCursorProxy(name_);
}

void VProject::initializeForms()
{
	auto headerElement = (new GridLayoutFormElement{})
				->setHorizontalSpacing(10)
				->setColumnStretchFactor(0, 1)
				->setColumnStretchFactor(3, 1)
				->setVerticalAlignment(LayoutStyle::Alignment::Center)
				->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
				->put(1, 0, item<Static>(&I::icon_, &StyleType::icon))
				->put(2, 0, item<VText>(&I::name_, &NodeType::nameNode, &StyleType::name));

	auto contentElement = (new GridLayoutFormElement{})
				->setSpacing(5)
				->setColumnStretchFactor(1, 1)
				->setNoBoundaryCursors([](Item*){return true;})
				->setNoInnerCursors([](Item*){return true;})
				->put(0, 0, item<VList>(&I::libraries_,
						[](I* v) {return v->node()->libraries()->size() > 0 ? v->node()->libraries() : nullptr;},
						&StyleType::libraries))
				->put(0, 1, item<VList>(&I::fields_,
						[](I* v) {return v->node()->fields()->size() > 0 ? v->node()->fields() : nullptr;},
						&StyleType::fields))
				->put(0, 2, item<VList>(&I::declarations_,
						[](I* v) {return v->node()->subDeclarations()->size() > 0 ? v->node()->subDeclarations() : nullptr;},
						&StyleType::declarations))
				->put(0, 3, item<VList>(&I::metaCalls_,
						[](I* v) {return v->node()->metaCalls();},
						&StyleType::metaCalls))
				->put(0, 4, item(&I::comment_, [](I* v){return v->node()->comment();}))
				->put(0, 5, (new DynamicGridFormElement{})->setSpacing(50, 50)->setMargins(50)
						->setMajorAxis(Visualization::GridLayouter::ColumnMajor)
						->setNodesGetter(
							[](Item* v)->QVector<QVector<Model::Node*>>{
							auto self = static_cast<I*>(v);
							return Visualization::GridLayouter::arrange(
									self->node()->projects()->nodes() + self->node()->modules()->nodes() +
									self->node()->classes()->nodes() + self->node()->methods()->nodes(),
									Visualization::GridLayouter::ColumnMajor);
						}));


	addForm((new AnchorLayoutFormElement{})
		// place the top left corner of the content element
		->put(TheLeftOf, headerElement, 10, FromLeftOf, contentElement)
		->put(TheTopOf, contentElement, 20, FromBottomOf, headerElement)
		// align content and header on their right side
		->put(TheRightOf, contentElement, AtRightOf, headerElement));
}

void VProject::updateGeometry(int availableWidth, int availableHeight)
{
// As it turns out, scaling the header triggers a complete repaint of the entire content of the project. This is rather
// slow, so for now, we disable it.
//
//	if (DCast<RootItem>(parent()))
//	{
//		qreal invGeometricZoomScale = 1 / mainViewScalingFactor();
//		if (icon_->scale() != invGeometricZoomScale)
//		{
//			icon_->setScale(invGeometricZoomScale);
//			name_->setScale(invGeometricZoomScale);
//		}
//	}

	Super::updateGeometry(availableWidth, availableHeight);
}

bool VProject::isSensitiveToScale() const
{
	return !parent() || DCast<RootItem>(parent());
}

}
