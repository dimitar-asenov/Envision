/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * Scene.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Scene.h"
#include "VisualizationManager.h"
#include "items/Item.h"
#include "items/SceneHandlerItem.h"
#include "items/SelectedItem.h"
#include "cursor/Cursor.h"
#include "CustomSceneEvent.h"

#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/Model.h"

namespace Visualization {

class UpdateSceneEvent : public QEvent
{
	public:
		static const QEvent::Type EventType;
		UpdateSceneEvent() : QEvent(EventType){};
};

const QEvent::Type UpdateSceneEvent::EventType = static_cast<QEvent::Type> (QEvent::registerEventType());

Scene::Scene()
	: QGraphicsScene(VisualizationManager::instance().getMainWindow()), needsUpdate_(false),
	  renderer_(defaultRenderer()), sceneHandlerItem_(new SceneHandlerItem(this)), inEventHandler_(false)
{
}

Scene::~Scene()
{
	for (Item* i : topLevelItems_) SAFE_DELETE_ITEM(i);
	topLevelItems_.clear();
	for (SelectedItem* si : selections_) SAFE_DELETE_ITEM(si);
	selections_.clear();
	for (Cursor* c : cursors_) SAFE_DELETE(c);
	cursors_.clear();
	SAFE_DELETE_ITEM(sceneHandlerItem_);

	if (renderer_ != defaultRenderer()) SAFE_DELETE(renderer_);
	else renderer_ = nullptr;
}

ModelRenderer* Scene::defaultRenderer()
{
	static ModelRenderer defaultRenderer_;
	return &defaultRenderer_;
}

void Scene::addTopLevelItem(Item* item)
{
	topLevelItems_.append(item);
	addItem(item);
}

void Scene::removeTopLevelItem(Item* item)
{
	topLevelItems_.removeAll(item);
	removeItem(item);
}

void Scene::scheduleUpdate()
{
	if (!needsUpdate_)
	{
		needsUpdate_ = true;
		if (!inEventHandler_) QApplication::postEvent(this, new UpdateSceneEvent());
	}
}

void Scene::updateItems()
{
	// Update Top level items
	for (int i = 0; i<topLevelItems_.size(); ++i)
		topLevelItems_.at(i)->updateSubtree();

	// Update Selections
	// TODO do not recreate all items all the time.
	for (int i = 0; i<selections_.size(); i++) SAFE_DELETE_ITEM(selections_[i]);
	QList<QGraphicsItem *> selected = selectedItems();

	// Only display a selection when there are multiple selected items or no cursor
	bool draw_selections = selected.size() !=1 || cursors_.isEmpty() || cursors_.first() == nullptr
			|| cursors_.first()->visualization() == nullptr;

	if (!draw_selections)
	{
		auto selectable = cursors_.first()->owner();
		while (selectable && ! (selectable->flags() &  QGraphicsItem::ItemIsSelectable))
			selectable = selectable->parent();

		draw_selections = !selectable || selectable != selected.first();
	}

	if (draw_selections)
	{
		for (int i = 0; i<selected.size(); ++i)
		{
			Item* s = static_cast<Item*> (selected[i]);
			selections_.append(new SelectedItem(s));
			addItem(selections_.last());
			selections_.last()->updateSubtree();
		}
	}


	// Update Cursors
	for (Cursor* c : cursors_)
	{
		if (c->visualization())
		{
			if (c->visualization()->scene() != this) addItem(c->visualization());
			c->visualization()->updateSubtree();
		}
	}

	if (!cursors_.isEmpty())
	{
		for (auto view : views())
			if (view->isActiveWindow())
			{
				auto vis = cursors_.at(0)->visualization();
				if (!vis) vis = cursors_.at(0)->owner();
				view->ensureVisible( vis->boundingRect().translated(vis->scenePos()), 5, 5);
			}
	}

	needsUpdate_ = false;
}

void Scene::listenToModel(Model::Model* model)
{
	connect(model, SIGNAL(nodesModified(QList<Node*>)), this,  SLOT(nodesUpdated(QList<Node*>)), Qt::QueuedConnection);
}

void Scene::nodesUpdated(QList<Node*> nodes)
{
	// TODO implement this in a more efficient way.
	for (QGraphicsItem* graphics_item :  items())
	{
		Item* item = static_cast<Item*> ( graphics_item );
		if (item->hasNode() && nodes.contains(item->node())) item->setUpdateNeeded(Item::StandardUpdate);
	}

	scheduleUpdate();
}

void Scene::customEvent(QEvent *event)
{
	if ( event->type() == UpdateSceneEvent::EventType ) updateItems();
	else if (auto e = dynamic_cast<CustomSceneEvent*>(event))
	{
		e->execute();
	}
	else
		QGraphicsScene::customEvent(event);
}

bool Scene::event(QEvent *event)
{
	inEventHandler_ = true;

	bool result = false;
	if (event->type() != UpdateSceneEvent::EventType &&
		 event->type() != QEvent::MetaCall  &&
		 event->type() != QEvent::GraphicsSceneMouseMove &&
		 event->type() != QEvent::GraphicsSceneHoverMove
		)
		scheduleUpdate();

	if (event->type() == QEvent::KeyPress)
	{
		//Circumvent the standard TAB handling of the scene.
		keyPressEvent(static_cast<QKeyEvent *>(event));
		result = true;
	}
	else result = QGraphicsScene::event(event);

	inEventHandler_ = false;

	if (needsUpdate_) updateItems();
	for(auto e : postEventActions_)
	{
		customEvent(e);
		SAFE_DELETE(e);
	}
	postEventActions_.clear();

	return result;
}

void Scene::addPostEventAction(QEvent* action)
{
	if (inEventHandler_)
		postEventActions_.append(action);
	else throw VisualizationException("Can not add post event actions when not in event!");
}

void Scene::setMainCursor(Cursor* cursor)
{
	if (!cursors_.isEmpty())
	{
		Cursor* main = cursors_.first();
		SAFE_DELETE(main);
		cursors_.removeFirst();
	}

	if (cursor) cursors_.prepend(cursor);
}

}
