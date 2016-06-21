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

#pragma once

#include "visualizationbase_api.h"
#include "overlays/OverlayGroup.h"
#include "CustomSceneEvent.h"

namespace Model {
	class TreeManager;
	class Node;
}

namespace Visualization {

class Item;
class ViewItem;
class ModelRenderer;
class SceneHandlerItem;
class Cursor;
class View;
class ViewItemManager;

struct MajorMinorIndex
{
	int major_{0};
	int minor_{0};

	bool operator==(const MajorMinorIndex& other) const { return major_==other.major_ && minor_==other.minor_; }
	bool operator!=(const MajorMinorIndex& other) const { return ! (*this == other); }
};

class VISUALIZATIONBASE_API Scene : public QGraphicsScene
{
	Q_OBJECT

	private:

		// This is needed in order to make the Signals and Slots mechanism work. Otherwise we are not able to connect to
		// the signal provided from TreeManager. This is because the signatures of the two methods, must match exactly
		// (stringwise).
		typedef Model::Node Node;

	public:

		using RefreshActionFunction = std::function<void (Scene* scene)>;
		using OnZoomHandler = std::function<void (qreal factor)>;
		using OnZoomHandlerRemove = std::function<void ()>;

		Scene();
		virtual ~Scene();
		void setRenderer(ModelRenderer* renderer);

		ModelRenderer* renderer();
		static ModelRenderer* defaultRenderer();

		void addTopLevelNode(Node* node, MajorMinorIndex index = {});
		void addTopLevelItem(Item* item, bool show = true);
		void removeTopLevelItem(Item* item);
		void scheduleUpdate();
		void updateNow();
		void listenToTreeManager(Model::TreeManager* manager);

		Cursor* mainCursor();
		void setMainCursor(Cursor* cursor);

		virtual void customEvent(QEvent *event) override;

		virtual SceneHandlerItem* sceneHandlerItem();

		ViewItemManager* viewItems() const;
		/**
		 * Convenience method to get the manager's current view item.
		 */
		ViewItem* currentViewItem();

		/**
		 * Adds an action to be executed after the current event.
		 *
		 * This method can only be called while handling an event.
		 */
		void addPostEventAction(QEvent* action);
		void addPostEventAction(CustomSceneEvent::EventFunction function);

		enum ItemCategory {
			NoItemCategory = 0x0,
			CodeItemCategory = 0x1,
			MenuItemCategory = 0x2,
			SelectionItemCategory = 0x4,
			CursorItemCategory = 0x8
		};
		using ItemCategories = QFlags<ItemCategory>;

		void setHiddenItemCategories( ItemCategories hidden = NoItemCategory);
		bool isHiddenCategory(ItemCategory cat);

		const QList<Item*>& topLevelItems() const;

		void addRefreshActionFunction(RefreshActionFunction func);
		void addOnZoomHandler(OnZoomHandler onZoomHandler, OnZoomHandlerRemove onZoomHandlerRemove);
		void removeOnZoomHandlers();

		/**
		 * Returns the focused item if it is an instance of Item* or the closest of its ancestors that is an Item*.
		 *
		 * This method hides QGraphicsScene::focusItem() const.
		 */
		Item* focusItem() const;

		/**
		 * Returns a list of selected items.
		 *
		 * Only items that derive from Item are returned. If an item is selected but does not derive from Item then its
		 * first Item ancestor will be returned in the list, if it is not already in the list.
		 *
		 * This method hised QGraphicsScene::selectedItems() const
		 */
		QList<Item*> selectedItems() const;

		bool isCurrentMousePressAClick() const;
		QPointF lastMouseHoverPosition() const;

		View* currentPaintView() const;

		void setItemIsSensitiveToScale(Item* item, bool update);

		/**
		 * Returns the scaling factor applied by the main view associated of this item's scene
		 */
		qreal mainViewScalingFactor() const;
		void setMainViewScalingFactor(qreal factor);

		/**
		 * Returns the scaling factor used before the active one
		 */
		qreal previousMainViewScalingFactor() const;

		/**
		 * Returns all existing scenes.
		 */
		static QList<Scene*>& allScenes();

