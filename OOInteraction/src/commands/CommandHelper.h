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

#pragma once

#include "../oointeraction_api.h"

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Field.h"

#include "InteractionBase/src/events/SetCursorEvent.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/cursor/LayoutCursor.h"
#include "VisualizationBase/src/declarative/GridLayouter.h"
#include "VisualizationBase/src/views/MainView.h"

class OOINTERACTION_API CommandHelper
{
	public:

		template <typename Parent, typename ChildContainer, typename Child>
		static void addToParent(Parent* p, ChildContainer* container, Child* child,
				QVector<Model::Node*> existingChildren, Visualization::Item* target,
				const std::unique_ptr<Visualization::Cursor>& cursor, bool showPrompt = true)
		{
			p->beginModification("create " + child->typeName());
			if (auto layc = dynamic_cast<Visualization::LayoutCursor*>(cursor.get()))
			{
				Visualization::GridLayouter::setPositionInGrid( existingChildren, layc->x(), layc->y(),
						child, Visualization::GridLayouter::ColumnMajor);
			}
			else if (container->isEmpty())
			{
				Visualization::GridLayouter::setPositionInGrid( existingChildren, 0, 0,
										child, Visualization::GridLayouter::ColumnMajor);
			}
			container->append(child);
			p->endModification();

			target->setUpdateNeeded(Visualization::Item::StandardUpdate);
			target->scene()->addPostEventAction([child](){
				centerOnItem(Visualization::Item::nodeItemsMap().value(child, nullptr));
			});
			target->scene()->addPostEventAction(new Interaction::SetCursorEvent{target, child,
					Interaction::SetCursorEvent::CursorDefault, showPrompt});
		}

		template <typename Child>
		static void addFreshTree(Child* child, Visualization::Item* target, bool showPrompt = true)
		{
			auto manager = new Model::TreeManager{};
			manager->setRoot(child);

			target->scene()->currentViewItem()->insertNode(child);
			target->scene()->listenToTreeManager(manager);

			target->setUpdateNeeded(Visualization::Item::StandardUpdate);
			target->scene()->addPostEventAction([child](){
				centerOnItem(Visualization::Item::nodeItemsMap().value(child, nullptr));
			});
			target->scene()->addPostEventAction(new Interaction::SetCursorEvent{target->scene(), child,
					Interaction::SetCursorEvent::CursorDefault, showPrompt});
		}

		static void centerOnItem(Visualization::Item* target);
};

inline void CommandHelper::centerOnItem(Visualization::Item* target)
{
	if (target)
		for (auto view : target->scene()->views())
			if (auto mainView = dynamic_cast<Visualization::MainView*>(view) )
				mainView->centerOn(target->mapToScene(target->boundingRect().center()));
}
