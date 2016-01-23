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

#include "GenericHandler.h"

#include "HList.h"
#include "../autocomplete/AutoComplete.h"
#include "../commands/CommandExecutionEngine.h"
#include "../actions/Action.h"
#include "../actions/ActionPrompt.h"
#include "../prompt/Prompt.h"
#include "../prompt/PromptShell.h"
#include "../vis/ViewSwitcherMenu.h"
#include "../vis/VViewSwitcherEntry.h"

#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/ViewItemManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/renderer/ModelRenderer.h"
#include "VisualizationBase/src/cursor/Cursor.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/icons/Icon.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/overlays/BoxOverlay.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"
#include "FilePersistence/src/SystemClipboard.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/nodes/List.h"
#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/nodes/Text.h"

#include "events/KeyInputHandler.h"

namespace Interaction {

void GenericHandlerManagerListener::nodesUpdated(QSet<Node*>, QSet<Node*>)
{
	GenericHandler::fixCursorPositionForUndoAfterTreeManagerChange();
}

Model::TreeManager* GenericHandlerManagerListener::managerOf(Visualization::Item* item)
{
	auto node = item->node();
	while (!node && item->parent())
	{
		item = item->parent();
		node = item->node();
	}

	if (!node) return nullptr;
	else return node->manager();
}

void GenericHandlerManagerListener::listenToTreeManagerOf(Visualization::Item* item)
{
	auto manager = managerOf(item);
	if (!manager) return;

	if (!managers_.contains(manager))
	{
		managers_.append(manager);
		connect(manager, &Model::TreeManager::nodesModified, this,
				&GenericHandlerManagerListener::nodesUpdated, Qt::QueuedConnection);
	}
}

void GenericHandlerManagerListener::stopListeningToTreeManagerOf(Visualization::Item* item)
{
	auto manager = managerOf(item);
	if (!manager) return;

	if (managers_.contains(manager))
	{
		managers_.removeAll(manager);
		disconnect(manager, &Model::TreeManager::nodesModified, this,
					  &GenericHandlerManagerListener::nodesUpdated);
	}
}

GenericHandler::CommentCreationFunction GenericHandler::createComment_{};

CommandExecutionEngine* GenericHandler::executionEngine_ = CommandExecutionEngine::instance();
ActionPrompt* GenericHandler::actionPrompt_{};

QPoint GenericHandler::cursorOriginMidPoint_;
GenericHandler::CursorMoveOrientation GenericHandler::cursorMoveOrientation_ = NoOrientation;

QHash<int, QPoint> GenericHandler::keyToViewItemIndexMap_
		{{Qt::Key_1, QPoint(0, 0)}, {Qt::Key_2, QPoint(1, 0)}, {Qt::Key_3, QPoint(2, 0)},
		 {Qt::Key_4, QPoint(0, 1)}, {Qt::Key_5, QPoint(1, 1)}, {Qt::Key_6, QPoint(2, 1)},
		 {Qt::Key_7, QPoint(0, 2)}, {Qt::Key_8, QPoint(1, 2)}, {Qt::Key_9, QPoint(2, 2)}};

QList<QPair<Visualization::Item*, QPoint> > GenericHandler::cursorPositionsForUndo_{};
int GenericHandler::cursorUndoIndex_{-1};
QPair<Visualization::Item*, QPoint> GenericHandler::lastCursorPosition_;

GenericHandlerManagerListener& GenericHandler::managerListener()
{
	static GenericHandlerManagerListener m;
	return m;
}

GenericHandler::GenericHandler()
{
}

GenericHandler* GenericHandler::instance()
{
	static GenericHandler h;
	return &h;
}

CommandExecutionEngine* GenericHandler::executionEngine()
{
	return executionEngine_;
}

void GenericHandler::setCommandExecutionEngine(CommandExecutionEngine *engine)
{
	executionEngine_ = engine;
}

void GenericHandler::resetCursorOrigin()
{
	cursorOriginMidPoint_ = QPoint();
	cursorMoveOrientation_ = NoOrientation;
}

void GenericHandler::toggleComment(Visualization::Item *itemWithComment, Model::Node *aNode, bool hideOnly)
{
	auto scene = itemWithComment->scene();
	auto overlayGroup = scene->overlayGroup("PopupComments");

	if (overlayGroup && overlayGroup->removeOverlayOf(itemWithComment)) return;
	if (hideOnly) return;

	if (!overlayGroup) overlayGroup = scene->addOverlayGroup("PopupComments");

	overlayGroup->addOverlay(makeOverlay( new Visualization::BoxOverlay{itemWithComment,
		[aNode](Visualization::BoxOverlay* self){
		self->renderer()->sync(self->content(), self, aNode);
		return QString("comment");
	}}));
}

void GenericHandler::command(Visualization::Item *target, const QString& command,
		const std::unique_ptr<Visualization::Cursor>& cursor)
{
	executionEngine_->execute(target, command, cursor);
}

void GenericHandler::beforeEvent(Visualization::Item * target, QEvent* event)
{
	recordCursorPosition(target);
	managerListener().listenToTreeManagerOf(target);

	if (	event->type() == QEvent::GraphicsSceneMouseMove
			|| event->type() == QEvent::GraphicsSceneMousePress
			|| event->type() == QEvent::GraphicsSceneMouseDoubleClick)
	{
		resetCursorOrigin();
	}
}

void GenericHandler::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	KeyInputHandler::instance()->handleKeyInput(target, QKeySequence(event->modifiers()|event->key()));
	if (event->matches(QKeySequence::Copy) && !target->ignoresCopyAndPaste())
	{
		event->accept();

		QList<const Model::Node*> nodesToCopy;
		auto selected = target->scene()->selectedItems();

		// Get all items from the current selection that have a node
		for (int i = 0; i<selected.size(); ++i)
		{
			auto item = selected.at(i);
			if (item->hasNode()) nodesToCopy.append(item->node());
		}

		// In case there is exactly one selected item and it has no node, try to find the first parent that has a node
		if (nodesToCopy.size() == 0 && selected.size() == 1)
		{
			auto item = selected.at(0);
			while (item)
			{
				if (item->hasNode())
				{
					nodesToCopy.append(item->node());
					break;
				}

				item = item->parent();
			}
		}

		if (nodesToCopy.size() > 0)
		{

			FilePersistence::SystemClipboard clipboard;
			arrangeNodesForClipboard(nodesToCopy);
			clipboard.putNodes(nodesToCopy);
		}
	}
	else if (event->matches(QKeySequence::Paste) && !target->ignoresCopyAndPaste())
	{
		event->accept();

		FilePersistence::SystemClipboard clipboard;
		if (clipboard.numNodes() == 1 && target->scene()->selectedItems().size() == 1 && target->isSelected())
		{
			if (target->hasNode() && target->node()->typeName() == clipboard.currentNodeType())
			{
				target->node()->manager()->beginModification(target->node(), "paste");
				target->node()->load(clipboard);
				target->node()->manager()->endModification();
				target->setUpdateNeeded(Visualization::Item::StandardUpdate);
			}
			else InteractionHandler::keyPressEvent(target, event);
		}
		else InteractionHandler::keyPressEvent(target, event);
	}
	else if (event->matches(QKeySequence::Undo))
	{
		if (target->hasNode())
		{
			event->accept();
			auto scene = target->scene();
			managerListener().stopListeningToTreeManagerOf(target);

			Model::TreeManager* m = target->node()->manager();
			m->beginModification(nullptr, "undo");
			m->undo();
			m->endModification();
			target->setUpdateNeeded(Visualization::Item::StandardUpdate);

			// Reposition the cursor to the location it had before the tree was changed
			if (cursorUndoIndex_ + 1 == cursorPositionsForUndo_.size())
			{
				cursorPositionsForUndo_.append(lastCursorPosition_);
				++cursorUndoIndex_;
			}

			if (cursorUndoIndex_ > 0)
			{
				auto undoData = cursorPositionsForUndo_.at(--cursorUndoIndex_);
				if (scene->topLevelItems().contains(undoData.first))
				{
					scene->addPostEventAction( new SetCursorEvent{undoData.first, undoData.second});

					undoData.first->moveCursor(Visualization::Item::MoveOnPosition, undoData.second);
				}
			}
		}
		else InteractionHandler::keyPressEvent(target, event);
	}
	else if (event->matches(QKeySequence::Redo))
	{
		if (target->hasNode())
		{
			event->accept();
			auto scene = target->scene();
			managerListener().stopListeningToTreeManagerOf(target);

			Model::TreeManager* m = target->node()->manager();
			m->beginModification(nullptr, "redo");
			m->redo();
			m->endModification();
			target->setUpdateNeeded(Visualization::Item::StandardUpdate);

			// Reposition the cursor to the location it had after the tree was changed
			if (cursorUndoIndex_+1 < cursorPositionsForUndo_.size())
			{
				auto undoData = cursorPositionsForUndo_.at(++cursorUndoIndex_);
				if (scene->topLevelItems().contains(undoData.first))
				{
					scene->addPostEventAction( new SetCursorEvent{undoData.first, undoData.second});

					undoData.first->moveCursor(Visualization::Item::MoveOnPosition, undoData.second);
				}

				if (cursorUndoIndex_ + 1 == cursorPositionsForUndo_.size())
				{
					cursorPositionsForUndo_.removeLast();
					--cursorUndoIndex_;
				}
			}
		}
		else InteractionHandler::keyPressEvent(target, event);
	}
	else if (event->modifiers() == 0
			&& (	event->key() == Qt::Key_Up
					|| event->key() == Qt::Key_Down
					|| event->key() == Qt::Key_Left
					|| event->key() == Qt::Key_Right))
	{
		// Only initiate the movement procedure if the target is the cursor owner
		if (!target->scene()->mainCursor() || target != target->scene()->mainCursor()->owner())
			InteractionHandler::keyPressEvent(target, event);
		else
		{
			event->accept();
			bool oldNotLocationEquivalent;
			Visualization::Cursor::CursorType oldType;
			bool oldBoundary;
			Visualization::Item* oldOwner;
			Visualization::Item* t = target;
			bool moved = false;
			do
			{
				oldNotLocationEquivalent = t->scene()->mainCursor()->notLocationEquivalent();
				oldType = t->scene()->mainCursor()->type();
				oldBoundary = t->scene()->mainCursor()->isAtBoundary();
				oldOwner = t->scene()->mainCursor()->owner();
				moved = moveCursor(t, event->key()) || moved;
				t = t->scene()->mainCursor()->owner();
			} while (t->scene()->mainCursor()->isLocationEquivalent(oldNotLocationEquivalent, oldType, oldBoundary,
																					  oldOwner));

			// If the arrow key did not result in a movement, let the handlers of parent items try and process the key
			// press.
			if (!moved) InteractionHandler::keyPressEvent(target, event);
		}
	}
	else if (event->key() == Qt::Key_Escape && (AutoComplete::isVisible() || Prompt::isVisible()))
	{
		event->accept();

		if (Prompt::isVisible()) Prompt::hide();
		if (AutoComplete::isVisible()) AutoComplete::hide();
	}
	else if ( ((event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Escape) // Regular command prompt
				  || (event->modifiers() == Qt::ControlModifier
						&& Prompt::showShortcut(static_cast<Qt::Key>(event->key())))) // Custom prompt

			&& !(actionPrompt_ && actionPrompt_->isVisible()) && !Prompt::isVisible())
	{
		event->accept();

		// Only show the command prompt if this event was not received within it.
		if (event->key() == Qt::Key_Escape) Prompt::show(target);
		else if (event->modifiers() == Qt::ControlModifier)
		{
			auto show = Prompt::showShortcut(static_cast<Qt::Key>(event->key()));
			Q_ASSERT(show);
			show(target);
		}
		else Q_ASSERT(false);
	}
	else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Escape && target->node()
			&& !Prompt::isVisible())
	{
		event->accept();

		// Only show the action prompt if none of the other "menu items" are visible
		showActionPrompt(target, true);
	}
	else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_F1)
	{
		auto aNode = target;
		while (aNode)
		{
			auto aCompositeNode = DCast<Model::CompositeNode>(aNode->node());
			if (aCompositeNode)
			{
				if (aCompositeNode->comment() == nullptr)
				{
					if (createComment_)
					{
						aCompositeNode->beginModification("add comment");
						aCompositeNode->setComment(createComment_());
						aCompositeNode->endModification();
						aNode->updateSubtree();
					}
				}
				if (!aNode->findVisualizationOf(aCompositeNode->comment()))
					toggleComment(aNode, aCompositeNode->comment(), false);
				break;
			}
			aNode = aNode->parent();
		}
	}
	else if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_F2)
	{
		auto aNode = target;
		while (aNode)
		{
			auto aCompositeNode = DCast<Model::CompositeNode>(aNode->node());
			if (aCompositeNode)
			{
				if (aCompositeNode->comment() != nullptr)
				{
					aCompositeNode->beginModification("delete comment");
					aCompositeNode->setComment(nullptr);
					aCompositeNode->endModification();
					toggleComment(aNode, aCompositeNode->comment(), true);
				}
				break;
			}
			aNode = aNode->parent();
		}
	}
	else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_F1)
	{
		bool allSelected = true;
		auto scene = target->scene();

		QList<Visualization::Item*> stack = scene->topLevelItems();

		while (!stack.isEmpty())
		{
			auto i = stack.takeLast();
			if (auto aCompositeNode = DCast<Model::CompositeNode>(i->node()))
			{
				if (aCompositeNode->comment() != nullptr && !i->findVisualizationOf(aCompositeNode->comment()))
					allSelected = allSelected && i->isSelected();
			}
			stack.append(i->childItems());
		}

		scene->clearSelection();

		if (!allSelected)
		{
			stack = scene->topLevelItems();

			while (!stack.isEmpty())
			{
				auto i = stack.takeLast();
				if (auto aCompositeNode = DCast<Model::CompositeNode>(i->node()))
				{
					if (aCompositeNode->comment() != nullptr && !i->findVisualizationOf(aCompositeNode->comment()))
						i->setSelected(true);
				}
				stack.append(i->childItems());
			}
		}

		scene->scheduleUpdate();
	}
	else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_G)
	{
		event->accept();
		if (Menu::isVisible()) Menu::hide();
		else	ViewSwitcherMenu::show(target);
	}
	else if (event->modifiers() == Qt::ControlModifier && keyToViewItemIndexMap_.contains(event->key()))
	{
		if (Menu::isVisible()) Menu::hide();

		Visualization::ViewItem* view{};
		auto col = keyToViewItemIndexMap_[event->key()].x();
		auto row = keyToViewItemIndexMap_[event->key()].y();
		if (target->scene()->viewItems()->viewItems().size() > col
			&& target->scene()->viewItems()->viewItems()[col].size() > row)
			view = target->scene()->viewItems()->viewItems()[col][row];
		if (!view)
			view = target->scene()->viewItems()->
				newViewItem("View" + QString::number(col) + QString::number(row), QPoint(col, row));

		if (view) target->scene()->viewItems()->switchToView(view);
	}
	else InteractionHandler::keyPressEvent(target, event);
}

