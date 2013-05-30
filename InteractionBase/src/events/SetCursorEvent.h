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

#pragma once

#include "../interactionbase_api.h"

#include "VisualizationBase/src/CustomSceneEvent.h"

namespace Visualization {
	class Item;
	class Scene;
}

namespace Model {
	class Node;
}

namespace Interaction {

class INTERACTIONBASE_API SetCursorEvent : public Visualization::CustomSceneEvent{
	public:
		static const QEvent::Type EventType;

		enum CursorPlacement { CursorOnTop, CursorOnBottom, CursorOnLeft, CursorOnRight, CursorOnCenter,
										CursorAboveOf, CursorBelowOf, CursorLeftOf, CursorRightOf, CursorDefault};

		typedef std::function<Visualization::Item* ()> GetItemFunction;
		typedef std::function<CursorPlacement ()> GetCursorPlacement;

		SetCursorEvent(Visualization::Item* itemToGetCursor, CursorPlacement placement = CursorDefault,
				bool showPrompt = false);
		SetCursorEvent(Visualization::Item* itemToGetCursor, QPoint point, bool showPrompt = false);
		SetCursorEvent(GetItemFunction getItemToFocus, CursorPlacement placement = CursorDefault,
				bool showPrompt = false);
		SetCursorEvent(GetItemFunction getItemToFocus, GetCursorPlacement getCursorPlacement,
				bool showPrompt = false);
		SetCursorEvent(Visualization::Item* parentContainer, Model::Node* node,
				CursorPlacement placement = CursorDefault, bool showPrompt = false);
		SetCursorEvent(Visualization::Scene* scene, Model::Node* node, CursorPlacement placement = CursorDefault,
				bool showPrompt = false);
		virtual void execute();

	private:
		Visualization::Item* itemToGetCursor_{};
		Visualization::Item* parentContainer_{};
		GetItemFunction getItemToFocus_{};
		GetCursorPlacement getCursorPlacement_{};
		Visualization::Scene* scene_{};
		Model::Node* node_{};
		CursorPlacement placement_{};
		QPoint point_{-1,-1};
		bool showPrompt_{false};
};

} /* namespace Interaction */
