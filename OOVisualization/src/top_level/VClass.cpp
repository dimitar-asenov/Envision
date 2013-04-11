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

#include "top_level/VClass.h"
#include "OOVisualizationException.h"

#include "../elements/VStatementItemList.h"

#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
#include "VisualizationBase/src/layouts/PositionLayout.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VClass, "item")

VClass::VClass(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<PanelBorderLayout>, Class>(parent, node, style)
{
	layout()->setTop(true);
	header_ = new SequentialLayout(layout()->top(), &style->header());
	layout()->top()->setFirst(header_);
	icon_ = new Static(header_, &style->icon());
	header_->append(icon_);
	name_ = new VText(header_, node->nameNode(), &style->nameDefault());
	header_->append(name_);
	setDefaultMoveCursorProxy(name_);
	typeArguments_ = new VList(header_, node->typeArguments(), &style->typeArguments());
	header_->append(typeArguments_);
	baseClasses_ = new VList(header_, node->baseClasses(), &style->baseClasses());
	header_->append(baseClasses_);

	layout()->setLeft(true);
	fieldContainer_ = new SequentialLayout(layout()->left(), &style->fieldContainer());
	layout()->left()->setFirst(fieldContainer_);

	publicFieldArea_ = new SequentialLayout(fieldContainer_, &style->publicFieldArea());
	fieldContainer_->append(publicFieldArea_);
	protectedFieldArea_ = new SequentialLayout(fieldContainer_, &style->protectedFieldArea());
	fieldContainer_->append(protectedFieldArea_);
	defaultFieldArea_ = new SequentialLayout(fieldContainer_, &style->defaultFieldArea());
	fieldContainer_->append(defaultFieldArea_);
	privateFieldArea_ = new SequentialLayout(fieldContainer_, &style->privateFieldArea());
	fieldContainer_->append(privateFieldArea_);

	content_ = new SequentialLayout(layout(), &style->content());
	layout()->setContent(content_);

	body_ = new PositionLayout(content_, &style->body());
	content_->append(body_);
}

VClass::~VClass()
{
	// These were automatically deleted by LayoutProvider's destructor
	header_ = nullptr;
	icon_ = nullptr;
	name_ = nullptr;
	typeArguments_ = nullptr;
	baseClasses_ = nullptr;
	annotations_ = nullptr;
	enumerators_ = nullptr;
	body_ = nullptr;
	content_ = nullptr;
	fieldContainer_ = nullptr;
	publicFieldArea_ = nullptr;
	privateFieldArea_ = nullptr;
	protectedFieldArea_ = nullptr;
	defaultFieldArea_ = nullptr;
}

void VClass::determineChildren()
{
	const TextStyle* nameStyle = nullptr;

	if (node()->visibility() == Visibility::DEFAULT) nameStyle = &style()->nameDefault();
	else if (node()->visibility() == Visibility::PUBLIC) nameStyle = &style()->namePublic();
	else if (node()->visibility() == Visibility::PRIVATE) nameStyle = &style()->namePrivate();
	else if (node()->visibility() == Visibility::PROTECTED) nameStyle = &style()->nameProtected();
	else throw OOVisualizationException("Unknown visibility in VClass::determineChildren");

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout() );
	icon_->setStyle(&style()->icon());
	header_->setStyle( &style()->header() );
	name_->setStyle( nameStyle );
	body_->setStyle( &style()->body() );
	if (annotations_) annotations_->setStyle( &style()->annotations() );
	if (enumerators_) enumerators_->setStyle( &style()->enumerators() );
	content_->setStyle( &style()->content() );
	typeArguments_->setStyle( &style()->typeArguments() );
	baseClasses_->setStyle( &style()->baseClasses() );
	fieldContainer_->setStyle( &style()->fieldContainer() );
	publicFieldArea_->setStyle( &style()->publicFieldArea() );
	privateFieldArea_->setStyle( &style()->privateFieldArea() );
	protectedFieldArea_->setStyle( &style()->protectedFieldArea() );
	defaultFieldArea_->setStyle( &style()->defaultFieldArea() );

	// Synchronize header
	header_->synchronizeMid(name_, node()->nameNode(), nameStyle, 1);
	header_->synchronizeMid(typeArguments_, node()->typeArguments(), &style()->typeArguments(), 2);
	header_->synchronizeLast(baseClasses_, node()->baseClasses(), &style()->baseClasses());

	// Synchronize methods
	if (body_->needsUpdate() == FullUpdate) body_->clear(true);
	body_->synchronizeWithNodes(node()->methods()->nodes().toList(), renderer());
	content_->synchronizeFirst(annotations_,
			node()->annotations()->size() > 0 ? node()->annotations() : nullptr, &style()->annotations());
	int index = annotations_ ? 1 : 0;
	content_->synchronizeMid(enumerators_,
			node()->enumerators()->size() > 0 ? node()->enumerators() : nullptr,	&style()->enumerators(), index);

	// Synchronize fields
	QList<Model::Node*> publicFields;
	QList<Model::Node*> privateFields;
	QList<Model::Node*> protectedFields;
	QList<Model::Node*> defaultFields;
	for (int i = 0; i< node()->fields()->size(); ++i)
	{
		if (node()->fields()->at(i)->visibility() == Visibility::PUBLIC)
			publicFields.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->visibility() == Visibility::PRIVATE)
			privateFields.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->visibility() == Visibility::PROTECTED)
			protectedFields.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->visibility() == Visibility::DEFAULT)
			defaultFields.append(node()->fields()->at(i));
		else throw OOVisualizationException("Unknown visibility value when updating VClass instance.");
	}

	publicFieldArea_->synchronizeWithNodes(publicFields, renderer());
	privateFieldArea_->synchronizeWithNodes(privateFields, renderer());
	protectedFieldArea_->synchronizeWithNodes(protectedFields, renderer());
	defaultFieldArea_->synchronizeWithNodes(defaultFields, renderer());
}

}