bool GenericHandler::moveCursor(Visualization::Item *target, int key)
{
	bool processed = false;
	Visualization::Item::CursorMoveDirection dir{};

	// Set the source navigation point when beginning to navigate in a new direction
	if (	(key == Qt::Key_Up || key == Qt::Key_Down)
			&& cursorMoveOrientation_ != VerticalOrientation)
	{
		cursorMoveOrientation_ = VerticalOrientation;
		Visualization::Cursor* c = target->scene()->mainCursor();
		if (c) cursorOriginMidPoint_ = c->sceneRegion().center();
	}
	if (	(key == Qt::Key_Left || key == Qt::Key_Right)
			&& cursorMoveOrientation_ != HorizontalOrientation)
	{
		cursorMoveOrientation_ = HorizontalOrientation;
		Visualization::Cursor* c = target->scene()->mainCursor();
		if (c) cursorOriginMidPoint_ = c->sceneRegion().center();
	}

	QPoint midpoint = target->mapFromScene(cursorOriginMidPoint_).toPoint();
	switch (key)
	{
		case Qt::Key_Up:
		{
			processed = target->moveCursor(Visualization::Item::MoveUp, midpoint);
			if (!processed) dir = Visualization::Item::MoveUpOf;
		}
		break;
		case Qt::Key_Down:
		{
			processed = target->moveCursor(Visualization::Item::MoveDown, midpoint);
			if (!processed) dir = Visualization::Item::MoveDownOf;
		}
		break;
		case Qt::Key_Left:
		{
			processed = target->moveCursor(Visualization::Item::MoveLeft, midpoint);
			if (!processed) dir = Visualization::Item::MoveLeftOf;
		}
		break;
		case Qt::Key_Right:
		{
			processed = target->moveCursor(Visualization::Item::MoveRight, midpoint);
			if (!processed) dir = Visualization::Item::MoveRightOf;
		}
		break;
		default:
		{
			resetCursorOrigin();
		}
		break;
	}

	if (!processed)
	{
		Visualization::Item* current = target;

		while (current && !processed)
		{
			Visualization::Item* parent = current->parent();
			if (!parent) break;

			QPoint reference;
			switch ( key )
			{
				case Qt::Key_Up:
				{
					int border = current->scenePos().y();
					reference = QPoint(cursorOriginMidPoint_.x(), border);
				} break;
				case Qt::Key_Down:
				{
					int border = current->scenePos().y() + current->heightInScene()-1;
					reference = QPoint(cursorOriginMidPoint_.x(), border);
				} break;
				case Qt::Key_Left:
				{
					int border = current->scenePos().x();
					reference = QPoint(border, cursorOriginMidPoint_.y());
				} break;
				case Qt::Key_Right:
				{
					int border = current->scenePos().x() + current->widthInScene()-1;
					reference = QPoint(border, cursorOriginMidPoint_.y());
				} break;
			}

			reference = parent->mapFromScene(reference).toPoint();

			processed = parent->moveCursor(dir, reference);
			current = parent;
		}
	}

	return processed;
}

