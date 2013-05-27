/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "../visualizationbase_api.h"
#include "ItemMacros.h"
#include "ItemRegion.h"
#include "../InteractionHandler.h"
#include "../Scene.h"

#include "ModelBase/src/nodes/Node.h"

namespace Visualization {

class Shape;
class ShapeStyle;
class ItemStyle;
class ModelRenderer;
class VisualizationAddOn;
class SequentialLayout;

class VISUALIZATIONBASE_API Item : public QGraphicsItem
{
	DECLARE_TYPE_ID

	public:
		typedef ItemStyle StyleType;
		const static int LAYER_DEFAULT_Z = 0;
		const static int LAYER_COMMAND = 25;
		const static int LAYER_AUTOCOMPLETE_Z = 50;
		const static int LAYER_SELECTION_Z = 100;
		const static int LAYER_CURSOR_Z = 200;

		Item(Item* parent, const StyleType* style = nullptr);
		virtual ~Item();

		Item* parent() const;
		Scene* scene() const;

		virtual bool hasNode() const;
		virtual Model::Node* node() const;
		virtual int revision() const;
		virtual void setRevision(int newRevision);

		bool itemOrChildHasFocus() const;

		/**
		 * \brief Returns the child item which is focused or which contains a child that is focused or nullptr if no child
		 *        has focus.
		 */
		virtual Item* focusedChild() const;

		virtual QRectF boundingRect() const;
		int width() const;
		int height() const;
		QSizeF size() const;
		virtual bool sizeDependsOnParent() const;

		enum UpdateType {
			NoUpdate, /**< The item should not be updated. */
			StandardUpdate, /**< Update the geometry of the item and properties depending on child items. This update
									should be used when a child item has changed and the parent just needs to account for this
									change.*/
			FullUpdate, /**< The item should completely update itself and recreate all child items. This should be used
								when the item has been directly edited, or when it's style, purpose or similar defining
								properties have changed. */
			RepeatUpdate /**< This update is similar to StandardUpdate but can also be requested while the item is being
			 	 	 	 	 	updated. Normally, update requests which happen while an item is being updated are disregarded.
			 	 	 	 	 	If a RepeatUpdate is requested, the item will be updated one more time after the current update
			 	 	 	 	 	is finished. This is useful when the item needs to consider how its children are drawn (their
			 	 	 	 	 	size) and update accordingly.*/
		};
		void setUpdateNeeded(UpdateType updateType);

		/**
		 * Finds the currently existing direct or indirect child Item that visualizes \a nodeVisualizedByChild and sets
		 * it update status to \a updateType.
		 *
		 * If there is no child item which yet visualizes \a nodeVisualizedByChild then this method will search for the
		 * Item which visualizes the parent of \a nodeVisualizedByChild. This will continue recursively until an existing
		 * visualization of one of the ancestors of \a nodeVisualizedByChild is found.
		 *
		 * Use this method when a new node has been created and the a visualization needs to be updated in order to
		 * display it.
		 */
		void setUpdateNeededForChildItem(UpdateType updateType, Model::Node* nodeVisualizedByChild);

		virtual UpdateType needsUpdate();
		virtual void updateSubtree();
		virtual void changeGeometry(int availableWidth = 0, int availableHeight = 0);

		virtual bool isEmpty() const;

		virtual void setStyle(const ItemStyle* style);
		const ItemStyle* style() const;
		bool hasShape() const;
		Shape* getShape() const;
		void useShape();
		void removeShape();

		enum CursorMoveDirection {
			MoveUp, /**< Move the cursor up from its current position within the item. */
			MoveDown, /**< Move the cursor down from its current position within the item. */
			MoveLeft, /**< Move the cursor left from its current position within the item. */
			MoveRight, /**< Move the cursor right from its current position within the item. */
			MoveOnPosition, /**< Move the cursor as close as possible to the specified reference point within the item. */
			MoveUpOf, /**< Move the cursor as close as possible above the specified reference point. */
			MoveDownOf, /**< Move the cursor as close as possible below the specified reference point. */
			MoveLeftOf, /**< Move the cursor as close as possible to left of the specified reference point. */
			MoveRightOf, /**< Move the cursor as close as possible to right of the specified reference point. */
			MoveDefault /**< Move the cursor to the default location. This is typically used after the item is created.
			 	 	 	 	 	  The default behavior is to move the cursor to the top left corner. If a defaultMove cursor
			 	 	 	 	 	  proxy is set, it will be selected instead. Subclasses might reimplement moveCursor to
			 	 	 	 	 	  customize this behavior. */
		};

