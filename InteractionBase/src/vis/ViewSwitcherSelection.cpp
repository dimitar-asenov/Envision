/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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
#include "ViewSwitcherSelection.h"

#include "nodes/ViewSwitcherEntry.h"
#include "vis/VViewSwitcherNode.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/cursor/TextCursor.h"

namespace Interaction {

ITEM_COMMON_DEFINITIONS(ViewSwitcherSelection, "item")

ViewSwitcherSelection* ViewSwitcherSelection::instance{};

void ViewSwitcherSelection::show(QVector<Model::Node*> selectableNodes, Visualization::Item* target)
{
	QApplication::postEvent(target->scene(),
							new Visualization::CustomSceneEvent( [=]() { showNow(selectableNodes, target); }));
}

void ViewSwitcherSelection::hide()
{
	if (instance)
		QApplication::postEvent(instance->scene(),
								new Visualization::CustomSceneEvent( [&]() { hideNow(); }));
}

void ViewSwitcherSelection::showNow(QVector<Model::Node*> selectableNodes, Visualization::Item* target)
{
	ViewSwitcherSelection::hideNow();
	instance = new ViewSwitcherSelection(selectableNodes, target);
	target->scene()->addTopLevelItem(instance);
	instance->installSceneEventFilter(instance);
	instance->setFiltersChildEvents(true);
}

void ViewSwitcherSelection::hideNow()
{
	if (instance)
	{
		instance->removeSceneEventFilter(instance);
		instance->scene()->removeTopLevelItem(instance);
	}
	SAFE_DELETE_ITEM(instance);
	instance = nullptr;
}

ViewSwitcherSelection::ViewSwitcherSelection(QVector<Model::Node*> selectableNodes,
											 Visualization::Item* target, StyleType* style)
	: Super(selectableNodes, target, style)
{
}

bool ViewSwitcherSelection::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
	if (inEditMode_ && event->type() == QEvent::KeyPress)
	{
		//If editing, let the mover use the cursor keys to move within the item itself,
		//rather than between items
		auto keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right)
			return false;
	}
	auto result = Super::sceneEventFilter(watched, event);
	if (result)
		return true;
	else if (event->type() == QEvent::KeyPress)
	{
		//F2 can trigger a rename of the view item,
		//also makes sure left click and enter then don't actually
		//execute the existing item to make editing easier
		auto keyEvent = static_cast<QKeyEvent*>(event);
		auto asSwitcher = DCast<VViewSwitcherNode>(focusedItem());
		if (asSwitcher && (keyEvent->key() == Qt::Key_F2
				|| (keyEvent->key() == Qt::Key_Return && inEditMode_)))
		{
			inEditMode_ = !inEditMode_;
			if (!inEditMode_)	selectItem(asSwitcher);
			else
			{
				asSwitcher->setGraphicsEffect(nullptr);
				asSwitcher->nameField()->moveCursor();
				asSwitcher->nameField()->correspondingSceneCursor<Visualization::TextCursor>()->selectAll();
			}
			return true;
		}
	}
	return false;
}

bool ViewSwitcherSelection::onSelectNode(Model::Node *node)
{
	if (inEditMode_) return false;
	if (auto asSwitcher = DCast<ViewSwitcherEntry>(node))
	{
		auto view = scene()->viewItem(asSwitcher->viewName());
		if (!view)
			view = scene()->newViewItem(asSwitcher->viewName());
		scene()->switchToView(view);
	}
	return true;
}

bool ViewSwitcherSelection::onSelectText(QString text)
{
	if (inEditMode_) return false;
	auto newView = scene()->newViewItem(text);
	scene()->switchToView(newView);
	return true;
}

void ViewSwitcherSelection::hideSelection()
{
	ViewSwitcherSelection::hide();
}

}
