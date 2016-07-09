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

#include "../visualizationbase_api.h"
#include "ItemMacros.h"
#include "ItemRegion.h"
#include "../InteractionHandler.h"
#include "../Scene.h"
#include "../cursor/Cursor.h"
#include "../renderer/ModelRenderer.h"
#include "../overlays/OverlayAccessor.h"

#include "ModelBase/src/nodes/Node.h"

namespace Visualization {

class Shape;
class ShapeStyle;
class ItemStyle;
class VisualizationAddOn;
class SequentialLayout;
class OverlayAccessor;

class VISUALIZATIONBASE_API Item : public QGraphicsItem
{
	DECLARE_TYPE_ID_BASE

	public:
		typedef ItemStyle StyleType;
		const static int LAYER_DEFAULT_Z = 0;
		const static int LAYER_OVERLAY_Z = 10;
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

		/**
		 * Returns the first ancestor with a node, or nullptr if none such exists. If the item itself contains a node,
		 * returns this.
		 */
		Item* findAncestorWithNode();

		/**
		 * Returns the first ancestor which is of the given template type, or nullptr if none exists.
		 */
		template <typename ItemType>
		ItemType* findAncestorOfType();

		bool itemOrChildHasFocus() const;
		bool hasSceneCursor() const;

		/**
		 * \brief Returns the child item which is focused or which contains a child that is focused or nullptr if no child
		 *        has focus.
		 */
		virtual Item* focusedChild() const;

		virtual QRectF boundingRect() const;

		int widthInLocal() const;
		int heightInLocal() const;
		QSizeF sizeInLocal() const;

		int widthInParent() const;
		int heightInParent() const;
		QSizeF sizeInParent() const;

		int widthInScene() const;
		int heightInScene() const;
		QSizeF sizeInScene() const;

		qreal xEndInParent() const;
		qreal yEndInParent() const;

		int fromParent(int x) const;
		int fromScene(int x) const;

		qreal totalScale() const;

		virtual bool sizeDependsOnParent() const;

		enum UpdateType {
			NoUpdate, /**< The item should not be updated. */
			StandardUpdate, /**< Update the geometry of the item and properties depending on child items. This update
									should be used when a child item has changed and the parent just needs to account for this
									change.*/
			FullUpdate, /**< The item and all its children will be updated using a StandardUpdate. */
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

		/**
		 * Requests that the item updates the position of its children and its size.
		 *
		 * If any of \a availableWidth or \a availableHeight are different from 0, the item is allowed to stretch to fill
		 * any available space as indicated. Both arguments are in the parent coordinate system and will be transformed
		 * using the item's transformations before they are used.
		 *
		 * This method adjusts the passed arguments and calls updateGeometry() to do the actual rearranging.
		 */
		virtual void changeGeometry(int availableWidth = 0, int availableHeight = 0);

		virtual bool isEmpty() const;

		virtual void setStyle(const ItemStyle* style);
		const ItemStyle* style() const;
		bool hasShape() const;
		Shape* getShape() const;
		void useShape();
		void removeShape();
		virtual QColor customShapeColor() const;

		enum CursorMoveDirection {
			MoveUp, /**< Move the cursor up from its current position within the item. */
			MoveDown, /**< Move the cursor down from its current position within the item. */
			MoveLeft, /**< Move the cursor left from its current position within the item. */
			MoveRight, /**< Move the cursor right from its current position within the item. */

			MoveOnPosition, /**< Move the cursor as close as possible to the specified reference rect within the item. */
			MoveOnTop, /**< Move the cursor to the top of the item*/
			MoveOnLeft, /**< Move the cursor to the left of the item*/
			MoveOnBottom, /**< Move the cursor to the bottom of the item*/
			MoveOnRight, /**< Move the cursor to the right of the item*/
			MoveOnTopLeft, /**< Move the cursor to the top-left corner of the item*/
			MoveOnBottomRight, /**< Move the cursor to the bottom-right corner of the item*/
			MoveOnCenter, /**< Move the cursor to the center of the item*/

