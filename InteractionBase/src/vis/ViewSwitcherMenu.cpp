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

namespace Interaction {

ITEM_COMMON_DEFINITIONS(ViewSwitcherMenu, "item")

void ViewSwitcherMenu::show(Visualization::Item* target)
{
	QApplication::postEvent(target->scene(),
							new Visualization::CustomSceneEvent( [=]() { showNow(target); }));
}

void ViewSwitcherMenu::showNow(Visualization::Item* target)
{
	QVector<Visualization::Item*> items;
	for (auto view : target->scene()->viewItems())
		items.append(new VViewSwitcherEntry(nullptr, view->name()));
	for (int i = items.size(); i < 9; i++)
		items.append(new VViewSwitcherEntry(nullptr, "Empty slot " + QString::number(i)));
	Menu::hideNow();
	Menu::instance = new ViewSwitcherMenu(items, target);
	target->scene()->addTopLevelItem(Menu::instance);
	target->scene()->addPostEventAction( [=]()
					{ Menu::instance->selectItem(Menu::instance->currentItems()[0][0]); });
}

ViewSwitcherMenu::ViewSwitcherMenu(QVector<Visualization::Item*> items,
											 Visualization::Item* target, StyleType* style)
	: Super(items, target, style)
{
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
		auto nameAfter = entry->nameField()->text();
		if (scene()->viewItem(nameAfter) == nullptr && nameAfter != nameBefore_)
			scene()->newViewItem(nameAfter);
	}
}

bool ViewSwitcherMenu::executeEntry(Visualization::Item* item)
{
	if (inEditMode_) return false;
	if (auto asSwitcher = DCast<VViewSwitcherEntry>(item))
	{
		auto view = scene()->viewItem(asSwitcher->nameField()->text());
		if (!view)
			view = scene()->newViewItem(asSwitcher->nameField()->text());
		scene()->switchToView(view);
	}
	return true;
}

}
