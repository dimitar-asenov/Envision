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

#include <QtGui/QGraphicsItem>

namespace Visualization {

class Shape;
class ShapeStyle;
class ItemStyle;
class Scene;

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
		virtual void updateState() = 0;

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
		Item(Item* parent, const ItemStyle* style = NULL);
		virtual ~Item();

		Scene* scene() const;

		virtual QRectF boundingRect() const;
		int width() const;
		int height() const;
		QSizeF size() const;

		void setUpdateNeeded();

		virtual bool needsUpdate();
		virtual void updateSubtreeState();

		virtual void setStyle(const ItemStyle* style);
		const ItemStyle* style() const;
		Shape* getShape() const;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

		virtual InteractionHandler* handler() const;
		void execute(QString command);
};

inline int Item::width() const { return boundingRect_.width(); }
inline int Item::height() const { return boundingRect_.height(); }
inline QSizeF Item::size() const { return boundingRect_.size();}
inline void Item::setWidth(int width)  { boundingRect_.setWidth(width); };
inline void Item::setHeight(int height) { boundingRect_.setHeight(height);};
inline void Item::setSize(int width, int height) { boundingRect_.setSize(QSizeF(width, height)); };
inline void Item::setSize(const QSizeF& size) { boundingRect_.setSize(size); };
inline const ItemStyle* Item::style() const { return style_; }
inline Shape* Item::getShape() const {	return shape_; }
inline bool Item::needsUpdate() { return needsUpdate_; }

}

#endif /* ITEM_H_ */
