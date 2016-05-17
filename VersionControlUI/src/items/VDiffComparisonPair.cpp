/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#include "VDiffComparisonPair.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"

#include "ModelBase/src/nodes/Text.h"

#include "VisualizationBase/src/items/VText.h"

namespace VersionControlUI
{

DEFINE_ITEM_COMMON(VDiffComparisonPair, "item")

VDiffComparisonPair::VDiffComparisonPair(Visualization::Item* parent, NodeType* node, const StyleType* style)
	: Super{parent, node, style}
{}

void VDiffComparisonPair::initializeForms()
{
	auto oldVersion = item(&I::oldVersionNode_, [](I* v) {
			return v->node()->oldVersionNode();});

	auto newVersion = item(&I::newVersionNode_, [](I* v) {
			return v->node()->newVersionNode();});

	auto oldVersionObjectPath = item<Visualization::VText>(&I::oldVersionObjectPath_, [](I* v) {
			return v->node()->oldVersionObjectPath();},
			[](I* v) {return &v->style()->oldVersionObjectPath();});

	auto newVersionObjectPath = item<Visualization::VText>(&I::newVersionObjectPath_, [](I* v) {
			return v->node()->newVersionObjectPath();},
			[](I* v) {return &v->style()->newVersionObjectPath();});

	auto componentType = item<Visualization::VText>(&I::componentType_, [](I* v) {
			return v->node()->componentType();},
			[](I* v) {return &v->style()->componentType();});

	auto infoGrid = (new Visualization::GridLayoutFormElement{})
			->setHorizontalSpacing(50)
			->setLeftMargin(10)
			->setRightMargin(10)
			->setHorizontalAlignment(Visualization::LayoutStyle::Alignment::Center)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
			->setColumnStretchFactor(0, 1)
			->setColumnStretchFactor(1, 1)
			->put(0, 0, oldVersionObjectPath)
			->put(1, 0, newVersionObjectPath)
			->put(0, 1, componentType);
	auto container = (new Visualization::GridLayoutFormElement{})
				->setHorizontalSpacing(30)
				->setVerticalSpacing(15)
				->setHorizontalAlignment(Visualization::LayoutStyle::Alignment::Center)
				->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Item*){return true;})
				->setLeftMargin(10)
				->setRightMargin(10)
				->setColumnStretchFactor(0, 1)
				->setColumnStretchFactor(1, 1)
				->setColumnHorizontalAlignment(0, Visualization::LayoutStyle::Alignment::Right)
				->setColumnHorizontalAlignment(1, Visualization::LayoutStyle::Alignment::Left)
				->put(0, 0, infoGrid)
				->setCellSpanning(2, 1)
				->put(0, 1, oldVersion)
				->put(1, 1, newVersion);

	addForm(container);

}

bool VDiffComparisonPair::isSensitiveToScale() const
{
	return true;
}

void VDiffComparisonPair::scaleVisualizations()
{
	qreal factor = Visualization::VisualizationManager::instance().mainScene()->mainViewScalingFactor();
	qreal scale;
	if (factor >= 1.0)
		scale = 1.0;
	else if (factor >= 0.03)
		scale = 1.0/factor;
	else
		scale = 1.0;

	if (componentType_)
		componentType_->setScale(scale);

	if (oldVersionObjectPath_)
		oldVersionObjectPath_->setScale(scale);

	if (newVersionObjectPath_)
		newVersionObjectPath_->setScale(scale);
}

void VDiffComparisonPair::determineChildren()
{
	Super::determineChildren();
	scaleVisualizations();
}
}
