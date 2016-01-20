/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "SetExpressionCursorEvent.h"

#include "../string_offset_providers/StringOffsetProvider.h"

#include "VisualizationBase/src/items/Item.h"
#include "Core/src/AdapterManager.h"

namespace OOInteraction {

const QEvent::Type SetExpressionCursorEvent::EventType = static_cast<QEvent::Type> (QEvent::registerEventType());

SetExpressionCursorEvent::SetExpressionCursorEvent(Visualization::Item* parentContainer, Model::Node* node, int offset)
	: CustomSceneEvent{EventType}, parentContainerChain_{}, node_{node}, offset_{offset}
{
	while (parentContainer)
	{
		parentContainerChain_.prepend(parentContainer);
		parentContainer = parentContainer->parent();
	}
}

void SetExpressionCursorEvent::execute()
{
	// First start with the parent container and try to find the last descendent item that is still in the chain
	// TODO: Do not assume that the first element of the parentContainerChain_ is a valid (non-deleted) item.
	int container = 0;
	while (container+1 < parentContainerChain_.size() &&
			parentContainerChain_.at(container)->childItems().contains(parentContainerChain_.at(container+1)))
		++container;


	int mostSpecificContainer = container;
	// Now start from the last found container and go up the chain, until the visualization of the node is found.
	while (container >=0)
	{
		auto nodeVis = parentContainerChain_.at(container)->findVisualizationOf(node_);
		if (nodeVis)
		{
			auto* sp = Core::AdapterManager::adapt<StringOffsetProvider>( nodeVis );
			if (sp)
			{
				sp->setOffset(offset_);
				SAFE_DELETE(sp);
			}
			return;
		}
		--container;
	}

	// A visualization was not found, repost this event, as most likely there are some updates pending.
	// TODO: This is quite a hack, ideally we shouldn't have to do this at all, but the currently broken Qt event
	// handling makes our life harder.
	if (!reposted_)
	{
		auto event = new SetExpressionCursorEvent{parentContainerChain_.at(mostSpecificContainer), node_, offset_};
		event->reposted_ = true;
		qApp->postEvent(parentContainerChain_.first()->scene(), event);
	}
}

}