void GenericHandler::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (auto staticParent = DCast<Visualization::Static>(target->parent()))
	{
		if (auto clickHandler = staticParent->style()->clickHandler())
			if (clickHandler(staticParent)) return;
	}

	if (event->modifiers() == Qt::NoModifier)
		target->moveCursor(Visualization::Item::MoveOnPosition, event->pos().toPoint());
	else if (event->button() == Qt::RightButton) {} // Accept the event
	else event->ignore();
}


void GenericHandler::mouseReleaseEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (isClick(target) && event->button() == Qt::RightButton) Prompt::show(target);
}

void GenericHandler::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	if (event->modifiers() == 0 && !event->buttonDownPos( Qt::LeftButton).isNull() )
	{
		QPainterPath path;
		path.addRect( QRectF(event->buttonDownScenePos(Qt::LeftButton), event->scenePos()) );
		target->scene()->setSelectionArea(path, Qt::IntersectsItemShape);
		filterSelectedItems(target, event);
		target->scene()->scheduleUpdate();
	}
}

void GenericHandler::mouseDoubleClickEvent(Visualization::Item *, QGraphicsSceneMouseEvent *)
{
	// Do no use the default handlers.
}

void GenericHandler::wheelEvent(Visualization::Item* target, QGraphicsSceneWheelEvent *event)
{
	if (event->modifiers() == Qt::AltModifier) // modify the semantic zoom level
	{
		// individual semantic zoom level chang
		Model::Node* node = nullptr;
		Visualization::Item* parent = nullptr;

		auto selectedItem = target->scene()->mainCursor() ? target->scene()->mainCursor()->owner() : nullptr;
		if (selectedItem)
		{
			// get the nearest parent item that has a node which is not of type Model::Text
			while (selectedItem
					 && (!selectedItem->node() || selectedItem->node()->isSubtypeOf(Model::Text::typeIdStatic())))
				selectedItem = selectedItem->parent();
			if (!selectedItem) return;

			parent = selectedItem->parent();
			if ( parent )
			{
				node = selectedItem->node();

				int newSemanticZoomLevel = 0;
				if (parent->definesChildNodeSemanticZoomLevel(node))
				{
					newSemanticZoomLevel = event->delta() < 0 ?
						target->scene()->renderer()->getCoarserSemanticZoomLevel( parent->childNodeSemanticZoomLevel(node)):
						target->scene()->renderer()->getFinerSemanticZoomLevel( parent->childNodeSemanticZoomLevel(node));
				}

				if (newSemanticZoomLevel >= 0) parent->setChildNodeSemanticZoomLevel(node, newSemanticZoomLevel);
				else parent->clearChildNodeSemanticZoomLevel(node);
			}
		}

		if (node) qApp->postEvent(target->scene(), new Interaction::SetCursorEvent{parent, node});
	} else InteractionHandler::wheelEvent(target, event);
}

