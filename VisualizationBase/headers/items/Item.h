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
 * Item.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ITEM_H_
#define ITEM_H_

#include "../visualizationbase_api.h"
#include "ItemMacros.h"
#include "../InteractionHandler.h"
#include "../Scene.h"

#include "ModelBase/headers/nodes/Node.h"

#include "Core/headers/global.h"

#include <QtGui/QGraphicsItem>

namespace Visualization {

class Shape;
class ShapeStyle;
class ItemStyle;
class ModelRenderer;

class VISUALIZATIONBASE_API Item : public QGraphicsItem
{
	private:
		friend class Shape;
		friend class InteractionHandler;
		QRectF boundingRect_;
		const ItemStyle* style_;
		Shape* shape_;
		bool needsUpdate_;

		void updateChildren();

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

		//Event handlers
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

	public:
		typedef ItemStyle StyleType;
		const static int LAYER_DEFAULT_Z = 0;
		const static int LAYER_SELECTION_Z = 100;
		const static int LAYER_CURSOR_Z = 200;

		Item(Item* parent, const StyleType* style = nullptr);
		virtual ~Item();

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

		void setUpdateNeeded();

		virtual bool needsUpdate();
		virtual void updateSubtree();
		virtual void changeGeometry(int availableWidth = 0, int availableHeight = 0);

		virtual bool isEmpty() const;

		virtual void setStyle(const ItemStyle* style);
		const ItemStyle* style() const;
		bool hasShape() const;
		Shape* getShape() const;
		void useShape();
		void removeShape();

		enum FocusTarget {FOCUS_DEFAULT, FOCUS_TOPMOST, FOCUS_BOTTOMMOST, FOCUS_LEFTMOST, FOCUS_RIGHTMOST, FOCUS_UP, FOCUS_DOWN, FOCUS_LEFT, FOCUS_RIGHT};
		virtual bool focusChild(FocusTarget location);
		bool focusChild(Item* child);

		/**
		 * \brief Reimplement this method in derived classes to create a new cursor and set it as the main scene cursor.
		 *
		 * The default implementation creates a cursor with no visualization whose position is the item's scene position.
		 *
		 * This call is typically followed by a call to \a moveCursor() to set a specific position for the cursor within
		 * the item.
		 */
		virtual void createDefaultCursor();

		enum CursorMoveDirection {
			MoveUp, /**< Move the cursor up from its current position within the item. */
			MoveDown, /**< Move the cursor down from its current position within the item. */
			MoveLeft, /**< Move the cursor left from its current position within the item. */
			MoveRight, /**< Move the cursor right from its current position within the item. */
			MoveOnPosition, /**< Move the cursor as close as possible to the specified reference point within the item. */
			MoveUpOf, /**< Move the cursor as close as possible above the specified reference point. */
			MoveDownOf, /**< Move the cursor as close as possible below the specified reference point. */
			MoveLeftOf, /**< Move the cursor as close as possible to left of the specified reference point. */
			MoveRightOf /**< Move the cursor as close as possible to right of the specified reference point. */
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
		 * the combination of \a dir and \a reference is not satisfiable. The item is assumed to have no children.
		 *
		 * If the main scene cursor is not currently owned by this item, the default implementation will call
		 * createDefaultCursor() to create a suitable cursor. Classes which reimplement this method should also call
		 * createDefaultCursor() in case the current main cursor does not correspond to this item.
		 */
		virtual bool moveCursor(CursorMoveDirection dir, const QPoint& reference = QPoint());

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
				Overlap = 0x16
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

		inline qreal xEnd() const;
		inline qreal yEnd() const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Item::PositionConstraints)

inline int Item::width() const { return boundingRect_.width(); }
inline int Item::height() const { return boundingRect_.height(); }
inline QSizeF Item::size() const { return boundingRect_.size();}
inline void Item::setWidth(int width)  { boundingRect_.setWidth(width); };
inline void Item::setHeight(int height) { boundingRect_.setHeight(height);};
inline void Item::setSize(int width, int height) { boundingRect_.setSize(QSizeF(width, height)); };
inline void Item::setSize(const QSizeF& size) { boundingRect_.setSize(size); };
inline const ItemStyle* Item::style() const { return style_; }
inline bool Item::hasShape() const { return shape_; }
inline Shape* Item::getShape() const {	return shape_; }
inline qreal Item::xEnd() const { return x() + width(); }
inline qreal Item::yEnd() const { return y() + height(); }

template <class T> void Item::synchronizeItem(T*& item, bool present, const typename T::StyleType* style)
{
	if (item && !present )
	{
		SAFE_DELETE_ITEM(item);
		setUpdateNeeded();
	}

	if (!item && present)
	{
		if (style) item = new T(this, style);
		else item = new T(this);

		item->setParent(this);
		setUpdateNeeded();
	}
}

template <class T> void Item::synchronizeItem(T*& item, typename T::NodeType* node, const typename T::StyleType* style)
{
	if (item && item->node() != node )
	{
		SAFE_DELETE_ITEM(item);
		setUpdateNeeded();
	}

	if (!item && node)
	{
		if (style) item = new T(nullptr, node, style);
		else item = new T(nullptr, node);

		item->setParentItem(this);
		setUpdateNeeded();
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

}

#endif /* ITEM_H_ */