		/**
		 * \brief Moves the position of the current main scene cursor within the item and returns true on success.
		 *
		 * The \a dir parameter determines how the cursor will be moved. Some modes of movement require a reference point
		 * provided in \a reference in item local coordinates.
		 *
		 * The method returns false if it is not possible to move the cursor within the borders of the current item given
		 * the provided movement parameters.
		 *
		 * The default implementation returns false when \a dir is any of MoveUp, MoveDown, MoveLeft, MoveRight or when
		 * the combination of \a dir and \a reference is not satisfiable. The item is assumed to have no children. If the
		 * item is focused a default cursor with no visualization will be created whose position is the item's scene
		 * position.
		 *
		 * This method is responsible for creating a corresponding Cursor item and setting it as the main scene cursor.
		 */
		virtual bool moveCursor(CursorMoveDirection dir = MoveDefault, QPoint reference = QPoint());
		void setDefaultMoveCursorProxy(Item* proxy);

		virtual QList<ItemRegion> regions();

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

		virtual InteractionHandler* handler() const;
		void execute(const QString& command);

		void removeFromScene();

		ModelRenderer* renderer();

		template <class T>
		T* correspondingSceneCursor();

		/**
		 * \brief Returns the distance between the item and \a point.
		 *
		 * The point \a point must be in local item coordinates.
		 */
		int distanceTo(const QPoint& point) const;

		enum PositionConstraint {
				NoConstraints = 0x0,
				Below = 0x1,
				Above = 0x2,
				LeftOf = 0x4,
				RightOf = 0x8,
				Overlap = 0x10
		};
		Q_DECLARE_FLAGS(PositionConstraints, PositionConstraint)

		/**
		 * \brief Returns the child item (if any) that has the least distance to the point \a point.
		 *
		 * The point \a point must be in the current item's local coordinates.
		 *
		 * The \a childConstraint argument can be used to limit which children are considered. Only children that match
		 * all specified constraints may be returned by this method. The \a childConstraint argument specifies the
		 * constraints from a child item's point of view, e.g. a constraint \a LeftOf will be satisfied if a child is left
		 * of the specified point.
		 *
		 * \sa satisfiedPositionConstraints
		 */
		virtual Item* childClosestTo(const QPoint& point, PositionConstraints childConstraint = NoConstraints) const;

		/**
		 * \brief Returns all position constraints with respect to the point \a point satisfied by this item.
		 *
		 * The point \a point must be in the item's local coordinates.
		 *
		 * The returned constraints are from the item's point of view, e.g. a constraint \a LeftOf means that the item is
		 * left of the specified point.
		 *
		 * \sa childClosestTo
		 */
		PositionConstraints satisfiedPositionConstraints(const QPoint& point) const;

		qreal xEnd() const;
		qreal yEnd() const;

		/**
		 * \brief Returns the child item that is visualizing \a node or nullptr if no such child exists.
		 *
		 * Depth-first search is used.
		 */
		virtual Item* findVisualizationOf(Model::Node* node);

		/**
		 * Returns what purpose should the children of this item be chosen for when deciding what visualizations to use.
		 *
		 * If the item's own purpose has been set, it's value will be returned. Otherwise the value of the item's parent
		 * will be returned. If the item has no parent the return value is an unspecified purpose (-1).
		 */
		int purpose() const;

		void setPurpose(int purpose);
		void clearPurpose();

		int childNodePurpose(const Model::Node* node) const;
		void setChildNodePurpose(const Model::Node* node, int purpose);
		void clearChildNodePurpose(const Model::Node* node);
		bool definesChildNodePurpose(const Model::Node* node) const;

