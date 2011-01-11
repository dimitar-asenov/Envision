/***********************************************************************************************************************
 * Item.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/Item.h"
#include "items/ItemStyle.h"
#include "shapes/Shape.h"
#include "shapes/ShapeStyle.h"
#include "VisualizationException.h"

namespace Visualization {

Item::Item(Item* parent, const ItemStyle* style) :
	QGraphicsItem(parent), style_(NULL), shape_(NULL), needsUpdate_(true)
{
	setStyle(style);
}

Item::~Item()
{
	if ( shape_ )
	{
		delete shape_;
		shape_ = NULL;
	}
}

void Item::setUpdateNeeded()
{
	needsUpdate_ = true;
	Item* parent = static_cast<Item*> (parentItem());
	while (parent)
	{
		parent->needsUpdate_ = true;
		parent = static_cast<Item*> (parent->parentItem());
	}
}

void Item::setStyle(const ItemStyle* style)
{
	if (shape_) delete shape_;
	style_ = style;
	if (style && style->hasShape()) shape_ = style->createShape(this);
	setUpdateNeeded();
}

void Item::updateSubtreeState()
{
	if ( needsUpdate_ || needsUpdate())
	{
		determineChildren();
		updateChildren();
		updateState();
		needsUpdate_ = false;
	}
}

void Item::updateChildren()
{
	QList<QGraphicsItem *> children = childItems();
	for (QList<QGraphicsItem *>::iterator child = children.begin(); child != children.end(); ++child)
	{
		Item* item = static_cast<Item*> (*child);
		item->updateSubtreeState();
	}
}

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ( shape_ ) shape_->paint(painter, option, widget);
	else
		throw VisualizationException("Calling Item::paint without specifying a shape.");
}

InteractionHandler* Item::handler() const
{
	return InteractionHandler::instance();
}

void Item::execute(QString command)
{
	handler()->command(this, command);
}

/***********************************************************************************************************************
 * Reimplemented Event handling methods. These simply dispatch the method call to the interaction handler of this
 * object.
 **********************************************************************************************************************/
// Keyboard events
void Item::keyPressEvent(QKeyEvent *event) { handler()->keyPressEvent(this, event); }
void Item::keyReleaseEvent(QKeyEvent *event) { handler()->keyReleaseEvent(this, event); }

// Mouse events
void Item::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) { handler()->mouseDoubleClickEvent(this, event); }
void Item::mouseMoveEvent(QGraphicsSceneMouseEvent *event) { handler()->mouseMoveEvent(this, event); }
void Item::mousePressEvent(QGraphicsSceneMouseEvent *event) { handler()->mousePressEvent(this, event); }
void Item::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { handler()->mouseReleaseEvent(this, event); }
void Item::wheelEvent(QGraphicsSceneWheelEvent *event) { handler()->wheelEvent(this, event); }

void Item::hoverEnterEvent(QGraphicsSceneHoverEvent *event) { handler()->hoverEnterEvent(this, event); }
void Item::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { handler()->hoverLeaveEvent(this, event); }
void Item::hoverMoveEvent(QGraphicsSceneHoverEvent *event) { handler()->hoverMoveEvent(this, event); }

void Item::dragEnterEvent(QGraphicsSceneDragDropEvent *event) { handler()->dragEnterEvent(this, event); }
void Item::dragLeaveEvent(QGraphicsSceneDragDropEvent *event) { handler()->dragLeaveEvent(this, event); }
void Item::dragMoveEvent(QGraphicsSceneDragDropEvent *event) { handler()->dragMoveEvent(this, event); }
void Item::dropEvent(QGraphicsSceneDragDropEvent *event) { handler()->dropEvent(this, event); }

// Menu events
void Item::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) { handler()->contextMenuEvent(this, event); }

// Focus events
void Item::focusInEvent(QFocusEvent *event) { handler()->focusInEvent(this, event); }
void Item::focusOutEvent(QFocusEvent *event) { handler()->focusOutEvent(this, event); }

/***********************************************************************************************************************
 * Default Event handling methods. These methods can be called from the event handler and they will simply call the
 * default QGraphicsObject implementation.
 **********************************************************************************************************************/
//Keyboard events
void Item::defaultKeyPressEvent(QKeyEvent *event) { QGraphicsItem::keyPressEvent(event); }
void Item::defaultKeyReleaseEvent(QKeyEvent *event) { QGraphicsItem::keyReleaseEvent(event); }

// Mouse events
void Item::defaultMouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseDoubleClickEvent(event); }
void Item::defaultMouseMoveEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseMoveEvent(event); }
void Item::defaultMousePressEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mousePressEvent(event); }
void Item::defaultMouseReleaseEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseReleaseEvent(event); }
void Item::defaultWheelEvent(QGraphicsSceneWheelEvent *event) { QGraphicsItem::wheelEvent(event); }

void Item::defaultHoverEnterEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverEnterEvent(event); }
void Item::defaultHoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event); }
void Item::defaultHoverMoveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverMoveEvent(event); }

void Item::defaultDragEnterEvent(QGraphicsSceneDragDropEvent *event) { QGraphicsItem::dragEnterEvent(event); }
void Item::defaultDragLeaveEvent(QGraphicsSceneDragDropEvent *event) { QGraphicsItem::dragLeaveEvent(event); }
void Item::defaultDragMoveEvent(QGraphicsSceneDragDropEvent *event) { QGraphicsItem::dragMoveEvent(event); }
void Item::defaultDropEvent(QGraphicsSceneDragDropEvent *event) { QGraphicsItem::dropEvent(event); }

// Menu events
void Item::defaultContextMenuEvent(QGraphicsSceneContextMenuEvent *event) { QGraphicsItem::contextMenuEvent(event); }

// Focus events
void Item::defaultFocusInEvent(QFocusEvent *event) { QGraphicsItem::focusInEvent(event); }
void Item::defaultFocusOutEvent(QFocusEvent *event) { QGraphicsItem::focusOutEvent(event); }

}