void GenericHandler::focusInEvent(Visualization::Item *target, QFocusEvent *event)
{
	auto i = target;
	while (i)
	{
		if (i->flags() & QGraphicsItem::ItemIsSelectable)
		{
			target->scene()->clearSelection();
			i->setSelected(true);
			break;
		}
		else i = i->parent();
	}

	InteractionHandler::focusInEvent(target, event);
}

void GenericHandler::filterSelectedItems(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	auto selection = target->scene()->selectedItems();

	// Filter out items for which the selection is completely internal.
	for (int i = selection.size()-1; i>=0; --i)
	{
		if (  selection.at(i)->contains(selection.at(i)->mapFromScene(event->buttonDownScenePos(Qt::LeftButton)))
			&& selection.at(i)->contains(selection.at(i)->mapFromScene(event->scenePos())))
		{
			selection.at(i)->setSelected(false);
			selection.removeAt(i);
		}
	}

	// Filter out items whose parent is also included in the selection.
	for (int i = selection.size() - 1; i>=0; --i)
		for (int k = selection.size() - 1; k>=0; --k)
			if (k!=i && selection.at(k)->isAncestorOf(selection.at(i)))
			{
				selection.at(i)->setSelected(false);
				selection.removeAt(i);
				break;
			}

	// Check if there are any selected items with nodes remaining.
	bool itemWithNodeSelected = false;
	selection = target->scene()->selectedItems();
	for (int i = 0; i<selection.size(); ++i)
		if ( selection.at(i)->hasNode() )
		{
			itemWithNodeSelected = true;
			break;
		}

	// If there is at least one item with a node, discard all items which do not have a node.
	if (itemWithNodeSelected)
		for (int i = selection.size() - 1; i>=0; --i)
			if ( !selection.at(i)->hasNode() )
			{
				selection.at(i)->setSelected(false);
				selection.removeAt(i);
			}
}

