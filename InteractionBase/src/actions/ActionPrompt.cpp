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

#include "ActionPrompt.h"
#include "../vis/TextAndDescription.h"
#include "Action.h"

#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/overlays/SelectionOverlay.h"
#include "VisualizationBase/src/cursor/Cursor.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/CustomSceneEvent.h"

#include "ModelBase/src/nodes/composite/CompositeNode.h"

using namespace Visualization;

namespace Interaction {

ITEM_COMMON_DEFINITIONS(ActionPrompt, "item")

ActionPrompt::ActionPrompt(Item* actionReceiver,  bool autoExecuteAction, const StyleType* style) :
	Super{nullptr, style},
	autoExecuteAction_{autoExecuteAction},
	originalActionReceiver_{actionReceiver},
	currentActionReceiver_{actionReceiver}
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(ItemIgnoresTransformations);
	setZValue(LAYER_COMMAND);
	setItemCategory(Scene::MenuItemCategory);

	computeCurrentActionReceiver();
	originalActionReceiver_->scene()->addTopLevelItem(this);
	setHighlight(true);

	setPromptPosition();
	scene()->addPostEventAction(new CustomSceneEvent{ [this](){
		this->acquireCursor();
		this->setReceiverName();
	}});
}

ActionPrompt::~ActionPrompt()
{
	if (scene()) scene()->removeTopLevelItem(this);

	// These items are completely out of our control, we just know about them.
	currentActionReceiver_ = nullptr;
	originalActionReceiver_ = nullptr;

	setHighlight(false);
}

void ActionPrompt::initializeForms()
{
	addForm(grid({{
		item<Text>(&I::nodeTypeName_, [](I* v) {return &v->style()->nodeTypeName();})->setRightMargin(10),
		item<Text>(&I::actionText_, [](I* v) {return &v->style()->shortcutText();})
	},
	{
		Merge(
		(new SequentialLayoutFormElement{})->setListOfItems([](Item* item) -> QList<Item*> {
			QList<Item*> ret;
			auto self = static_cast<ActionPrompt*>(item);

			if (self->isVisible())
				for (auto a : self->actions())
				{
					if (a->shortcut().startsWith(self->actionText_->text()))
						ret.append(new TextAndDescription{a->shortcut(), a->name(), &self->style()->actionStyle()});
				}

			self->numActions_ = ret.size();
			return ret;
		})->setSpaceBetweenElements(20), 2, 1)

	}})
		->setNoInnerCursors([](Item*){return true;})
		->setColumnStretchFactor(1, 1.0)
		->setNoBoundaryCursors([](Item*){return true;})
	);
}

void ActionPrompt::setHighlight(bool show)
{
	SAFE_DELETE_ITEM(highlight_);

	if (show && currentActionReceiver_)
	{
		highlight_ = new Visualization::SelectionOverlay{currentActionReceiver_};
		Q_ASSERT(scene());
		scene()->addTopLevelItem(highlight_);
	}
}

void ActionPrompt::setReceiverName()
{
	if (currentActionReceiver_)
	{
		// Try to get a name for this node
		QString name;
		if (currentActionReceiver_->node()->definesSymbol())
			name = currentActionReceiver_->node()->symbolName();
		else if (auto cnParent = DCast<Model::CompositeNode>(currentActionReceiver_->node()->parent()))
			name = cnParent->meta().attribute(cnParent->indexOf(currentActionReceiver_->node())).name();

		if (!name.isEmpty()) name = " - <b>" + name + "</b>";
		nodeTypeName_->setText(currentActionReceiver_->node()->typeName() + name + ":" );
	}
	else nodeTypeName_->setText("None");

}

void ActionPrompt::showPrompt()
{
	parentActionsLevel_ = 0;
	computeCurrentActionReceiver();
	setHighlight(true);

	actionText_->setText("");
	setReceiverName();
	setPromptPosition();
	show();
	acquireCursor();
	setUpdateNeeded(StandardUpdate);
}

void ActionPrompt::hidePrompt()
{
	hide();
	setHighlight(false);
	originalActionReceiver_->moveCursor(Visualization::Item::MoveOnPosition, receiverCursorPosition_);
}

void ActionPrompt::acquireCursor()
{
	Q_ASSERT(actionText_);

	// Save the current cursor
	receiverCursorPosition_ = QPoint(0, 0);
	if (originalActionReceiver_->scene()->mainCursor()->owner() == originalActionReceiver_)
		// Note that the cursor's position is in item local coordinates.
		receiverCursorPosition_ = originalActionReceiver_->scene()->mainCursor()->position();
		
	actionText_->moveCursor();
}

void ActionPrompt::setPromptPosition()
{
	for ( auto v : originalActionReceiver_->scene()->views())
	{
		if (v->isActiveWindow())
		{
			setPos(v->mapToScene(210, v->viewport()->height() - 60)); // Put it after the mini map on the bottom
			break;
		}
	}
}

void ActionPrompt::computeCurrentActionReceiver()
{
	int level = parentActionsLevel_;

	if (level >= 0)
	{
		// Find a parent that has a node with actions
		auto item = originalActionReceiver_;
		QList<Model::Node*> nodesSeen;
		while (item)
		{
			// In some cases it happens that a single node is visualized multiple times in the hierarchy (e.g. RootItem
			// visualizes the same node as its child). We count only the first occurence.
			if (item->node() != nullptr && !nodesSeen.contains(item->node()))
			{
				nodesSeen.prepend(item->node());
				auto actionList = Action::actions(item->node());
				if (!actionList.isEmpty())
				{
					if (level == 0)
					{
						currentActionReceiver_ = item;
						return;
					}
					else --level;
				}
			}
			item = item->parent();
		}
	}
	else
	{
		// Get a linear order of children
		auto children = originalActionReceiver_->childItems();
		int i = 0;
		while (i < children.size())
		{
			// While exploring the children in a BFS manner, look out for reaching the desired child node
			auto child = children.at(i);
			if (child->node())
			{
				auto actionList = Action::actions(child->node());
				if (!actionList.isEmpty())
				{
					++level;
					if (level == 0)
					{
						currentActionReceiver_ = child;
						return;
					}
				}
			}

			children.append(child->childItems());
			++i;
		}
	}

	currentActionReceiver_ = nullptr;
}

QList<Action*> ActionPrompt::actions()
{
	if (currentActionReceiver_) return Action::actions(currentActionReceiver_->node());
	else return {};
}

void ActionPrompt::upParentActionsLevel()
{
	++parentActionsLevel_;
	computeCurrentActionReceiver();

	// Only allow this if we can actually get to an item that contains a node with actions or if we end up at 0
	if (parentActionsLevel_ > 0 && !currentActionReceiver_)
	{
		--parentActionsLevel_;
		computeCurrentActionReceiver();
	}
	else
	{
		setHighlight(true);
		setReceiverName();
		setUpdateNeeded(StandardUpdate);
	}
}

void ActionPrompt::downParentActionsLevel()
{
	--parentActionsLevel_;
	computeCurrentActionReceiver();

	// Only allow this if we can actually get to an item that contains a node with actions or if we end up at 0
	if (parentActionsLevel_ < 0 && !currentActionReceiver_)
	{
		++parentActionsLevel_;
		computeCurrentActionReceiver();
	}
	else
	{
		setHighlight(true);
		setReceiverName();
		setUpdateNeeded(StandardUpdate);
	}
}

}
