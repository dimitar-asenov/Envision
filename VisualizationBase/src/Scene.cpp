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

#include "Scene.h"
#include "VisualizationManager.h"
#include "items/Item.h"
#include "items/SceneHandlerItem.h"
#include "items/SelectedItem.h"
#include "items/NameOverlay.h"
#include "items/RootItem.h"
#include "renderer/ModelRenderer.h"
#include "cursor/Cursor.h"
#include "CustomSceneEvent.h"

#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/model/Model.h"

#include "Logger/src/Timer.h"
#include "Core/src/Profiler.h"

namespace Visualization {

class UpdateSceneEvent : public QEvent
{
	public:
		static const QEvent::Type EventType;
		UpdateSceneEvent() : QEvent(EventType){};
};

const QEvent::Type UpdateSceneEvent::EventType = static_cast<QEvent::Type> (QEvent::registerEventType());

Scene::Scene()
	: QGraphicsScene(VisualizationManager::instance().getMainWindow()),
	  	  renderer_(defaultRenderer()), sceneHandlerItem_(new SceneHandlerItem(this)),
	  	  nameOverlay_{new NameOverlay(this)}, hiddenItemCategories_(NoItemCategory)
{
	setItemIndexMethod(NoIndex);

	initialized_ = true;
}

Scene::~Scene()
{
	SAFE_DELETE(mainCursor_);
	SAFE_DELETE_ITEM(sceneHandlerItem_);

	while(!topLevelItems_.isEmpty())
		SAFE_DELETE_ITEM(topLevelItems_.takeLast());
	while(!selections_.isEmpty())
		SAFE_DELETE_ITEM(selections_.takeLast());

	SAFE_DELETE_ITEM(nameOverlay_);

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
	Q_ASSERT(!inAnUpdate_);
	topLevelItems_.append(item);
	addItem(item);
	scheduleUpdate();
}

void Scene::removeTopLevelItem(Item* item)
{
	auto removed = topLevelItems_.removeAll(item);
	Q_ASSERT(!inAnUpdate_ || removed == 0);

	removeItem(item);
	scheduleUpdate();
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
	inAnUpdate_ = true;
	auto updateTimer = Logger::Timer::start("Scene update");

	// Profiler code
	static bool alreadyProfiled = false;
	if (!alreadyProfiled)
		for (auto item : topLevelItems_)
			if(item->typeName() == "RootItem")
			{
				alreadyProfiled = true;
				Core::Profiler::startOnce(true, "Initial item update", "updateItems.prof");
				break;
			}

	// Update Top level items
	for (auto item : topLevelItems_)
		item->updateSubtree();

	// Update items on zoom
	if (mainViewScalingFactorChanged_)
	{
		mainViewScalingFactorChanged_ = false;
		for (auto item : itemsToUpdateGeometryWhenZoomChanges_)
				item->changeGeometry();
	}

	Core::Profiler::stop("Initial item update");

	// Update Selections
	auto selected = selectedItems();

	// Only display a selection when there are multiple selected items or no cursor
	bool draw_selections = selected.size() !=1 || mainCursor_ == nullptr || mainCursor_->visualization() == nullptr;

	if (!draw_selections)
	{
		// There is exactly one item and it has a cursor visualization
		auto selectable = mainCursor_->owner();
		while (selectable && ! (selectable->flags() &  QGraphicsItem::ItemIsSelectable))
			selectable = selectable->parent();

		draw_selections = !selectable || selectable != selected.first();
	}

	int numSelectionItems = 0;
	auto selectionsIt = selections_.begin();
	auto selectionsInitialSize = selections_.size();
	if (draw_selections && !(selected.size() == 1 && selected.first() == sceneHandlerItem_))
	{
		for (auto selectedItem : selected)
		{
			if (numSelectionItems < selectionsInitialSize)
			{
				auto selectionMarker = *selectionsIt;
				selectionMarker->setSelectedItem(selectedItem);
				selectionMarker->updateSubtree();
				++selectionsIt;
			}
			else
			{
				auto selectionMarker = new SelectedItem(selectedItem);
				selectionMarker->updateSubtree();
				selections_.append(selectionMarker);
				addItem(selectionMarker);
			}

			++numSelectionItems;
		}
	}

	while (numSelectionItems < selectionsInitialSize)
	{
		SAFE_DELETE_ITEM(selections_.takeLast());
		--selectionsInitialSize;
	}

	// Update the main cursor
	if (mainCursor_ && mainCursor_->visualization())
	{
		if (mainCursor_->visualization()->scene() != this) addItem(mainCursor_->visualization());
		mainCursor_->visualization()->updateSubtree();
	}

	computeSceneRect();

	nameOverlay_->updateSubtree();

	updateTimer->tick();
	needsUpdate_ = false;
	inAnUpdate_ = false;
}

void Scene::listenToModel(Model::Model* model)
{
	connect(model, SIGNAL(nodesModified(QSet<Node*>)), this,  SLOT(nodesUpdated(QSet<Node*>)), Qt::QueuedConnection);
}

void Scene::nodesUpdated(QSet<Node*> nodes)
{
	for(auto node : nodes)
	{
		auto it = Item::nodeItemsMap().find(node);
		auto end = Item::nodeItemsMap().end();
		while (it != end && it.key() == node)
		{
			auto item = it.value();
			if (item->scene() == this) item->setUpdateNeeded(Item::StandardUpdate);
			++it;
		}
	}

	scheduleUpdate();
}

void Scene::customEvent(QEvent *event)
{
	if ( event->type() == UpdateSceneEvent::EventType ) updateItems();
	else if (auto e = dynamic_cast<CustomSceneEvent*>(event))
	{
		auto t = Logger::Timer::start("Custom event");
		e->execute();
		t->tick();
	}
	else
		QGraphicsScene::customEvent(event);
}

bool Scene::event(QEvent *event)
{
	bool result = false;

	if (inAnUpdate_ || !initialized_)
		result = QGraphicsScene::event(event);
	else
	{
		inEventHandler_ = true;

		if (event->type() != UpdateSceneEvent::EventType &&
			 event->type() != QEvent::MetaCall  &&
			 event->type() != QEvent::GraphicsSceneMouseMove &&
			 event->type() != QEvent::GraphicsSceneHoverMove
			)
			scheduleUpdate();

		Logger::Timer* tInputEvent{};
		if (event->type() == QEvent::GraphicsSceneMousePress || event->type() == QEvent::KeyPress)
			tInputEvent = Logger::Timer::start("Input event");

		// Always move the scene handler item to the location of the last mouse click.
		// This assures that even if the user presses somewhere in the empty space of the scene, the scene handler
		// item will be selected.
		if (event->type() == QEvent::GraphicsSceneMousePress)
		{
			auto e = static_cast<QGraphicsSceneMouseEvent*>(event);
			sceneHandlerItem_->setPos(e->scenePos() - QPointF(1,1));
		}

		if (event->type() == QEvent::KeyPress)
		{
			//Circumvent the standard TAB handling of the scene.
			keyPressEvent(static_cast<QKeyEvent *>(event));
			result = true;
		}
		else result = QGraphicsScene::event(event);

		if (event->type() == QEvent::GraphicsSceneMousePress || event->type() == QEvent::KeyPress)
			tInputEvent->tick();

		inEventHandler_ = false;

		if (needsUpdate_) updateItems();
		for(auto e : postEventActions_)
		{
			customEvent(e);
			SAFE_DELETE(e);
		}
		postEventActions_.clear();

		// On keyboard events, make sure the cursor is visible
		if (mainCursorsJustSet_ && mainCursor_ && mainCursor_->visualization()
				&& mainCursor_->visualization()->boundingRect().isValid() )
		{
			for (auto view : views())
				if (view->isActiveWindow())
				{
					mainCursorsJustSet_ = false;
					auto vis = mainCursor_->visualization();
					view->ensureVisible( vis->sceneBoundingRect(), 5, 5);
				}
		}
	}

	return result;
}

void Scene::addPostEventAction(QEvent* action)
{
	if (inEventHandler_)
		postEventActions_.append(action);
	else throw VisualizationException("Can not add post event actions when not in event!");
}

void Scene::keyPressEvent (QKeyEvent *event)
{
	if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_F5)
	{
		event->accept();

		for (auto f : refreshActionFunctions_) f(this);

		for (auto top : topLevelItems_)
		{
			auto parent = top;
			while (parent->parent()) parent = parent->parent();

			auto rootItem = dynamic_cast<Visualization::RootItem*>(parent);
			if (rootItem) rootItem->setUpdateNeeded(Visualization::Item::FullUpdate);
		}
	}
	else QGraphicsScene::keyPressEvent(event);
}