		/**
		 * Registers a class derived from Item and returns the typeId for that class.
		 *
		 * Each class that is an Item should register itself to receive a unique id. This method is thread-safe so it can
		 * be called at any time.
		 *
		 * Classes that use the standard item macros, ITEM_COMMON / ITEM_COMMON_CUSTOM_STYLENAME and
		 * ITEM_COMMON_DEFINITIONS already include all required functionality.
		 */
		static int registerVisualization();

		/**
		 * Returns all visualization add-ons of this and inherited classes.
		 */
		virtual QList<VisualizationAddOn*> addOns();
		/**
		 * Adds the add-on \a addOn for the class.
		 */
		static void addAddOn(VisualizationAddOn* addOn);
		/**
		 * Removes the add-on \a addOn from the class. The add-on can only be removed if it was added directly to this
		 * class. Add-on from inherited classes can not be removed by calling removeAddOn on derived classes.
		 *
		 * Returns true if the add-on was removed and false otherwise.
		 */
		static bool removeAddOn(VisualizationAddOn* addOn);

		void setItemCategory( Scene::ItemCategory cat = Scene::NoItemCategory);
		Scene::ItemCategory itemCategory();
		bool isCategoryHiddenDuringPaint();

		static QMultiHash<Model::Node*, Item*>& nodeItemsMap();

		static void setDefaultClassHandler(InteractionHandler* handler);
		static InteractionHandler* defaultClassHandler();

		/**
		 * Returns \a item if it is an instance of Item* or the closest of its ancestors that is an Item*.
		 */
		static Item* envisionItem(QGraphicsItem* item);

		/**
		 * Returns a list of all direct children items which are an instance of Item.
		 *
		 * The default implementation assumes that all children are instances of Item and simply calls
		 * QGraphicsView::childItems(). Reimplement this in items that can contain non-Item children to return the correct
		 * list.
		 *
		 * This method hides QGraphicsItem::childItems();
		 */
		virtual QList<Item*> childItems() const;

	protected:

		void setWidth(int width);
		void setHeight(int height);
		void setSize(int widht, int height);
		void setSize(const QSizeF& size);

		virtual void determineChildren() = 0;
		virtual void updateGeometry(int availableWidth, int availableHeight) = 0;
		void updateGeometry(Item* content, int availableWidth, int availableHeight);

		void synchronizeItem(Item*& item, Model::Node* node);
		template <class T> void synchronizeItem(T*& item, bool present, const typename T::StyleType* style);
		template <class T> void synchronizeItem(T*& item, typename T::NodeType* node, const typename T::StyleType* style);

		/**
		 * Returns a map that associates each registered add-on with all items corresponding to it.
		 */
		const QMultiMap<VisualizationAddOn*, Item* >& addOnItems();

		/**
		 * Puts all items generated by add-ons inside a sequential layout.
		 *
		 * This is a helper function that can be used by any item to quickly visualize items from add-ons. The item just
		 * needs to provide a SequentialLayout that will server as the container.
		 */
		void putAddOnItemsInSequence(SequentialLayout* layout);

		//Event handlers
		virtual bool sceneEvent(QEvent *event);

		// Keyboard events
		virtual void keyPressEvent(QKeyEvent *event);
		virtual void keyReleaseEvent(QKeyEvent *event);

		// Mouse events
		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
		virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
		virtual void wheelEvent(QGraphicsSceneWheelEvent *event);

		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
		virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

		virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
		virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
		virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
		virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

		// Menu events
		virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

		// Focus events
		virtual void focusInEvent(QFocusEvent *event);
		virtual void focusOutEvent(QFocusEvent *event);

	private:
		friend class Shape;
		friend class InteractionHandler;
		QRectF boundingRect_;
		const ItemStyle* style_;
		Shape* shape_;

		UpdateType needsUpdate_;
		Item* defaultMoveCursorProxy_{};

		int purpose_;
		QMap<const Model::Node*, int> childNodePurpose_;

		Scene::ItemCategory itemCategory_;

		void updateAddOnItems();

		/**
		 * Returns a list of all add-on that have been added to this class.
		 *
		 * Use addAddOn() and removeAddOn() to change the contents of the list.
		 */
		static QList<VisualizationAddOn*>& staticAddOns();

		static InteractionHandler* defaultClassHandler_;

