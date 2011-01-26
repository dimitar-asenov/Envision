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
#include "Scene.h"

namespace Visualization {

Item::Item(Item* parent, const ItemStyle* style) :
	QGraphicsItem(parent), style_(NULL), shape_(NULL), needsUpdate_(true)
{
	setFlag(QGraphicsItem::ItemHasNoContents);
	setFlag(QGraphicsItem::ItemIsFocusable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	setStyle(style);
	setZValue(LAYER_DEFAULT_Z);
}

Item::~Item()
{
	SAFE_DELETE(shape_);
}

QRectF Item::boundingRect() const
{
	return boundingRect_;
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

bool Item::needsUpdate()
{
	return needsUpdate_;
}

void Item::setStyle(const ItemStyle* style)
{
	SAFE_DELETE(shape_);
	style_ = style;
	useShape();
	setUpdateNeeded();
}

void Item::useShape()
{
	if (!shape_)
	{
		if (style_ && style_->hasShape())
		{
			shape_ = style_->createShape(this);
			setFlag(QGraphicsItem::ItemHasNoContents, false);
		}
		else setFlag(QGraphicsItem::ItemHasNoContents);
		setUpdateNeeded();
	}
}

void Item::removeShape()
{
	SAFE_DELETE(shape_);
	setFlag(QGraphicsItem::ItemHasNoContents);
	setUpdateNeeded();
}

bool Item::sizeDependsOnParent() const
{
	return false;
}

void Item::updateSubtree()
{
	if ( needsUpdate_ || needsUpdate())
	{
		determineChildren();
		updateChildren();
		changeGeometry();
		needsUpdate_ = false;
	}
}

void Item::changeGeometry(int availableWidth, int availableHeight)
{
	prepareGeometryChange();
	updateGeometry(availableWidth, availableHeight);
	update();
}

void Item::updateChildren()
{
	QList<QGraphicsItem *> children = childItems();
	for (QList<QGraphicsItem *>::iterator child = children.begin(); child != children.end(); ++child)
	{
		Item* item = static_cast<Item*> (*child);
		item->updateSubtree();
	}
}

void Item::updateGeometry(Item* content, int availableWidth, int availableHeight)
{
	if (hasShape())
	{
		int width = getShape()->getInnerWidth(availableWidth);
		int height = getShape()->getInnerHeight(availableHeight);

		if (content->sizeDependsOnParent() && (availableWidth > 0 || availableHeight > 0))
			content->changeGeometry(width, height);

		if (content->width() > width ) width = content->width();
		if (content->height() > height ) height = content->height();

		getShape()->setOffset(0, 0);

		if ( sizeDependsOnParent() ) getShape()->setInnerSize(width, height);
		else getShape()->setInnerSize(content->width(), content->height());

		content->setPos(getShape()->contentLeft(), getShape()->contentTop());
	}
	else
	{
		if (content->sizeDependsOnParent() && (availableWidth > 0 || availableHeight > 0))
			content->changeGeometry(availableWidth, availableHeight);
		content->setPos(0,0);
		setSize(content->size());
	}
}

bool Item::focusChild(FocusTarget)
{
	return false;
}

void Item::focusChild(Item* child, FocusTarget location)
{
	if (child)
	{
		if (child->flags() & QGraphicsItem::ItemIsFocusable)
		{
			child->scene()->clearSelection();
			child->setSelected(true);
			child->setFocus();
			child->setUpdateNeeded();
		}
		else child->focusChild(location);
	}
}

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ( shape_ ) shape_->paint(painter, option, widget);
}

InteractionHandler* Item::handler() const
{
	return InteractionHandler::instance();
}

void Item::execute(const QString& command)
{
	handler()->command(this, command);
}


Scene* Item::scene() const
{
	return static_cast<Visualization::Scene*> (QGraphicsItem::scene());
}

bool Item::childHasFocus() const
{
	return QGraphicsItem::scene()->focusItem() == this || QGraphicsItem::isAncestorOf( QGraphicsItem::scene()->focusItem() );
}

void Item::removeFromScene()
{
	if ( scene() ) scene()->removeItem(this);
	setParentItem(NULL);
}

/***********************************************************************************************************************
 * Reimplemented Event handling methods. These simply dispatch the method call to the interaction handler of this
 * object.
 **********************************************************************************************************************/
// Keyboard events
void Item::keyPressEvent(QKeyEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->keyPressEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::keyReleaseEvent(QKeyEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->keyReleaseEvent(this, event);
	handler()->afterEvent(this, event);
}

// Mouse events
void Item::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->mouseDoubleClickEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->mouseMoveEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->mousePressEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->mouseReleaseEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->wheelEvent(this, event);
	handler()->afterEvent(this, event);
}

void Item::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->hoverEnterEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->hoverLeaveEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->hoverMoveEvent(this, event);
	handler()->afterEvent(this, event);
}

void Item::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->dragEnterEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->dragLeaveEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->dragMoveEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->dropEvent(this, event);
	handler()->afterEvent(this, event);
}

// Menu events
void Item::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->contextMenuEvent(this, event);
	handler()->afterEvent(this, event);
}

// Focus events
void Item::focusInEvent(QFocusEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->focusInEvent(this, event);
	handler()->afterEvent(this, event);
}
void Item::focusOutEvent(QFocusEvent *event)
{
	handler()->beforeEvent(this, event);
	handler()->focusOutEvent(this, event);
	handler()->afterEvent(this, event);
}

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