		OverlayGroup* addOverlayGroup(const QString& name);
		OverlayGroup* overlayGroup(const QString& name);
		QList<OverlayGroup*> allOverlayGroups() const;
		void removeOverlayGroup(const QString& name);
		void removeOverlayGroup(OverlayGroup* group);
		void removeOverlayOf(Item* itemWithOverlay, const QString& groupName = {});
		void removeOverlay(Item* overlay, const QString& groupName = {});

		void setApproximateUpdate(bool b);
		bool approximateUpdate();

		void nodesUpdated(QSet<Node*> modifiedNodes, QSet<Node*> removedNodes);

	protected:
		virtual bool event(QEvent* event) override;
		virtual void keyPressEvent(QKeyEvent* event) override;

		// Reimplemented in order to detect mouse clicks
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) override;
		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent) override;

	private:
		friend class ViewItemManager;

		bool needsUpdate_{};
		bool initialized_{};
		bool approximateUpdate_{};

		ModelRenderer* renderer_{};
		SceneHandlerItem* sceneHandlerItem_{};
		QList<Item*> topLevelItems_;

		// We use pointers for the value as we need a way to detach groups before deleting them.
		QHash<QString, OverlayGroup*> overlayGroups_;

		ViewItemManager* viewItemManager_{};

		Cursor* mainCursor_{};
		bool mainCursorsJustSet_{};
		QList<QEvent*> postEventActions_;

		bool inEventHandler_{};
		bool inAnUpdate_{};

		qreal mainViewScalingFactor_{1.0};
		qreal previousMainViewScalingFactor_{mainViewScalingFactor_};
		bool mainViewScalingFactorChanged_{false};

		ItemCategories hiddenItemCategories_;

		bool isCurrentMousePressAClick_{};
		QElapsedTimer lastMousePressTimer_{};
		QPointF lastMouseHoverPosition_;
		const int MAX_MILLISECONDS_FOR_A_CLICK = 500;

		QList<RefreshActionFunction> refreshActionFunctions_;
		QList<QPair<OnZoomHandler, OnZoomHandlerRemove>> onZoomHandlers_;

		QSet<Item*> itemsSensitiveToScale_;

		View* currentPaintView_{};
		friend class View;
		void setCurrentPaintView(View* view);

		void computeSceneRect();

		QList<Item*> itemsThatShouldHaveASelection();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Scene::ItemCategories)
inline void Scene::setHiddenItemCategories( ItemCategories hidden) {hiddenItemCategories_ = hidden;}
inline bool Scene::isHiddenCategory(ItemCategory cat) {return cat & hiddenItemCategories_;}

inline void Scene::setRenderer(ModelRenderer* renderer) { renderer_ = renderer? renderer : defaultRenderer(); }
inline ModelRenderer* Scene::renderer() { return renderer_; }
inline SceneHandlerItem* Scene::sceneHandlerItem() {return sceneHandlerItem_; }
inline Cursor* Scene::mainCursor() { return mainCursor_; }
inline const QList<Item*>& Scene::topLevelItems() const {return topLevelItems_; }
inline void Scene::addRefreshActionFunction(RefreshActionFunction func) {refreshActionFunctions_.append(func); }

inline void Scene::addOnZoomHandler(OnZoomHandler onZoomHandler, OnZoomHandlerRemove onZoomHandlerRemove)
	{onZoomHandlers_.append({onZoomHandler, onZoomHandlerRemove});}

inline bool Scene::isCurrentMousePressAClick() const { return isCurrentMousePressAClick_; }
inline QPointF Scene::lastMouseHoverPosition() const { return lastMouseHoverPosition_; }
inline View* Scene::currentPaintView() const { return currentPaintView_; }

inline qreal Scene::mainViewScalingFactor() const { return mainViewScalingFactor_; }
inline qreal Scene::previousMainViewScalingFactor() const { return previousMainViewScalingFactor_; }

inline ViewItemManager* Scene::viewItems() const { return viewItemManager_; }

inline void Scene::setApproximateUpdate(bool b) { approximateUpdate_ = b; }
inline bool Scene::approximateUpdate() { return approximateUpdate_; }

inline QList<OverlayGroup*> Scene::allOverlayGroups() const {return overlayGroups_.values();}

}