			MoveUpOf, /**< Move the cursor as close as possible above the specified reference rect. */
			MoveDownOf, /**< Move the cursor as close as possible below the specified reference rect. */
			MoveLeftOf, /**< Move the cursor as close as possible to left of the specified reference rect. */
			MoveRightOf, /**< Move the cursor as close as possible to right of the specified reference rect. */
			MoveDefault /**< Move the cursor to the default location. This is typically used after the item is created.
			 	 	 	 	 	  The default behavior is to move the cursor to the top left corner. If a defaultMove cursor
			 	 	 	 	 	  proxy is set, it will be selected instead. Subclasses might reimplement moveCursor to
			 	 	 	 	 	  customize this behavior. */
		};

		enum CursorMoveOption {
			None = 0,
			NoLimitDistance = 0x00000001, /**< If set, the cursor can be moved to any other location on the canvas.
														Otherwise, the move distance is limited to a specific number of pixels,
														in order to avoid big jumps that disorient the user. */
			LinelikeAlongX = 0x00000002, /**< If set, overrides the ItemStyle::linelikeCursorMovementAlongX property. */
			LinelikeAlongY = 0x00000004 /**< If set, overrides the ItemStyle::linelikeCursorMovementAlongY property. */
		};
		using CursorMoveOptions = QFlags<CursorMoveOption>;

		/**
		 * \brief Moves the position of the current main scene cursor within the item and returns true on success.
		 *
		 * The \a dir parameter determines how the cursor will be moved. Some modes of movement require a reference rect
		 * provided in \a reference in item local coordinates.
		 *
		 * The options parameter specifies additional options to control the move.
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
		virtual bool moveCursor(CursorMoveDirection dir = MoveDefault, QRect reference = {},
										CursorMoveOptions options = None);

		/**
		 * Overloaded version. Uses a QPoint as a \a reference.
		 */
		bool moveCursor(CursorMoveDirection dir, QPoint reference, CursorMoveOptions options = None);
		void setDefaultMoveCursorProxy(Item* proxy);

		/**
		 * Returns a list of regions that determine where there are curosrs and where there are child items in this item.
		 *
		 * All region coordinates are in item local coordinates.
		 */
		virtual QList<ItemRegion> regions();

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

		virtual InteractionHandler* handler() const;
		void execute(const QString& command, const std::unique_ptr<Cursor>& );

		void removeFromScene();

		ModelRenderer* renderer();

		template <typename T>
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
		using PositionConstraints = QFlags<PositionConstraint>;

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

		/**
		 * \brief Returns the child item that is visualizing \a node or nullptr if no such child exists.
		 *
		 * Depth-first search is used.
		 */
		virtual Item* findVisualizationOf(Model::Node* node);
		virtual QList<Item*> findAllVisualizationsOf(Model::Node* node);

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
		 * Returns what semantic zoom level should the children of this item be chosen for when deciding what
		 * visualizations to use.
		 *
		 * If the item's own semantic zoom level has been set, it's value will be returned. Otherwise the value of the
		 * item's parent will be returned. If the item has no parent the return value is an unspecified semantic zoom
		 * level (-1).
		 */
		int semanticZoomLevel() const;

		void setSemanticZoomLevel(int semanticZoomLevel);
		void clearSemanticZoomLevel();

		int childNodeSemanticZoomLevel(const Model::Node* node) const;
		void setChildNodeSemanticZoomLevel(const Model::Node* node, int semanticZoomLevel);
		void clearChildNodeSemanticZoomLevel(const Model::Node* node);
		bool definesChildNodeSemanticZoomLevel(const Model::Node* node) const;

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

		/**
		 * Returns the scaling factor applied by the main view associated of this item's scene
		 */
		qreal mainViewScalingFactor() const;

		/**
		 * Returns the scaling factor used before the active one
		 */
		qreal previousMainViewScalingFactor() const;

		/**
		 * Returns true if the item should be updated if its scale changes, e.g. when the user zooms in or out.
		 *
		 * It is assumed that this method will always return the same value for the entire lifetime of a single item.
		 */
		virtual bool isSensitiveToScale() const;