void GenericHandler::arrangeNodesForClipboard(QList<const Model::Node*>& list)
{
	if (list.size() > 0)
	{
		// Determine if all nodes are elements of a list
		const Model::List* parent = DCast<const Model::List> (list.first()->parent());
		if (parent)
		{
			for (int i = 1; i<list.size(); ++i)
			{
				if (list[i]->parent() != parent)
				{
					parent = nullptr;
					break;
				}
			}
		}

		if (parent)
		{
			// The selection consists only of nodes which are elements of the same list. Arrange them properly
			// according to the list's order.
			// Bubble sort
			for (int i = list.size() - 1; i > 0; --i)
				for (int k = 0; k < i; ++k)
					if (parent->indexOf(list[k]) > parent->indexOf(list[k+1])) list.swap(k, k+1);
		}
	}
}

bool GenericHandler::removeFromList(Visualization::Item* target)
{
	Visualization::VList* list = nullptr;
	auto p = target->parent();
	while (p)
	{
		list = dynamic_cast<Visualization::VList* >(p);
		if (list) break;
		else p = p->parent();
	}

	auto nodeIndex = list->node()->indexOf(target->node());
	if (list && nodeIndex >= 0)
	{
		Interaction::HList::instance()->removeNodeAndSetCursor(list, nodeIndex);
		return true;
	}
	return false;
}

