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

#include "VField.h"
#include "OOVisualizationException.h"

#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VField, "item")

VField::VField(Item* parent, NodeType* node, const StyleType* style) :
	Super(parent, node, style),
	name_(new VText(layout(), node->nameNode(), &style->nameDefault()) )
{
	layout()->append(name_);
}

VField::~VField()
{
	// These were automatically deleted by LayoutProvider's destructor
	name_ = nullptr;
	type_ = nullptr;
	assignmentSymbol_ = nullptr;
	initialValue_ = nullptr;
}

void VField::determineChildren()
{
	const TextStyle* nameStyle = nullptr;
	auto modifiers = node()->modifiers();
	if (modifiers->isSet(Modifier::Static))
	{
		if (modifiers->isSet(Modifier::Private)) nameStyle = &style()->nameStaticPrivate();
		else if (modifiers->isSet(Modifier::Protected)) nameStyle = &style()->nameStaticProtected();
		else if (modifiers->isSet(Modifier::Public)) nameStyle = &style()->nameStaticPublic();
		else nameStyle = &style()->nameStaticDefault();
	}
	else
	{
		if (modifiers->isSet(Modifier::Private)) nameStyle = &style()->namePrivate();
		else if (modifiers->isSet(Modifier::Protected)) nameStyle = &style()->nameProtected();
		else if (modifiers->isSet(Modifier::Public)) nameStyle = &style()->namePublic();
		else nameStyle = &style()->nameDefault();
	}

	layout()->synchronizeFirst(type_, node()->typeExpression());
	layout()->synchronizeMid(name_, node()->nameNode(), nameStyle, 1);
	layout()->synchronizeMid(assignmentSymbol_, node()->initialValue() != nullptr, &style()->assignmentSymbol(), 2);
	layout()->synchronizeLast(initialValue_, node()->initialValue());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout() );
	name_->setStyle(nameStyle);
	if (assignmentSymbol_) assignmentSymbol_->setStyle( &style()->assignmentSymbol() );
}

}
