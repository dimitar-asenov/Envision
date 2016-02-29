/***********************************************************************************************************************
**
** Copyright (c) 2015 ETH Zurich
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

#include "ViewItemManager.h"

#include "items/ViewItem.h"
#include "Scene.h"

#include "ModelBase/src/model/TreeManager.h"
#include "ModelBase/src/model/AllTreeManagers.h"

namespace Visualization {

ViewItemManager::ViewItemManager(Scene* scene)
	: scene_{scene}
{
	viewItems_.resize(VIEW_ITEM_COLUMNS);
	currentViewItem_ = newViewItem("ProjectView");
	currentViewItem_->show();
}

ViewItemManager::~ViewItemManager()
{
	auto viewItems = viewItems_;
	viewItems_.clear();
	for (auto vector : viewItems)
		for (auto view : vector)
			if (view)
			{
				scene_->removeTopLevelItem(view);
				SAFE_DELETE_ITEM(view);
			}
	currentViewItem_ = nullptr;
	scene_ = nullptr;
}

ViewItem* ViewItemManager::currentViewItem()
{
	if (std::all_of(viewItems_.begin(), viewItems_.end(),
					[](QVector<ViewItem*> v) { return v.isEmpty(); }))
	{
		currentViewItem_ = newViewItem("ProjectView");
		currentViewItem_->show();
	}
	Q_ASSERT(currentViewItem_);
	return currentViewItem_;
}

ViewItem* ViewItemManager::viewItem(const QString name)
{
	for (auto vector : viewItems_)
		for (auto item : vector)
			if (item && item->name() == name)
				return item;
	return nullptr;
}

void ViewItemManager::addViewItem(ViewItem *view, QPoint position)
{
	Q_ASSERT(!scene_->inAnUpdate_);
	if (position.x() < 0 || position.y() < 0)
		position = nextEmptyPosition();
	Q_ASSERT(position.x() >= 0 && position.x() < VIEW_ITEM_COLUMNS
			 && position.y() >= 0);
	if (viewItems_[position.x()].size() <= position.y())
		viewItems_[position.x()].resize(position.y() + 1);
	//If there already is an item -> use insert
	if (viewItems_[position.x()][position.y()])
		viewItems_[position.x()].insert(position.y(), view);
	else
	{
		//Else just overwrite the nullptr
		viewItems_[position.x()][position.y()] = view;
	}
	scene_->addTopLevelItem(view, false);
}

void ViewItemManager::switchToView(ViewItem *view)
{
	Q_ASSERT(!scene_->inAnUpdate_);
	Q_ASSERT(std::any_of(viewItems_.begin(), viewItems_.end(),
						 [view](QVector<ViewItem*> v) { return v.contains(view); }));
	currentViewItem_->hide();
	currentViewItem_ = view;
	currentViewItem_->show();
	currentViewItem_->setUpdateNeeded(Item::StandardUpdate);
	scene_->scheduleUpdate();
}

bool ViewItemManager::switchToView(const QString viewName)
{
	auto view = viewItem(viewName);
	if (view) switchToView(view);
	return view != nullptr;
}
ViewItem* ViewItemManager::newViewItem(const QString name, QPoint position)
{
	//If the name is invalid, don't create a new view
	if (!ViewItem::isValidName(name)) return nullptr;
	auto result = new ViewItem{nullptr, name};
	addViewItem(result, position);
	return result;
}

void ViewItemManager::removeAllViewItems()
{
	for (auto vector : viewItems_)
		for (auto item : vector)
			scene_->removeTopLevelItem(item);

	viewItems_.clear();
	currentViewItem_ = nullptr;
}

void ViewItemManager::saveView(ViewItem* view, Model::TreeManager* manager) const
{
	auto json = view->toJson().toJson();
	if (!QDir{DIRECTORY_NAME}.exists())
		QDir().mkdir(DIRECTORY_NAME);
	QFile file{fileName(view->name(), manager->name())};
	file.open(QIODevice::WriteOnly);
	QTextStream write{&file};
	write << json;
}

ViewItem* ViewItemManager::loadView(QString name, QPoint position)
{
	for (auto manager : Model::AllTreeManagers::instance().loadedManagers())
		if (auto view = loadView(name, manager))
		{
			addViewItem(view, position);
			return view;
		}
	return nullptr;
}

ViewItem* ViewItemManager::loadView(QString name, Model::TreeManager* manager)
{
	QFile file{fileName(name, manager->name())};
	if (!file.exists()) return nullptr;
	file.open(QIODevice::ReadOnly);
	QTextStream read{&file};
	QString json;
	while (!read.atEnd()) json = json + read.readLine();
	ViewItem* view = new ViewItem{nullptr};
	view->fromJson(QJsonDocument::fromJson(json.toUtf8()));
	return view;
}

void ViewItemManager::cleanupRemovedItem(Item *removedItem)
{
	for (auto vector : viewItems_)
		for (auto item : vector)
			if (item) item->cleanupRemovedItem(removedItem);
}

void ViewItemManager::cleanupRemovedNode(Model::Node* removedNode)
{
	for (auto vector : viewItems_)
		for (auto item : vector)
			if (item) item->cleanupRemovedNode(removedNode);
}

QPoint ViewItemManager::nextEmptyPosition() const
{
	//Take the first empty position, if one exists
	for (int col = 0; col < viewItems_.size(); col++)
		for (int row = 0; row < viewItems_[col].size(); row++)
			if (!viewItems_[col][row]) return QPoint{col, row};
	//Else just use the column with the least rows
	int colToInsert = 0;
	for (int col = 1; col < viewItems_.size(); col++)
		if (viewItems_[col].size() < viewItems_[colToInsert].size())
			colToInsert = col;
	return QPoint{colToInsert, viewItems_[colToInsert].size()};
}

QString ViewItemManager::fileName(QString viewName, QString managerName) const
{
	return QDir::toNativeSeparators(DIRECTORY_NAME + "/" + managerName + "__" + viewName + ".json");
}

}
