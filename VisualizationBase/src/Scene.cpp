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

#include "ModelBase/headers/nodes/Node.h"
#include "ModelBase/headers/Model.h"

#include <QtGui/QApplication>
#include <QtCore/QEvent>

namespace Visualization {

ModelRenderer Scene::defaultRenderer_;

class UpdateSceneEvent : public QEvent
{
	public:
		static const QEvent::Type EventType = (QEvent::Type) (User + 1);
		UpdateSceneEvent() : QEvent(EventType){};
};

Scene::Scene() : QGraphicsScene(VisualizationManager::instance().getMainWindow()), needsUpdate_(false), renderer_(&defaultRenderer_), sceneHandlerItem_(new SceneHandlerItem(this))
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

	if (renderer_ != &defaultRenderer_) SAFE_DELETE(renderer_);
	else renderer_ = nullptr;
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
		QApplication::postEvent(this, new UpdateSceneEvent());
	}
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
		if (item->hasNode() && nodes.contains(item->node())) item->setUpdateNeeded();
	}

	scheduleUpdate();
}

void Scene::customEvent(QEvent *event)
{
	if ( event->type() == UpdateSceneEvent::EventType )
	{
		// Update Top level items
		for (int i = 0; i<topLevelItems_.size(); ++i) topLevelItems_.at(i)->updateSubtree();

		// Update Selections
		// TODO do not recreate all items all the time.
		for (int i = 0; i<selections_.size(); i++) SAFE_DELETE_ITEM(selections_[i]);
		QList<QGraphicsItem *> selected = selectedItems();
		for (int i = 0; i<selected.size(); ++i)
		{
			Item* s = static_cast<Item*> (selected[i]);
			selections_.append(new SelectedItem(s));
			addItem(selections_.last());
			selections_.last()->updateSubtree();
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

		needsUpdate_ = false;
	}
	else
		QGraphicsScene::customEvent(event);
}

bool Scene::event(QEvent *event)
{
	if (event->type() != UpdateSceneEvent::EventType &&
		 event->type() != QEvent::MetaCall  &&
		 event->type() != QEvent::GraphicsSceneMouseMove &&
		 event->type() !=QEvent::GraphicsSceneHoverMove
		)
		scheduleUpdate();

	return QGraphicsScene::event(event);
}

void Scene::setMainCursor(Cursor* cursor)
{
	if (!cursors_.isEmpty())
	{
		Cursor* main = cursors_.first();
		SAFE_DELETE(main);
		cursors_.removeFirst();
	}

	cursors_.prepend(cursor);
}


}
