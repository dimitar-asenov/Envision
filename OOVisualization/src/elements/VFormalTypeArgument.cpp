/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "VFormalTypeArgument.h"

#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VFormalTypeArgument, "item")

VFormalTypeArgument::VFormalTypeArgument(Item* parent, NodeType* node, const StyleType* style) :
	Super(parent, node, style),
	name_(),
	subType_(),
	superType_(),
	subSymbol_(),
	superSymbol_(),
	subBackground_(),
	superBackground_()
{
}

VFormalTypeArgument::~VFormalTypeArgument()
{
	// These were automatically deleted by LayoutProvider's destructor
	name_ = nullptr;
	subType_ = nullptr;
	superType_ = nullptr;
	subSymbol_ = nullptr;
	superSymbol_ = nullptr;
	subBackground_ = nullptr;
	superBackground_ = nullptr;
}

void VFormalTypeArgument::determineChildren()
{
	layout()->synchronizeFirst(name_, node()->nameNode(), &style()->name());
	layout()->synchronizeMid(subBackground_, node()->subTypeOfExpression(), &style()->subLayout(), 1);
	layout()->synchronizeLast(superBackground_, node()->superTypeOfExpression(), &style()->superLayout());

	if (!subBackground_)
	{
		subSymbol_ = nullptr;
		subType_ = nullptr;
	}

	if (!superBackground_)
	{
		superBackground_ = nullptr;
		superType_ = nullptr;
	}

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout() );
	name_->setStyle( &style()->name());
	if (node()->subTypeOfExpression())
	{
		subBackground_->synchronizeFirst(subSymbol_, true, &style()->subTypeSymbol());
		subBackground_->synchronizeLast(subType_, node()->subTypeOfExpression());
		subBackground_->setStyle(&style()->subLayout());
		subSymbol_->setStyle(&style()->subTypeSymbol());
	}
	if (node()->superTypeOfExpression())
	{
		superBackground_->synchronizeFirst(superSymbol_, true, &style()->superTypeSymbol());
		superBackground_->synchronizeLast(superType_, node()->superTypeOfExpression());
		superBackground_->setStyle(&style()->superLayout());
		superSymbol_->setStyle(&style()->superTypeSymbol());
	}
}

} /* namespace OOVisualization */
