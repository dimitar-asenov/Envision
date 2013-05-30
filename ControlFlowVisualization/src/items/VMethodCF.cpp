/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "items/VMethodCF.h"
#include "ControlFlowVisualizationException.h"

#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOVisualization;
using namespace OOModel;

namespace ControlFlowVisualization {

ITEM_COMMON_DEFINITIONS(VMethodCF, "item")

VMethodCF::VMethodCF(Item* parent, NodeType* node, const StyleType* style) :
	Super(parent, node, style),
	header_( new SequentialLayout(nullptr, &style->header()) ),
	icon_(new Static(nullptr, &style->icon())),
	name_(new VText(nullptr, node->nameNode(), &style->nameDefault()) ),
	arguments_(new VList(nullptr, node->arguments(), &style->arguments()) ),
	contentCF_(nullptr),
	results_(new VList(nullptr, node->results(), &style->results()) )
{
	layout()->setTop(true);
	layout()->top()->setFirst(header_);
	header_->append(icon_);
	header_->append(name_);
	header_->append(arguments_);

	layout()->setLeft(true);
	layout()->left()->setFirst(results_);
}

VMethodCF::~VMethodCF()
{
	// These were automatically deleted by LayoutProvider's destructor
	header_ = nullptr;
	icon_ = nullptr;
	name_ = nullptr;
	contentCF_ = nullptr;
	arguments_ = nullptr;
	results_ = nullptr;
}

void VMethodCF::determineChildren()
{
	const TextStyle* nameStyle = nullptr;
	if (node()->storageSpecifier() == StorageSpecifier::INSTANCE_VARIABLE)
	{
		if (node()->visibility() == Visibility::DEFAULT) nameStyle = &style()->nameDefault();
		else if (node()->visibility() == Visibility::PRIVATE) nameStyle = &style()->namePrivate();
		else if (node()->visibility() == Visibility::PROTECTED) nameStyle = &style()->nameProtected();
		else if (node()->visibility() == Visibility::PUBLIC) nameStyle = &style()->namePublic();
		else throw ControlFlowVisualizationException("Unknown visibility type in VMethodCF::determineChildren");
	}
	else if (node()->storageSpecifier() == StorageSpecifier::CLASS_VARIABLE)
	{
		if (node()->visibility() == Visibility::DEFAULT) nameStyle = &style()->nameStaticDefault();
		else if (node()->visibility() == Visibility::PRIVATE)nameStyle = &style()->nameStaticPrivate();
		else if (node()->visibility() == Visibility::PROTECTED) nameStyle = &style()->nameStaticProtected();
		else if (node()->visibility() == Visibility::PUBLIC) nameStyle = &style()->nameStaticPublic();
		else throw ControlFlowVisualizationException("Unknown visibility type in VMethodCF::determineChildren");
	}
	else throw ControlFlowVisualizationException("Unknown static type in VMethodCF::determineChildren");

	header_->synchronizeMid(name_, node()->nameNode(), nameStyle, 1);
	header_->synchronizeLast(arguments_, node()->arguments(), &style()->arguments());
	layout()->left()->synchronizeFirst(results_, node()->results(), &style()->results());

	layout()->synchronizeContent(contentCF_, node()->items(), &style()->contentCF());



	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout() );
	icon_->setStyle( &style()->icon());
	header_->setStyle( &style()->header() );
	name_->setStyle(nameStyle);
	contentCF_->setStyle( &style()->contentCF() );
	arguments_->setStyle( &style()->arguments() );
	results_->setStyle( &style()->results() );
}

}
