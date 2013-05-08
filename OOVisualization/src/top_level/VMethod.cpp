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

#include "VMethod.h"
#include "../elements/VStatementItemList.h"
#include "OOVisualizationException.h"

#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Line.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VMethod, "item")

VMethod::VMethod(Item* parent, NodeType* node, const StyleType* style) : Super(parent, node, style)
{
	layout()->setTop(true);

	header_ = new SequentialLayout(layout()->top(), &style->header());
	layout()->top()->setFirst(header_);

	icon_ = new Static(header_, &style->icon());
	header_->append(icon_);

	results_ =new VList(header_, node->results(), &style->results());
	header_->append(results_);

	name_ =new VText(header_, node->nameNode(), &style->nameDefault());
	header_->append(name_);
	setDefaultMoveCursorProxy(name_);

	typeArguments_ =new VList(header_, node->typeArguments(), &style->arguments());
	header_->append(typeArguments_);

	arguments_ =new VList(header_, node->arguments(), &style->arguments());
	header_->append(arguments_);

	content_ = new SequentialLayout(layout(), &style->content());
	layout()->setContent(content_);

	signatureLine_ = new Line(content_, &style->signatureLine());
	content_->append(signatureLine_);

	body_ = new VStatementItemList(content_, node->items(), &style->body());
	content_->append(body_);
}

VMethod::~VMethod()
{
	// These were automatically deleted by LayoutProvider's destructor
	header_ = nullptr;
	icon_ = nullptr;
	name_ = nullptr;
	body_ = nullptr;
	signatureLine_ = nullptr;
	annotations_ = nullptr;
	addons_ = nullptr;
	content_ = nullptr;
	typeArguments_ = nullptr;
	arguments_ = nullptr;
	results_ = nullptr;
}

void VMethod::determineChildren()
{
	const TextStyle* nameStyle = nullptr;
	if (node()->storageSpecifier() == StorageSpecifier::INSTANCE_VARIABLE)
	{
		if (node()->visibility() == Visibility::DEFAULT) nameStyle = &style()->nameDefault();
		else if (node()->visibility() == Visibility::PRIVATE) nameStyle = &style()->namePrivate();
		else if (node()->visibility() == Visibility::PROTECTED) nameStyle = &style()->nameProtected();
		else if (node()->visibility() == Visibility::PUBLIC) nameStyle = &style()->namePublic();
		else throw OOVisualizationException("Unknown visibility type in VMethod::determineChildren");
	}
	else if (node()->storageSpecifier() == StorageSpecifier::CLASS_VARIABLE)
	{
		if (node()->visibility() == Visibility::DEFAULT) nameStyle = &style()->nameStaticDefault();
		else if (node()->visibility() == Visibility::PRIVATE)nameStyle = &style()->nameStaticPrivate();
		else if (node()->visibility() == Visibility::PROTECTED) nameStyle = &style()->nameStaticProtected();
		else if (node()->visibility() == Visibility::PUBLIC) nameStyle = &style()->nameStaticPublic();
		else throw OOVisualizationException("Unknown visibility type in VMethod::determineChildren");
	}
	else throw OOVisualizationException("Unknown static type in VMethod::determineChildren");


	header_->synchronizeMid(results_, node()->results(), &style()->results(), 1);
	header_->synchronizeMid(name_, node()->nameNode(), nameStyle, results_ ? 2 : 1);
	header_->synchronizeMid(typeArguments_, node()->typeArguments(), &style()->typeArguments(), results_ ? 3 : 2);
	header_->synchronizeLast(arguments_, node()->arguments(), &style()->arguments());

	content_->synchronizeLast(body_, node()->items(), &style()->body());
	content_->synchronizeFirst(addons_, !addOnItems().isEmpty(), &style()->addons());
	if (addons_) putAddOnItemsInSequence(addons_);

	content_->synchronizeMid(annotations_,
				node()->annotations()->size() > 0 ? node()->annotations() : nullptr,
						&style()->annotations(), addons_ ? 1 : 0);

	content_->synchronizeMid(signatureLine_, addons_ || annotations_ || node()->items()->size(),
			&style()->signatureLine(), (addons_ ?1:0) + (annotations_ ?1:0));

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout() );
	icon_->setStyle( &style()->icon());
	header_->setStyle( &style()->header() );
	name_->setStyle(nameStyle);
	body_->setStyle( &style()->body() );
	if (annotations_) annotations_->setStyle( &style()->annotations() );
	content_->setStyle( &style()->content() );
	typeArguments_->setStyle( &style()->typeArguments() );
	arguments_->setStyle( &style()->arguments() );
	results_->setStyle( &style()->results() );
}

}
