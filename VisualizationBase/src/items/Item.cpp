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

#include "items/Item.h"
#include "items/ItemStyle.h"
#include "layouts/SequentialLayout.h"
#include "shapes/Shape.h"
#include "shapes/ShapeStyle.h"
#include "VisualizationException.h"
#include "Scene.h"
#include "../renderer/ModelRenderer.h"
#include "VisualizationAddOn.h"

#include "cursor/Cursor.h"

namespace Visualization {

::Core::InitializationRegistry& itemTypeInitializationRegistry();
DEFINE_TYPE_ID_BASE(Item, itemTypeInitializationRegistry, "Item",)

InteractionHandler* Item::defaultClassHandler_ = InteractionHandler::instance();

int Item::registerVisualization()
{
	static QMutex mutex;
	static int i = 0;

	QMutexLocker locker(&mutex);
	return ++i;
}

QList<VisualizationAddOn*>& Item::staticAddOns()
{
	static QList<VisualizationAddOn*> addons;
	return addons;
}


void Item::addAddOn(VisualizationAddOn* addOn)
{
	if (!staticAddOns().contains(addOn))
		staticAddOns().append(addOn);
}

bool Item::removeAddOn(VisualizationAddOn* addOn)
{
	return staticAddOns().removeAll(addOn) != 0;
}

QMultiHash<Model::Node*, Item*>& Item::nodeItemsMap()
{
	static QMultiHash<Model::Node*, Item*> map;
	return map;
}

Item::Item(Item* parent, const StyleType* style) :
	QGraphicsItem(parent), style_(nullptr), shape_(nullptr), needsUpdate_(FullUpdate), purpose_(-1),
	itemCategory_(Scene::NoItemCategory)
{
	if ( !style || style->drawsOnlyShape() ) setFlag(QGraphicsItem::ItemHasNoContents);

	setFlag(QGraphicsItem::ItemIsFocusable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	setFlag(QGraphicsItem::ItemClipsToShape);
	setFlag(QGraphicsItem::ItemClipsChildrenToShape);

#ifdef QT_GRAPHICSVIEW_ENVISION_EXTENSIONS
	setFlag(QGraphicsItem::ItemDoesNotEnforceClip);
	setFlag(QGraphicsItem::ItemSkipsPaintingIfTooSmall);
#endif

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

void Item::setUpdateNeeded(UpdateType updateType)
{
	needsUpdate_ = updateType;
	Item* p = parent();
	while (p)
	{
		if (p->needsUpdate() == NoUpdate) p->needsUpdate_ = StandardUpdate;
		p = p->parent();
	}
}

void Item::setUpdateNeededForChildItem(UpdateType updateType, Model::Node* nodeVisualizedByChild)
{
	//TODO: Potentially quite expensive
	Item* c{};
	while (!c && nodeVisualizedByChild)
	{
		c = findVisualizationOf(nodeVisualizedByChild);
		nodeVisualizedByChild = nodeVisualizedByChild->parent();
	}
	if (c) c->setUpdateNeeded(updateType);
}

Item::UpdateType Item::needsUpdate()
{
	return needsUpdate_;
}

void Item::setPurpose(int purpose)
{
	purpose_ = purpose;
	setUpdateNeeded(FullUpdate);
}

void Item::clearPurpose()
{
	purpose_ = -1;
	setUpdateNeeded(FullUpdate);
}

void Item::setStyle(const ItemStyle* style)
{
	if (style == style_) return;
	SAFE_DELETE(shape_);
	style_ = style;
	useShape();
	setUpdateNeeded(FullUpdate);
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
		else
		{
			if (!style_ || style_->drawsOnlyShape() ) setFlag(QGraphicsItem::ItemHasNoContents);
		}
		setUpdateNeeded(StandardUpdate);
	}
}

void Item::removeShape()
{
	SAFE_DELETE(shape_);
	if (!style_ || style_->drawsOnlyShape()) setFlag(QGraphicsItem::ItemHasNoContents);
	setUpdateNeeded(StandardUpdate);
}

bool Item::sizeDependsOnParent() const
{
	return false;
}

void Item::updateSubtree()
{
	// It is important to update items regradless of their state when their size depends on the parent.
	// It is safe to assume that an item is updated before its parent item is updated. When an item's size depends on the
	// parent's size, we must first update the item without providing any size constraints, so that the item can report a
	// minimum size during the parent's update procedure.

	bool doUpdate = (needsUpdate_ != NoUpdate) || needsUpdate() || sizeDependsOnParent()
					|| (hasNode() && revision() != node()->revision());
	while (doUpdate)
	{
		updateAddOnItems();
		determineChildren();
		updateChildren();
		changeGeometry();
		if (needsUpdate_ == RepeatUpdate) needsUpdate_ = StandardUpdate;
		else
		{
			needsUpdate_ = NoUpdate;
			doUpdate = false;
			if (hasNode()) setRevision( node()->revision() );
		}
	}
}

void Item::changeGeometry(int availableWidth, int availableHeight)
{
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

void Item::updateAddOnItems()
{
	auto addons = addOns();

	for (auto key : addOnItems_.keys())
	{
		bool deleted = false;
		if (addons.contains(key)) addons.removeAll(key);
		else deleted = true;

		auto values = addOnItems_.values(key);
		decltype(values) new_values;
		if (!deleted) new_values = key->determineItems(this, values);

		for(auto item : values)
		{
			if (!deleted && new_values.contains( item )) new_values.removeAll(item);
			else
			{
				addOnItems_.remove(key, item);
				if (!item->parent()) SAFE_DELETE_ITEM(item);
			}
		}

		if (!deleted)
			for (auto v : new_values) addOnItems_.insert(key, v);
	}

	// Whatever is left in addons now is a new addon and must be processed
	for (auto a : addons)
		for (auto i : a->determineItems(this, QList<Item*>()))
			addOnItems_.insert(a,i);
}

void Item::updateGeometry(Item* content, int availableWidth, int availableHeight)
{
	if (hasShape())
	{
		getShape()->setOffset(0, 0);

		if ( (sizeDependsOnParent() || content->sizeDependsOnParent()) && (availableWidth > 0 || availableHeight > 0) )
		{
			QSize inner = getShape()->innerSize(availableWidth, availableHeight);

			if (content->sizeDependsOnParent() && (availableWidth > 0 || availableHeight > 0))
				content->changeGeometry(inner.width(), inner.height());

			if (content->width() > inner.width() ) inner.setWidth( content->width() );
			if (content->height() > inner.height() ) inner.setHeight( content->height() );
			getShape()->setInnerSize(inner.width(), inner.height());
		}
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

bool Item::isEmpty() const
{
	return false;
}

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if ( hasShape() && (style()->drawShapeWhenEmpty() || !isEmpty()) && !isCategoryHiddenDuringPaint() )
		shape_->paint(painter, option, widget);
}

InteractionHandler* Item::handler() const
{
	return defaultClassHandler();
}

void Item::execute(const QString& command)
{
	handler()->command(this, command);
}


Scene* Item::scene() const
{
	return static_cast<Visualization::Scene*> (QGraphicsItem::scene());
}

bool Item::hasNode() const
{
	return false;
}

Model::Node* Item::node() const
{
	return nullptr;
}

int Item::revision() const
{
	return -1;
}

void Item::setRevision(int)
{
}

bool Item::itemOrChildHasFocus() const
{
	return QGraphicsItem::scene()->focusItem() == this
			|| QGraphicsItem::isAncestorOf( QGraphicsItem::scene()->focusItem() );
}

Item* Item::focusedChild() const
{
	for(auto item : childItems())
	{
		Item* child = static_cast<Item*> (item);
		if (child->itemOrChildHasFocus()) return child;
	}

	return nullptr;
}

void Item::removeFromScene()
{
	if ( scene() )
	{
		// Remove cursor if it is in the hierarchy
		auto mc = scene()->mainCursor();
		if (mc && isAncestorOf(mc->owner()))
			scene()->setMainCursor(nullptr);

		if (parent()) scene()->removeItem(this);
		else scene()->removeTopLevelItem(this);
	}
	setParentItem(nullptr);
}

void Item::synchronizeItem(Item*& item, Model::Node* node)
{
	if (item && item->node() != node )
	{
		SAFE_DELETE_ITEM(item);
		setUpdateNeeded(StandardUpdate);
	}

	if (!item && node)
	{
		item = renderer()->render(this, node);
		setUpdateNeeded(StandardUpdate);
	}
}

ModelRenderer* Item::renderer()
{
	if ( (static_cast<Scene*>(scene()))->renderer() ) return (static_cast<Scene*>(scene()))->renderer();
	throw VisualizationException("The scene of an Item has no renderer.");
}

int Item::distanceTo(const QPoint& p) const
{
	if (p.y() < 0)
	{
		// Above
		if (p.x() < 0) return std::sqrt(p.y()*p.y() + p.x()*p.x()); // To the left
		else if (p.x() > width()) return  std::sqrt(p.y()*p.y() + (p.x()-width())*(p.x()-width())); // To the right
		else return -p.y(); // Directly above
	}
	else if (p.y() > height())
	{
		// Below
		if (p.x() < 0) return std::sqrt((p.y()-height())*(p.y()-height()) + p.x()*p.x()); // To the left
		else if (p.x() > width())
			return std::sqrt((p.y()-height())*(p.y()-height()) + (p.x()-width())*(p.x()-width())); // To the right
		else return p.y()-height(); // Directly below
	}
	else
	{
		// Within the same height
		if (p.x() < 0) return -p.x(); // To the left
		else if (p.x() > width()) return  p.x()-width(); // To the right
		else return 0; // Inside
	}
}

Item* Item::childClosestTo(const QPoint& point, PositionConstraints childConstraint) const
{
	Item* closest = nullptr;
	int closest_distance = 0;

	for(auto item : childItems())
	{
		Item* child = static_cast<Item*> (item);
		QPoint childCoordinates = mapToItem(child, point).toPoint();

		if ( (child->satisfiedPositionConstraints(childCoordinates) & childConstraint) == childConstraint)
		{
			int distance = child->distanceTo(childCoordinates);
			if (closest == nullptr || closest_distance > distance)
			{
				closest = child;
				closest_distance = distance;
			}
		}
	}

	return closest;
}

Item::PositionConstraints Item::satisfiedPositionConstraints(const QPoint& p) const
{
	PositionConstraints constraints = NoConstraints;

	if ( p.y() < height() - 1) constraints |= Below;
	if ( p.y() > 0) constraints |= Above;

	if ( p.x() < width() - 1) constraints |= RightOf;
	if ( p.x() > 0) constraints |= LeftOf;

	if ( p.y() >= 0 && p.y() < height() &&  p.x() >= 0 && p.x() < width())
		constraints |= Overlap;

	return constraints;
}

QList<ItemRegion> Item::regions()
{
	QList<ItemRegion> regs;

	bool hasChildren = false;

	if (!style()->wholeItemCursor())
	{
		for(auto item : childItems())
		{
			hasChildren = true;
			Item* child = static_cast<Item*> (item);
			QRect rect = child->boundingRect().toRect();
			rect.translate(child->pos().toPoint());
			regs.append(ItemRegion(rect));
			regs.last().setItem(child);
		}
	}

	if (!hasChildren)
	{
		regs.append(ItemRegion(boundingRect_.toRect()));

		Cursor* cur = new Cursor(this, Cursor::BoxCursor);
		cur->setRegion( boundingRect_.toRect() );
		cur->setPosition( boundingRect_.center().toPoint() );
		regs.last().setCursor(cur);
	}

	return regs;
}

bool Item::moveCursor(CursorMoveDirection dir, QPoint reference)
{
	// The condition below is only true if this method is called from a parent item, after it was already called for the
	// item itself. In that case simply return false as the original call returned false too.
	if (scene()->mainCursor() && scene()->mainCursor()->owner() == this &&
			(dir == MoveUpOf || dir == MoveDownOf || dir == MoveLeftOf || dir == MoveRightOf) )
	return false;

	QList<ItemRegion> regs = regions();
	if (regs.isEmpty()) return false;

	if (dir == MoveDefault)
	{
		if (defaultMoveCursorProxy_)
		{
			defaultMoveCursorProxy_->moveCursor(MoveDefault);
			return true;
		}
		reference = QPoint(0,0);
	}

	ItemRegion* current = nullptr;
	// Handle cursor movement in a specific direction
	if (hasFocus() && (dir == MoveUp || dir == MoveDown || dir == MoveLeft || dir == MoveRight) )
	{
		auto c = correspondingSceneCursor<Cursor>();
		// Find which region corresponds to the current cursor and set the source point accordingly
		for (ItemRegion& r : regs)
		{
			if (c->isSame(r.cursor()))
			{
				current = &r;
				switch(dir)
				{
					case MoveUp: reference = QPoint(reference.x(), r.region().y()); break;
					case MoveDown: reference = QPoint(reference.x(), r.region().y() + r.region().height()); break;
					case MoveLeft: reference = QPoint(r.region().x(), reference.y()); break;
					case MoveRight: reference = QPoint(r.region().x() + r.region().width(), reference.y()); break;
					default: /* Will never be the case because of the top-level if statement */ break;
				}
				break;
			}
		}
	}

	// Find all regions that match the constraints
	ItemRegion::PositionConstraint constraints = ItemRegion::NoConstraints;
	switch(dir)
	{
		case MoveUp: constraints = ItemRegion::Above; break;
		case MoveDown: constraints = ItemRegion::Below; break;
		case MoveLeft: constraints = ItemRegion::LeftOf; break;
		case MoveRight: constraints = ItemRegion::RightOf; break;
		case MoveOnPosition: constraints = ItemRegion::NoConstraints; break;
		case MoveUpOf: constraints = ItemRegion::Above; break;
		case MoveDownOf: constraints = ItemRegion::Below; break;
		case MoveLeftOf: constraints = ItemRegion::LeftOf; break;
		case MoveRightOf: constraints = ItemRegion::RightOf; break;
		case MoveDefault: constraints = ItemRegion::NoConstraints; break;
	}

	// We use a map since the elements are kept ordered according to the key. As key we use the distance multiplied by 10
	// so that we can implement minor corrections in order.
	QMap<int, ItemRegion*> matching;

	for (ItemRegion& r : regs)
	{
		ItemRegion::PositionConstraints satisfied = r.satisfiedPositionConstraints(reference);
		if (	(&r != current)
				&& ( (( satisfied & constraints) == constraints)
						// An overlapping cursor region is preferred, even if it does not satisfy the constraints.
						// This happens in 'tight' layouts that do not leave any spacing between the elements and so the
						// Cursors are overlapping the normal items. At the borders of such containers the alternative below
						// is needed.
						|| (r.cursor() && (satisfied & ItemRegion::Overlap)) )
			)
		{
			int distanceKey = r.distanceTo(reference)*10;
			if (r.cursor()) distanceKey -= 5;
			matching.insert(distanceKey, &r);
		}
	}

	bool canFocus = false;
	Cursor* focusedCursor = nullptr;

	for(auto r : matching.values())
	{
		// Try to focus
		if (r->cursor())
		{
			// This is a cursor region for a cursor belonging to the current item.
			scene()->setMainCursor(r->cursor());
			setFocus();
			canFocus = true;
			focusedCursor = r->cursor();
			break;
		}
		else if (r->item())
		{
			// This is a child item region

			CursorMoveDirection childDirection;
			switch(dir)
			{
				case MoveUp: childDirection = MoveUpOf; break;
				case MoveDown: childDirection = MoveDownOf; break;
				case MoveLeft: childDirection = MoveLeftOf; break;
				case MoveRight: childDirection = MoveRightOf; break;
				case MoveOnPosition: childDirection = MoveOnPosition; break;
				case MoveUpOf: childDirection = MoveUpOf; break;
				case MoveDownOf: childDirection = MoveDownOf; break;
				case MoveLeftOf: childDirection = MoveLeftOf; break;
				case MoveRightOf: childDirection = MoveRightOf; break;
				case MoveDefault: childDirection = MoveDefault; break;
				default: throw VisualizationException("Unknown move direction: " + QString::number(dir));
			}
			if( r->item()->moveCursor(childDirection, mapToItem(r->item(), reference).toPoint()))
			{
				canFocus = true;
				break;
			}
		}
		else
		{
			throw VisualizationException("Encountered an item region with no cursor or child item.");
		}
	}

	// Delete the cursors of all regions which have one except for the focused one.
	for (ItemRegion& r : regs)
		if (r.cursor() && r.cursor() != focusedCursor) delete r.cursor();

	return canFocus;
}

Item* Item::findVisualizationOf(Model::Node* node)
{
	if (this->node() == node) return this;

	auto it = nodeItemsMap().find(node);
	auto end = Item::nodeItemsMap().end();
	while (it != end && it.key() == node)
	{
		auto item = it.value();
		if (isAncestorOf(item)) return item;
		++it;
	}

	return nullptr;
}

bool Item::sceneEvent(QEvent *event)
{
	if (event->type() == QEvent::KeyPress)
	{
		keyPressEvent(static_cast<QKeyEvent *>(event));
		return true;
	}
	return QGraphicsItem::sceneEvent(event);
}

int Item::purpose() const
{
	if (purpose_ >= 0) return purpose_;
	if (!parent()) return -1;

	if ( node() ) return parent()->childNodePurpose( node() );
	else return parent()->purpose();
}


int Item::childNodePurpose(const Model::Node* node) const
{
	auto c = childNodePurpose_.find(node);
	if (c != childNodePurpose_.end()) return *c;

	return purpose();
}

void Item::setChildNodePurpose(const Model::Node* node, int purpose)
{
	childNodePurpose_.insert(node, purpose);
	setUpdateNeeded(FullUpdate);
}

void Item::clearChildNodePurpose(const Model::Node* node)
{
	childNodePurpose_.remove(node);
	setUpdateNeeded(FullUpdate);
}

bool Item::definesChildNodePurpose(const Model::Node* node) const
{
	return childNodePurpose_.find(node) != childNodePurpose_.end();
}

QList<VisualizationAddOn*> Item::addOns()
{
	return staticAddOns();
}

void Item::putAddOnItemsInSequence(SequentialLayout* layout)
{
	auto values = addOnItems_.values();
	for (int i = layout->length() - 1; i>=0; --i)
	{
		auto item = layout->at<Item>(i);
		if (values.contains(item)) values.removeAll(item);
		else layout->removeAll(item,true);
	}

	for (auto item : values)
		layout->append(item);
}

Scene::ItemCategory Item::itemCategory()
{
	if (itemCategory_ == Scene::NoItemCategory && parent()) return parent()->itemCategory();
	else return itemCategory_;
}

void Item::setWidth(int width)
{
	if (width != this->width())
	{
		prepareGeometryChange();
		boundingRect_.setWidth(width);
	}
}
void Item::setHeight(int height)
{
	if (height != this->height())
	{
		prepareGeometryChange();
		boundingRect_.setHeight(height);
	}
}
void Item::setSize(int width, int height)
{
	if (width != this->width() || height != this->height())
	{
		prepareGeometryChange();
		boundingRect_.setSize(QSizeF(width, height));
	}
}
void Item::setSize(const QSizeF& size)
{
	if (size != this->size())
	{
		prepareGeometryChange();
		boundingRect_.setSize(size);
	}
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
void Item::defaultMouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseDoubleClickEvent(event);}
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
