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
#include "ViewSwitcherMenu.h"

#include "vis/VViewSwitcherEntry.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "VisualizationBase/src/cursor/TextCursor.h"
#include "VisualizationBase/src/ViewItemManager.h"

namespace Interaction {

ITEM_COMMON_DEFINITIONS(ViewSwitcherMenu, "item")

QHash<int, QPoint> ViewSwitcherMenu::keyToIndexMap_
		{{Qt::Key_Q, QPoint(0, 0)}, {Qt::Key_W, QPoint(1, 0)}, {Qt::Key_E, QPoint(2, 0)},
		 {Qt::Key_A, QPoint(0, 1)}, {Qt::Key_S, QPoint(1, 1)}, {Qt::Key_D, QPoint(2, 1)},
		 {Qt::Key_Z, QPoint(0, 2)}, {Qt::Key_X, QPoint(1, 2)}, {Qt::Key_C, QPoint(2, 2)}};

void ViewSwitcherMenu::show(Visualization::Item* target)
{
	QApplication::postEvent(target->scene(),
							new Visualization::CustomSceneEvent( [=]() { showNow(target); }));
}

void ViewSwitcherMenu::showNow(Visualization::Item* target)
{
	auto viewItems = target->scene()->viewItems()->viewItems();
	QVector<QVector<Visualization::Item*>> items;
	items.resize(viewItems.size());
	for (int col = 0; col < viewItems.size(); col++)
		for (int row = 0; row < viewItems[col].size(); row++)
			if (viewItems[col][row])
				items[col].append(new VViewSwitcherEntry(nullptr, viewItems[col][row]->name()));
			else
				items[col].append(new VViewSwitcherEntry(nullptr, "Empty slot"));
	for (int col = 0; col < items.size(); col++)
		for (int row = items[col].size(); row < 3; row++)
			items[col].append(new VViewSwitcherEntry(nullptr, "Empty slot"));
	Menu::hideNow();
	Menu::instance = new ViewSwitcherMenu(items, target);
	target->scene()->addTopLevelItem(Menu::instance);
	target->scene()->addPostEventAction( [=]()
					{ Menu::instance->selectItem(Menu::instance->currentItems()[0][0]); });
}

ViewSwitcherMenu::ViewSwitcherMenu(QVector<QVector<Visualization::Item*>> items,
											 Visualization::Item* target, StyleType* style)
	: Super(items, target, style)
{
}

bool ViewSwitcherMenu::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
	if (Super::sceneEventFilter(watched, event)) return true;
	else if (event->type() == QEvent::KeyPress)
	{
		auto keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->modifiers() == Qt::ControlModifier && keyToIndexMap_.contains(keyEvent->key()))
			if (executeEntry(currentItems()[keyToIndexMap_[keyEvent->key()].x()]
											  [keyToIndexMap_[keyEvent->key()].y()]))
			{
				hide();
				return true;
			}
	}
	return false;
}

void ViewSwitcherMenu::startFocusMode(Visualization::Item *target)
{
	if (auto asSwitcher = DCast<VViewSwitcherEntry>(target))
	{
		nameBefore_ = asSwitcher->nameField()->text();
		asSwitcher->setGraphicsEffect(nullptr);
		asSwitcher->nameField()->moveCursor();
		asSwitcher->nameField()->correspondingSceneCursor<Visualization::TextCursor>()->selectAll();
	}
}

void ViewSwitcherMenu::endFocusMode(Visualization::Item *target)
{
	if (auto entry = DCast<VViewSwitcherEntry>(target))
	{
		//If the view item didn't exist and the user changed the name,
		//create a new item with that name
		QPoint pos = indexOf(entry);
		auto nameAfter = entry->nameField()->text();
		if (scene()->viewItems()->viewItem(nameAfter) == nullptr && nameAfter != nameBefore_
				&& Visualization::ViewItem::isValidName(nameAfter))
		{
			if (!scene()->viewItems()->loadView(nameAfter, pos))
				scene()->viewItems()->newViewItem(nameAfter, pos);
		}
	}
}

bool ViewSwitcherMenu::executeEntry(Visualization::Item* item)
{
	if (inEditMode_) return false;
	if (auto entry = DCast<VViewSwitcherEntry>(item))
	{
		QPoint pos = indexOf(item);
		auto view = scene()->viewItems()->viewItem(entry->nameField()->text());
		if (!view)
		{
			auto name = entry->nameField()->text();
			if (!Visualization::ViewItem::isValidName(name))
				name = "View" + QString::number(pos.x()) + QString::number(pos.y()) + "";
			view = scene()->viewItems()->newViewItem(name, pos);
			if (!view)
				view = scene()->viewItems()->viewItem(name);
		}
		scene()->viewItems()->switchToView(view);
	}
	return true;
}

}