		/**
		 * Sets the item's scale and schedules an update for all descendant items that are sensitive to scale changes.
		 */
		void setScale(qreal newScale);

		/**
		 * Inserts elements that are not yet in store and adjusts the order to match that in def.
		 */
		template <typename Definition, typename Store, typename CompareFunction, typename CreateFunction,
					 typename SyncFunction, typename DeleteFunction>
		static bool synchronizeCollections(Item* parent, const Definition& def, Store& store, CompareFunction compare,
											 CreateFunction create, SyncFunction sync, DeleteFunction deleter);

		/**
		 * Returns all the overlays of this item for the specified group. If no group is specified returns the overlays
		 * in all groups.
		 *
		 * All associated overlays are returned, regardless of the position of this item in the association.
		 */
		QList<OverlayAccessor*> overlays(QString overlayGroup = QString::null) const;

		/**
		 * Returns the overlay of the specified \a OverlayType that is associated with this item.
		 *
		 * If \a overlayGroup is specified only overlays in that group will be searched, otherwise all overlay groups
		 * are searched. All overlays are searched, regardless of the position of this item in the association.
		 *
		 * Returns null if no overlay is found.
		 *
		 * This method will return the first found overlay, in case there are more than one overlays that match
		 * \a OverlayType and \a overlayGroup.
		 */
		template <typename OverlayType> OverlayType* overlay(QString overlayGroup = QString::null);

		/**
		 * Adds \a overlay to this item.
		 *
		 * The overlay becomes part of the \a groupName overlay group.
		 *
		 * Returns the added overlay.
		 */
		template <typename OverlayType> OverlayType* addOverlay(OverlayType* overlay, QString groupName);

		/**
		 * Returns whether this item should ignore copy and paste hanling and let the parent handler take care of this.
		 *
		 * This is useful to skip the default copy and paste handlers and let a parent handler implement specialized
		 * behavior.
		 *
		 * The default implementation returns false. Reimplement in derived classes that belong to a special hierarchy
		 * where some parent object should handle copy and paste (e.g. Expressions or Queries).
		 */
		virtual bool ignoresCopyAndPaste();


	protected:

		void setWidth(int width);
		void setHeight(int height);
		void setSize(int width, int height);
		void setSize(const QSizeF& size);

		virtual void determineChildren() = 0;

		/**
		 * This method sets the position of child elements, stretching them if needed, and determines the final size
		 * of the item.
		 *
		 * If any of \a availableWidth or \a availableHeight is different than 0, the item is allowed to stretch in
		 * the specified dimension until it fills all the available space. Both parameters are in item local coordinates.
		 *
		 * Reimplemnet in derived classes to define the behavior.
		 */
		virtual void updateGeometry(int availableWidth, int availableHeight) = 0;

		/**
		 * This is a helper method that classes can use when reimplementing updateGeometry().
		 *
		 * It does the appropriate update, if the current item is a thin wrapper around \a content.
		 */
		void updateGeometry(Item* content, int availableWidth, int availableHeight);

		bool synchronizeItem(Item*& item, Model::Node* node);
		template <typename FieldType, typename VisualizationType = FieldType>
		bool synchronizeItem(FieldType*& item, bool present, const typename VisualizationType::StyleType* style);
		template <typename FieldType, typename VisualizationType = FieldType>
		bool synchronizeItem(FieldType*& item, typename VisualizationType::NodeType* node,
				const typename VisualizationType::StyleType* style);

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
		friend class SequentialLayoutFormElement;
		friend class DynamicGridFormElement;
		template <typename ParentType> friend class NodeItemWrapperFormElement;
		template <typename ParentType, typename VisualizationType> friend class NodeWithVisualizationItemWrapperFormElement;
		template <typename ParentType, typename VisualizationType, bool externalSynchronization>
			friend class VisualizationItemWrapperFormElement;
		template <typename ChildItem, typename Style, bool use>
			friend struct VisualizationItemWrapperFormElementSyncMethod;

		QRectF boundingRect_;
		const ItemStyle* style_;
		Shape* shape_;