ActionPrompt* GenericHandler::actionPrompt()
{
	return actionPrompt_;
}

void GenericHandler::removeActionPrompt()
{
	SAFE_DELETE_ITEM(actionPrompt_);
}

void GenericHandler::showActionPrompt(Visualization::Item *actionRecevier, bool autoExecuteAction)
{
	if (actionPrompt_ && actionPrompt_->originalActionReceiver() == actionRecevier)
	{
		actionPrompt_->showPrompt();
	}
	else
	{
		removeActionPrompt();
		actionPrompt_ = new ActionPrompt{actionRecevier, autoExecuteAction};
	}
}

void GenericHandler::action(Visualization::Item *target, const QString& action)
{
	for (auto a : Action::actions(target->node()))
	{
		if (a->shortcut() == action)
		{
			a->execute(target);
			break;
		}
	}
}

void GenericHandler::recordCursorPosition(Visualization::Item* target)
{
	auto c = target->scene()->mainCursor();
	if (c)
	{
		auto top_level = c->owner();
		Q_ASSERT(top_level);
		while (top_level->parent()) top_level = top_level->parent();

		lastCursorPosition_.first = top_level;
		lastCursorPosition_.second = top_level->mapFromItem(c->owner(), c->region().center()).toPoint();
	}
}

void GenericHandler::fixCursorPositionForUndoAfterTreeManagerChange()
{
	// If making changes after an Undo, discard the remaining hierarchy
	while (cursorUndoIndex_ + 1 < cursorPositionsForUndo_.size())
		cursorPositionsForUndo_.removeLast();

	cursorPositionsForUndo_.append(lastCursorPosition_);
	++cursorUndoIndex_;
}

}