		QMultiMap<VisualizationAddOn*, Item* > addOnItems_;

		// Default event handlers
		//Keyboard events
		void defaultKeyPressEvent(QKeyEvent *event);
		void defaultKeyReleaseEvent(QKeyEvent *event);

		// Mouse events
		void defaultMouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
		void defaultMouseMoveEvent(QGraphicsSceneMouseEvent *event);
		void defaultMousePressEvent(QGraphicsSceneMouseEvent *event);
		void defaultMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
		void defaultWheelEvent(QGraphicsSceneWheelEvent *event);

		void defaultHoverEnterEvent(QGraphicsSceneHoverEvent *event);
		void defaultHoverLeaveEvent(QGraphicsSceneHoverEvent *event);
		void defaultHoverMoveEvent(QGraphicsSceneHoverEvent *event);

		void defaultDragEnterEvent(QGraphicsSceneDragDropEvent *event);
		void defaultDragLeaveEvent(QGraphicsSceneDragDropEvent *event);
		void defaultDragMoveEvent(QGraphicsSceneDragDropEvent *event);
		void defaultDropEvent(QGraphicsSceneDragDropEvent *event);

		// Menu events
		void defaultContextMenuEvent(QGraphicsSceneContextMenuEvent *event);

		// Focus events
		void defaultFocusInEvent(QFocusEvent *event);
		void defaultFocusOutEvent(QFocusEvent *event);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Item::PositionConstraints)

inline Item* Item::parent() const {return static_cast<Item*>(parentItem()); }
inline int Item::width() const { return boundingRect_.width(); }
inline int Item::height() const { return boundingRect_.height(); }
inline QSizeF Item::size() const { return boundingRect_.size();}
inline const ItemStyle* Item::style() const { return style_; }
inline bool Item::hasShape() const { return shape_; }
inline Shape* Item::getShape() const {	return shape_; }
inline qreal Item::xEnd() const { return x() + width() - 1; }
inline qreal Item::yEnd() const { return y() + height() - 1; }
inline void Item::setDefaultMoveCursorProxy(Item* proxy) {defaultMoveCursorProxy_ = proxy;}

template <class T> void Item::synchronizeItem(T*& item, bool present, const typename T::StyleType* style)
{
	if (item && !present )
	{
		SAFE_DELETE_ITEM(item);
		setUpdateNeeded(StandardUpdate);
	}

	if (!item && present)
	{
		if (style) item = new T(this, style);
		else item = new T(this);

		setUpdateNeeded(StandardUpdate);
	}
}

template <class T> void Item::synchronizeItem(T*& item, typename T::NodeType* node, const typename T::StyleType* style)
{
	if (item && item->node() != node )
	{
		SAFE_DELETE_ITEM(item);
		setUpdateNeeded(StandardUpdate);
	}

	if (!item && node)
	{
		if (style) item = new T(this, node, style);
		else item = new T(this, node);

		setUpdateNeeded(StandardUpdate);
	}
}

template <class T> inline void SAFE_DELETE_ITEM( T* & item)
{
	if (item)
	{
		item->removeFromScene();
		SAFE_DELETE(item);
	}
}

template <class T> T* Item::correspondingSceneCursor()
{
	//Currently ony the main cursor is supported.
	T* cursor = dynamic_cast<T*> (scene()->mainCursor());
	Q_ASSERT(cursor != nullptr);
	Q_ASSERT(cursor->owner() == this);
	return cursor;
}

inline const QMultiMap<VisualizationAddOn*, Item* >& Item::addOnItems()
{
	return addOnItems_;
}

inline Item* Item::envisionItem(QGraphicsItem* item)
{
	while(item && !dynamic_cast<Item*>(item)) item = item->parentItem();
	return static_cast<Item*>(item);
}

inline void Item::setItemCategory( Scene::ItemCategory cat) { itemCategory_ = cat; }
inline bool Item::isCategoryHiddenDuringPaint() { return scene()->isHiddenCategory(itemCategory()); }

inline void Item::setDefaultClassHandler(InteractionHandler* handler) {defaultClassHandler_ = handler;}
inline InteractionHandler* Item::defaultClassHandler() {return defaultClassHandler_;}

}