		UpdateType needsUpdate_;
		Item* defaultMoveCursorProxy_{};

		int purpose_;
		QMap<const Model::Node*, int> childNodePurpose_;

		int semanticZoomLevel_;
		QMap<const Model::Node*, int> childNodeSemanticZoomLevel_;

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
Q_DECLARE_OPERATORS_FOR_FLAGS(Item::CursorMoveOptions)

inline Item* Item::parent() const {return static_cast<Item*>(parentItem()); }

inline int Item::widthInLocal() const { return boundingRect_.width(); }
inline int Item::heightInLocal() const { return boundingRect_.height(); }
inline QSizeF Item::sizeInLocal() const { return boundingRect_.size();}

// TODO: Support other transformations if necessary
inline int Item::widthInParent() const
{auto s = scale(); return s == 1.0 ? widthInLocal() : std::ceil(widthInLocal()*s);}
inline int Item::heightInParent() const
{auto s = scale(); return s == 1.0 ? heightInLocal() : std::ceil(heightInLocal()*s);}
inline QSizeF Item::sizeInParent() const
{auto s = scale(); return s == 1.0 ? sizeInLocal() : sizeInLocal()*s;}

// TODO: Support other transformations if necessary
inline int Item::widthInScene() const
{auto s = totalScale(); return s == 1.0 ? widthInLocal() : std::ceil(widthInLocal()*s);}
inline int Item::heightInScene() const
{auto s = totalScale(); return s == 1.0 ? heightInLocal() : std::ceil(heightInLocal()*s);}
inline QSizeF Item::sizeInScene() const
{auto s = totalScale(); return s == 1.0 ? sizeInLocal() : sizeInLocal()*s;}

// TODO: Support other transformations if necessary
inline int Item::fromParent(int x) const
{auto s = scale(); return s == 1.0 ? x : std::ceil(x/s);}
inline int Item::fromScene(int x) const
{auto s = totalScale(); return s == 1.0 ? x : std::ceil(x/s);}

inline qreal Item::xEndInParent() const { return x() + widthInParent() - 1; }
inline qreal Item::yEndInParent() const { return y() + heightInParent() - 1; }

inline const ItemStyle* Item::style() const { return style_; }
inline bool Item::hasShape() const { return shape_; }
inline Shape* Item::getShape() const {	return shape_; }
inline void Item::setDefaultMoveCursorProxy(Item* proxy) {defaultMoveCursorProxy_ = proxy;}

inline Scene* Item::scene() const { return static_cast<Visualization::Scene*> (QGraphicsItem::scene()); }
inline bool Item::hasSceneCursor() const { auto mc = scene()->mainCursor(); return mc && mc->owner() == this;}

inline bool Item::synchronizeItem(Item*& item, Model::Node* node) { return renderer()->sync(item, this, node); }

template <typename FieldType, typename VisualizationType>
bool Item::synchronizeItem(FieldType*& item, bool present, const typename VisualizationType::StyleType* style)
{
	bool changed = false;
	if (item && !present )
	{
		changed = true;
		SAFE_DELETE_ITEM(item);
		setUpdateNeeded(StandardUpdate);
	}

	if (!item && present)
	{
		changed = true;
		if (style) item = new VisualizationType{this, style};
		else item = new VisualizationType{this};

		setUpdateNeeded(StandardUpdate);
	}

	if (item && !changed) item->setStyle(style);

	return changed;
}

template <typename FieldType, typename VisualizationType>
bool Item::synchronizeItem(FieldType*& item, typename VisualizationType::NodeType* node,
		const typename VisualizationType::StyleType* style)
{
	bool changed = false;

	if (item && item->node() != node )
	{
		changed = true;
		SAFE_DELETE_ITEM(item);
		setUpdateNeeded(StandardUpdate);
	}

	if (!item && node)
	{
		changed = true;
		if (style) item = new VisualizationType{this, node, style};
		else item = new VisualizationType{this, node};

		setUpdateNeeded(StandardUpdate);
	}

	if (item && !changed) item->setStyle(style);

	return changed;
}

template <typename Definition, typename Store, typename CompareFunction, typename CreateFunction,
			 typename SyncFunction, typename DeleteFunction>
bool Item::synchronizeCollections(Item* parent, const Definition& def, Store& store, CompareFunction compare,
											 CreateFunction create, SyncFunction sync,
											 DeleteFunction deleter)
{
	static_assert(std::is_pointer<decltype(store.value(0))>::value, "the elements of store must be pointers");

	bool changed = false;

	for (int i = 0; i < def.size(); ++i)
	{
		if (i >= store.size() ) // This element is new
		{
			changed = true;
			store.append(create(parent, def[i]));
		}
		else if ( compare(def[i], store[i]) )// This element is already there, sync it
		{
			changed = sync(parent, def[i], store[i]) | changed;
		}
		else // This element might appear somewhere ahead, we should look for it
		{
			changed = true;

			bool found = false;
			for (int k = i + 1; k<store.size(); ++k)
			{
				if ( compare(def[i], store[k]) )
				{
					// We found this element, swap the positions
					auto temp = store[i];
					store[i] = store[k];
					store[k] = temp;
					sync(parent, def[i], store[i]);

					found = true;
					break;
				}
			}

			// The node was not found, insert a visualization here
			if (!found ) store.insert( i, create(parent, def[i]) );
		}
	}

	// Remove excess items
	while (store.size() > def.size())
	{
		changed = true;
		deleter(store.last());
		store.pop_back();
	}

	if (changed && parent) parent->setUpdateNeeded(StandardUpdate);
	return changed;
}

/**
 * Do not make this a non-template function since then the reference to pointer won't work as it is not polymorphic.
 */
template <typename T> inline void SAFE_DELETE_ITEM(T* const & item)
{
	if (item)
		item->removeFromScene();
}

/**
 * Do not make this a non-template function since then the reference to pointer won't work as it is not polymorphic.
 */
template <typename T> inline void SAFE_DELETE_ITEM( T* & item)
{
	if (item)
	{
		item->removeFromScene();
		SAFE_DELETE(item);
	}
}

/**
 * Do not make this a non-template function since then the reference to pointer won't work as it is not polymorphic.
 */
template <typename T> inline void SAFE_DELETE_ITEM( T* && item)
{
	if (item)
	{
		item->removeFromScene();
		SAFE_DELETE(item);
	}
}

template <typename T> T* Item::correspondingSceneCursor()
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
	while (item && !dynamic_cast<Item*>(item)) item = item->parentItem();
	return static_cast<Item*>(item);
}

inline void Item::setItemCategory( Scene::ItemCategory cat) { itemCategory_ = cat; }
inline bool Item::isCategoryHiddenDuringPaint() { return scene()->isHiddenCategory(itemCategory()); }

inline void Item::setDefaultClassHandler(InteractionHandler* handler) {defaultClassHandler_ = handler;}
inline InteractionHandler* Item::defaultClassHandler() {return defaultClassHandler_;}

template <typename OverlayType> OverlayType* Item::overlay(QString overlayGroup)
{
	for (auto accessor : overlays(overlayGroup))
	{
		if (auto item = DCast<OverlayType>(accessor->overlayItem()))
			return item;
	}

	return nullptr;
}

template <typename OverlayType> OverlayType* Item::addOverlay(OverlayType* overlay, QString groupName)
{
	Q_ASSERT(overlay);
	auto overlayGroup = scene()->overlayGroup(groupName);
	if (!overlayGroup) overlayGroup = scene()->addOverlayGroup(groupName);
	overlayGroup->addOverlay(makeOverlay(overlay));

	return overlay;
}

template <typename ItemType> ItemType* Item::findAncestorOfType()
{
	auto result = this;
	while (result && !DCast<ItemType>(result))
		result = result->parent();
	return DCast<ItemType>(result);
}

inline bool Item::moveCursor(CursorMoveDirection dir, QPoint reference, CursorMoveOptions options)
{
	return this->moveCursor(dir, QRect{reference.x(), reference.y(), 1, 1}, options);
}

}
