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

/*
 * SetExpressionCursorEvent.cpp
 *
 *  Created on: Feb 17, 2012
 *      Author: Dimitar Asenov
 */

#include "handlers/SetExpressionCursorEvent.h"

#include "string_offset_providers/StringOffsetProvider.h"

#include "VisualizationBase/src/items/Item.h"
#include "ModelBase/src/adapter/AdapterManager.h"

namespace OOInteraction {

const QEvent::Type SetExpressionCursorEvent::EventType = static_cast<QEvent::Type> (QEvent::registerEventType());

SetExpressionCursorEvent::SetExpressionCursorEvent(Visualization::Item* parentContainer, Model::Node* node, int offset)
	: CustomSceneEvent(EventType), parentContainer_(parentContainer), node_(node), offset_(offset)
{
}

void SetExpressionCursorEvent::execute()
{
	auto nodeVis = parentContainer_->findVisualizationOf(node_);
	Q_ASSERT( nodeVis != nullptr);
	auto* sp = Model::AdapterManager::adapt<StringOffsetProvider>( nodeVis );
	if (sp)
	{
		sp->setOffset(offset_);
		SAFE_DELETE(sp);
	}
}

} /* namespace OOInteraction */
