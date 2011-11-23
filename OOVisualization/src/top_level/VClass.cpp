/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * VClass.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "top_level/VClass.h"
#include "OOVisualizationException.h"

#include "VisualizationBase/headers/layouts/PanelBorderLayout.h"
#include "VisualizationBase/headers/layouts/PositionLayout.h"
#include "VisualizationBase/headers/items/VText.h"
#include "VisualizationBase/headers/items/VList.h"
#include "VisualizationBase/headers/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VClass, "item")

VClass::VClass(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<PanelBorderLayout>, Class>(parent, node, style),
	header_( new SequentialLayout(nullptr, &style->header()) ),
	icon_( new Static(nullptr, &style->icon()) ),
	name_(new VText(nullptr, node->nameNode(), &style->nameDefault()) ),
	baseClasses_( new VList(nullptr, node->baseClasses(), &style->baseClasses()) ),
	content_( new PositionLayout(nullptr, &style->content()) ),
	fieldContainer_(new SequentialLayout(nullptr, &style->fieldContainer()) ),
	publicFieldArea_(new SequentialLayout(nullptr, &style->publicFieldArea()) ),
	privateFieldArea_(new SequentialLayout(nullptr, &style->privateFieldArea()) ),
	protectedFieldArea_(new SequentialLayout(nullptr, &style->protectedFieldArea()) ),
	defaultFieldArea_(new SequentialLayout(nullptr, &style->defaultFieldArea()) )
{
	layout()->setTop(true);
	layout()->top()->setFirst(header_);
	header_->append(icon_);
	header_->append(name_);
	header_->append(baseClasses_);

	layout()->setLeft(true);
	layout()->left()->setFirst(fieldContainer_);
	fieldContainer_->append(publicFieldArea_);
	fieldContainer_->append(protectedFieldArea_);
	fieldContainer_->append(defaultFieldArea_);
	fieldContainer_->append(privateFieldArea_);

	layout()->setContent(content_);
}

VClass::~VClass()
{
	// These were automatically deleted by LayoutProvider's destructor
	header_ = nullptr;
	icon_ = nullptr;
	name_ = nullptr;
	baseClasses_ = nullptr;
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
	content_->setStyle( &style()->content() );
	baseClasses_->setStyle( &style()->baseClasses() );
	fieldContainer_->setStyle( &style()->fieldContainer() );
	publicFieldArea_->setStyle( &style()->publicFieldArea() );
	privateFieldArea_->setStyle( &style()->privateFieldArea() );
	protectedFieldArea_->setStyle( &style()->protectedFieldArea() );
	defaultFieldArea_->setStyle( &style()->defaultFieldArea() );

	// Synchronize header
	header_->synchronizeMid(name_, node()->nameNode(), nameStyle, 1);
	header_->synchronizeLast(baseClasses_, node()->baseClasses(), &style()->baseClasses());

	// Synchronize methods
	content_->synchronizeWithNodes(node()->methods()->nodes().toList(), renderer());

	// Synchronize fields
	QList<Model::Node*> publicFields;
	QList<Model::Node*> privateFields;
	QList<Model::Node*> protectedFields;
	QList<Model::Node*> defaultFields;
	for (int i = 0; i< node()->fields()->size(); ++i)
	{
		if (node()->fields()->at(i)->visibility() == Visibility::PUBLIC) publicFields.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->visibility() == Visibility::PRIVATE) privateFields.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->visibility() == Visibility::PROTECTED) protectedFields.append(node()->fields()->at(i));
		else if (node()->fields()->at(i)->visibility() == Visibility::DEFAULT) defaultFields.append(node()->fields()->at(i));
		else throw OOVisualizationException("Unknown visibility value when updating VClass instance.");
	}

	publicFieldArea_->synchronizeWithNodes(publicFields, renderer());
	privateFieldArea_->synchronizeWithNodes(privateFields, renderer());
	protectedFieldArea_->synchronizeWithNodes(protectedFields, renderer());
	defaultFieldArea_->synchronizeWithNodes(defaultFields, renderer());
}

}