void Scene::setMainCursor(Cursor* cursor)
{
	SAFE_DELETE(mainCursor_);
	mainCursor_ = cursor;
	mainCursorsJustSet_ = true;
}

Item* Scene::focusItem() const
{
	return Item::envisionItem(QGraphicsScene::focusItem());
}

QList<Item*> Scene::selectedItems() const
{
	QList<Item*> list;
	for (auto item :  QGraphicsScene::selectedItems())
	{
		auto envisionItem = Item::envisionItem(item);
		if (envisionItem && !list.contains(envisionItem))
			list.append(envisionItem);
	}
	return list;
}

void Scene::computeSceneRect()
{
	QRectF sceneRect;
	QRectF viewRect;
	for (auto i: topLevelItems_)
	{
		QRectF br = i->sceneBoundingRect();
		sceneRect = sceneRect.united(br);
		if (i->itemCategory() != MenuItemCategory && i->itemCategory() != CursorItemCategory)
			viewRect = viewRect.united(br);
	}

	if (viewRect.isNull()) viewRect = QRectF(sceneRect.x(),sceneRect.y(),1,1);

	// TODO: Currently the user is not able to scroll in a way that will make menu items visible, if they are outside
	// the outter bounding (scene) rectangle of normal items. The code below was meant to fix this, but it caused
	// other issues, with too many scroll bars appearing when they are not necessary. In the future this should be fixed.

	//So far viewRect is strictly inside sceneRect.
	//Counterbalance the visible rect so that the main content is not displaced
	//At the end any extra space in the scene that is filled with menu items will be counterbalanced with equal amounts
	//at the opposite end.
//	int leftExtra = viewRect.left() - sceneRect.left();
//	int rightExtra = sceneRect.right() - viewRect.right();
//	int topExtra = viewRect.top() - sceneRect.top();
//	int bottomExtra = sceneRect.bottom() - viewRect.bottom();
//
//	if (leftExtra > rightExtra)
//	{
//		viewRect.setWidth(viewRect.width() + leftExtra - rightExtra);
//		viewRect.setX(viewRect.x() - leftExtra +  rightExtra);
//	}
//	if (rightExtra > leftExtra)
//	{
//		viewRect.setWidth(viewRect.width() + rightExtra - leftExtra);
//		viewRect.setX(viewRect.x() - rightExtra + leftExtra);
//	}
//	if (topExtra > bottomExtra)
//	{
//		viewRect.setHeight(viewRect.height() + topExtra - bottomExtra);
//		viewRect.setY(viewRect.y() - topExtra +  bottomExtra);
//	}
//	if (bottomExtra > topExtra)
//	{
//		viewRect.setHeight(viewRect.height() + bottomExtra - topExtra);
//		viewRect.setY(viewRect.y() - bottomExtra + topExtra);
//	}

	viewRect.adjust(-20,-20,20,20); // Add some margin
	sceneRect.adjust(-20,-20,20,20); // Add some margin

	//sceneRect = viewRect.united(sceneRect);
	setSceneRect(sceneRect);
	for(auto v: views()) v->setSceneRect(viewRect);
}

