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

#include "VReferenceExpression.h"

#include "OOModel/src/types/PointerType.h"

#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VReferenceExpression, "item")

VReferenceExpression::VReferenceExpression(Item* parent, NodeType* node, const StyleType* style) :
	Super{parent, node, style},
	name_{new VOOReference{layout(), node->ref(), &style->unresolvedName()} },
	separator_{},
	prefix_{},
	typeArguments_{}
{
	layout()->append(name_);
}

VReferenceExpression::~VReferenceExpression()
{
	// These were automatically deleted by LayoutProvider's destructor
	name_ = nullptr;
	separator_ = nullptr;
	prefix_ = nullptr;
	typeArguments_ = nullptr;
}

void VReferenceExpression::determineChildren()
{
	layout()->synchronizeFirst(prefix_, node()->prefix());

	// TODO: In C++ we should have this->... In Java it should be this. ...
	auto* separatorStyle = &style()->standardSeparator();
	if (node()->memberKind() == OOModel::ReferenceExpression::MemberKind::Pointer)
		separatorStyle = &style()->pointerSeparator();
	else if (node()->memberKind() == OOModel::ReferenceExpression::MemberKind::Static)
		separatorStyle = &style()->staticSeparator();
	else if (node()->memberKind() == OOModel::ReferenceExpression::MemberKind::Template)
		separatorStyle = &style()->templateSeparator();


	layout()->synchronizeMid(separator_, node()->prefix() != nullptr, separatorStyle, 1);
	layout()->synchronizeLast(typeArguments_, node()->typeArguments()->size() > 0 ? node()->typeArguments() : nullptr,
			&style()->typeArguments());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());

	auto nameStyle = &style()->resolvedName();
	if (name_->node()->name() == "this") nameStyle = &style()->thisIdentifier();
	else if (name_->node()->name() == "class") nameStyle = &style()->classIdentifier();
	else if (style()->showUnresolved() && !node()->ref()->isResolved()) nameStyle = &style()->unresolvedName();
	name_->setStyle( nameStyle );
	if (prefix_) separator_->setStyle( separatorStyle );
	if (typeArguments_)
	{
		typeArguments_->setStyle(&style()->typeArguments());
		typeArguments_->setSuppressDefaultRemovalHandler(true);
	}
}

}
