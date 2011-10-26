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

Scene::Scene() : QGraphicsScene(VisualizationManager::instance().getMainWindow()), needsUpdate(false), renderer_(NULL), sceneHandlerItem_(new SceneHandlerItem(this))
{
	// TODO Auto-generated constructor stub
}

Scene::~Scene()
{
	for (int i = 0; i<topLevelItems.size(); ++i) SAFE_DELETE_ITEM(topLevelItems[i]);
	topLevelItems.clear();
	for (int i = 0; i<selections.size(); ++i) SAFE_DELETE_ITEM(selections[i]);
	selections.clear();
	SAFE_DELETE_ITEM(sceneHandlerItem_);
	SAFE_DELETE(renderer_);
}

void Scene::addTopLevelItem(Item* item)
{
	topLevelItems.append(item);
	addItem(item);
}

void Scene::removeTopLevelItem(Item* item)
{
	topLevelItems.removeAll(item);
	removeItem(item);
}

void Scene::scheduleUpdate()
{
	if (!needsUpdate)
	{
		needsUpdate = true;
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

	QList<QGraphicsItem*> list = items();
	for (int i = 0; i < list.size(); ++i)
	{
		Item* item = static_cast<Item*> (list[i]);
		if (item->hasNode() && nodes.contains(item->node())) item->setUpdateNeeded();
	}

	scheduleUpdate();
}

void Scene::customEvent(QEvent *event)
{
	if ( event->type() == UpdateSceneEvent::EventType )
	{
		// Update Top level items
		for (int i = 0; i<topLevelItems.size(); ++i) topLevelItems.at(i)->updateSubtree();

		// Update Selections
		// TODO do not recreate all items all the time.
		for (int i = 0; i<selections.size(); i++) SAFE_DELETE_ITEM(selections[i]);
		QList<QGraphicsItem *> selected = selectedItems();
		for (int i = 0; i<selected.size(); ++i)
		{
			Item* s = static_cast<Item*> (selected[i]);
			selections.append(new SelectedItem(s));
			addItem(selections.last());
			selections.last()->updateSubtree();
		}
		needsUpdate = false;
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


}