void Scene::setUpdateItemGeometryWhenZoomChanges(Item* item, bool update = true)
{
	Q_ASSERT(item);
	if (update) itemsToUpdateGeometryWhenZoomChanges_.insert(item);
	else itemsToUpdateGeometryWhenZoomChanges_.remove(item);
}

void Scene::setMainViewScalingFactor(qreal factor)
{
	if (factor != mainViewScalingFactor_)
	{
		mainViewScalingFactor_ = factor;
		mainViewScalingFactorChanged_ = true;
		scheduleUpdate();
	}
}


// Reimplemented in order to detect mouse clicks
void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	isCurrentMousePressAClick_ = isCurrentMousePressAClick_
				&& lastMousePressTimer_.elapsed() <= MAX_MILLISECONDS_FOR_A_CLICK;
	QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	lastMousePressTimer_.start();
	isCurrentMousePressAClick_ = true;
	QGraphicsScene::mousePressEvent(mouseEvent);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	isCurrentMousePressAClick_ = isCurrentMousePressAClick_
			&& lastMousePressTimer_.elapsed() <= MAX_MILLISECONDS_FOR_A_CLICK;
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}


void Scene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	isCurrentMousePressAClick_ = false;
	QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void Scene::setCurrentPaintView(View* view)
{
	Q_ASSERT((view != nullptr) != (currentPaintView_ != nullptr));
	currentPaintView_ = view;
}

}
