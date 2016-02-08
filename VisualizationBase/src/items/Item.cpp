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

#include "Item.h"
#include "ItemStyle.h"
#include "../layouts/SequentialLayout.h"
#include "../shapes/Shape.h"
#include "../shapes/ShapeStyle.h"
#include "../VisualizationException.h"
#include "../Scene.h"
#include "../renderer/ModelRenderer.h"
#include "../views/MainView.h"
#include "VisualizationAddOn.h"
#include "../ViewItemManager.h"

#include "../cursor/Cursor.h"

namespace Visualization {

static constexpr int MAX_CURSOR_JUMP_DISTANCE = 300;

DEFINE_TYPE_ID_BASE(Item, "Item", )

InteractionHandler* Item::defaultClassHandler_ = InteractionHandler::instance();

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
	QGraphicsItem(parent), style_{nullptr}, shape_{nullptr}, needsUpdate_{StandardUpdate}, purpose_{-1},
	semanticZoomLevel_{-1}, itemCategory_{Scene::NoItemCategory}
{
	// By default no flags in a QGraphicsItem are enabled.
	GraphicsItemFlags flags;

	if ( !style || style->drawsOnlyShape() ) flags |= ItemHasNoContents;

	flags	|=	ItemIsFocusable
			| ItemIsSelectable;

#if QT_VERSION >= 0x050400
	flags |= ItemContainsChildrenInShape;
#endif

	// This is an expensive operation, so only do it once.
	setFlags(flags);

	setStyle(style);
	setZValue(LAYER_DEFAULT_Z);
}

Item::~Item()
{
	// Mark this item as not needing updates
	for (auto s : Scene::allScenes())
		s->setItemIsSensitiveToScale(this, false);

	SAFE_DELETE(shape_);
}

QRectF Item::boundingRect() const
{
	return boundingRect_;
}

void Item::setUpdateNeeded(UpdateType updateType)
{
	// Set own update
	needsUpdate_ = updateType;

	// Request that all parents are updated
	Item* p = parent();
	while (p)
	{
		if (p->needsUpdate() == NoUpdate) p->needsUpdate_ = StandardUpdate;
		p = p->parent();
	}

	// If Full update, update all children
	if (updateType == FullUpdate)
	{
		QList<Item*> stack = childItems();
		while (!stack.isEmpty())
		{
			auto child = stack.takeLast();
			child->needsUpdate_ = StandardUpdate;
			stack << child->childItems();
		}
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

void Item::setSemanticZoomLevel(int semanticZoomLevel)
{
	semanticZoomLevel_ = semanticZoomLevel;
	setUpdateNeeded(FullUpdate);
}

void Item::clearSemanticZoomLevel()
{
	semanticZoomLevel_ = -1;
	setUpdateNeeded(FullUpdate);
}

void Item::setScale(qreal newScale)
{
	if (scale() != newScale)
	{
		QGraphicsItem::setScale(newScale);

		if (isSensitiveToScale()) setUpdateNeeded(StandardUpdate);
		else
		{
			QList<Item*> stack = childItems();
			while (!stack.empty())
			{
				Item* last = stack.takeLast();
				if (last->isSensitiveToScale()) last->setUpdateNeeded(StandardUpdate);
				else stack << last->childItems();
			}
		}
	}
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
	if (shape_)
	{
		SAFE_DELETE(shape_);
		if (!style_ || style_->drawsOnlyShape()) setFlag(QGraphicsItem::ItemHasNoContents);
		setUpdateNeeded(StandardUpdate);
	}
}

QColor Item::customShapeColor() const
{
	return {};
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
		for (auto child : childItems()) child->updateSubtree();
		changeGeometry();
		if (needsUpdate_ == RepeatUpdate) needsUpdate_ = StandardUpdate;
		else
		{
			needsUpdate_ = NoUpdate;
			doUpdate = false;
			if (hasNode()) setRevision( node()->revision() );
		}
	}

	if (isSensitiveToScale())
		scene()->setItemIsSensitiveToScale(this, true);
}

bool Item::isSensitiveToScale() const
{
	return false;
}

void Item::changeGeometry(int availableWidth, int availableHeight)
{
	updateGeometry(fromParent(availableWidth), fromParent(availableHeight));
	update();
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

		for (auto item : values)
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
			addOnItems_.insert(a, i);
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

			if (content->widthInParent() > inner.width() ) inner.setWidth( content->widthInParent() );
			if (content->heightInParent() > inner.height() ) inner.setHeight( content->heightInParent() );
			getShape()->setInnerSize(inner.width(), inner.height());
		}
		else getShape()->setInnerSize(content->widthInParent(), content->heightInParent());

		content->setPos(getShape()->contentLeft(), getShape()->contentTop());
	}
	else
	{
		if (content->sizeDependsOnParent() && (availableWidth > 0 || availableHeight > 0))
			content->changeGeometry(availableWidth, availableHeight);
		content->setPos(0, 0);
		setSize(content->sizeInParent());
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

void Item::execute(const QString& command, const std::unique_ptr<Cursor>& cursor)
{
	handler()->command(this, command, cursor);
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

Item* Item::findAncestorWithNode()
{
	auto result = this;
	while (result && !result->hasNode())
		result = result->parent();
	return result;
}

bool Item::itemOrChildHasFocus() const
{
	return QGraphicsItem::scene()->focusItem() == this
			|| QGraphicsItem::isAncestorOf( QGraphicsItem::scene()->focusItem() );
}

Item* Item::focusedChild() const
{
	for (auto child : childItems())
		if (child->itemOrChildHasFocus()) return child;

	return nullptr;
}

void Item::removeFromScene()
{
	if ( scene() )
	{
		// Remove cursor if it is in the hierarchy
		auto mc = scene()->mainCursor();
		if (mc && (mc->owner() == this || isAncestorOf(mc->owner())))
			scene()->setMainCursor(nullptr);

		// Remove associated overlays
		scene()->removeOverlayOf(this);

		// Mark this item as not needing updates
		scene()->setItemIsSensitiveToScale(this, false);

		//Notify the view manager about removal of the item
		scene()->viewItems()->cleanupRemovedItem(this);

		//Finally remove this item from the scene
		if (parent()) scene()->removeItem(this);
		else scene()->removeTopLevelItem(this);
	}
	setParentItem(nullptr);
}

ModelRenderer* Item::renderer()
{
	if ( (static_cast<Scene*>(scene()))->renderer() ) return (static_cast<Scene*>(scene()))->renderer();
	throw VisualizationException{"The scene of an Item has no renderer."};
}

int Item::distanceTo(const QPoint& point) const
{
	if (point.y() < 0)
	{
		// Above
		if (point.x() < 0) return std::sqrt(point.y()*point.y() + point.x()*point.x()); // To the left
		else if (point.x() > widthInLocal())
			return std::sqrt(point.y()*point.y() + (point.x()-widthInLocal())*(point.x()-widthInLocal())); // To the right
		else return -point.y(); // Directly above
	}
	else if (point.y() > heightInLocal())
	{
		// Below
		if (point.x() < 0)
			return std::sqrt((point.y()-heightInLocal())*(point.y()-heightInLocal()) + point.x()*point.x()); // To the left
		else if (point.x() > widthInLocal())
			return std::sqrt((point.y()-heightInLocal())*(point.y()-heightInLocal())
								  + (point.x()-widthInLocal())*(point.x()-widthInLocal())); // To the right
		else return point.y()-heightInLocal(); // Directly below
	}
	else
	{
		// Within the same height
		if (point.x() < 0) return -point.x(); // To the left
		else if (point.x() > widthInLocal()) return point.x()-widthInLocal(); // To the right
		else return 0; // Inside
	}
}

Item* Item::childClosestTo(const QPoint& point, PositionConstraints childConstraint) const
{
	Item* closest = nullptr;
	int closest_distance = 0;

	for (auto child : childItems())
	{
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

Item::PositionConstraints Item::satisfiedPositionConstraints(const QPoint& point) const
{
	PositionConstraints constraints = NoConstraints;

	if ( point.y() < heightInLocal() - 1) constraints |= Below;
	if ( point.y() > 0) constraints |= Above;

	if ( point.x() < widthInLocal() - 1) constraints |= RightOf;
	if ( point.x() > 0) constraints |= LeftOf;

	if ( point.y() >= 0 && point.y() < heightInLocal() &&  point.x() >= 0 && point.x() < widthInLocal())
		constraints |= Overlap;

	return constraints;
}

QList<ItemRegion> Item::regions()
{
	QList<ItemRegion> regs;

	if (!style()->noItemRegions())
	{
		bool hasChildren = false;

		if (!style()->wholeItemCursor())
		{
			for (auto child : childItems())
			{
				hasChildren = true;
				QRect rect = QRect{QPoint{0, 0}, child->sizeInParent().toSize()};
				rect.translate(child->pos().toPoint());
				regs.append(ItemRegion{rect});
				regs.last().setItem(child);
			}
		}

		if (!hasChildren)
		{
			regs.append(ItemRegion{boundingRect_.toRect()});

			Cursor* cur = new Cursor{this, Cursor::BoxCursor};
			cur->setRegion( boundingRect_.toRect() );
			cur->setPosition( boundingRect_.center().toPoint() );
			regs.last().setCursor(cur);
		}
	}

	return regs;
}

bool Item::moveCursor(CursorMoveDirection dir, QPoint reference, CursorMoveOptions options)
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
		reference = QPoint{0, 0};
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
				switch (dir)
				{
					case MoveUp: reference = QPoint{reference.x(), r.region().y()}; break;
					case MoveDown: reference = QPoint{reference.x(), r.region().y() + r.region().height()}; break;
					case MoveLeft: reference = QPoint{r.region().x(), reference.y()}; break;
					case MoveRight: reference = QPoint{r.region().x() + r.region().width(), reference.y()}; break;
					default: Q_ASSERT(false); /* Will never be the case because of the top-level if statement */ break;
				}
				break;
			}
		}
	}

	// Find all regions that match the constraints
	// Also adjust the reference where necessary
	ItemRegion::PositionConstraint constraints = ItemRegion::NoConstraints;
	auto xEnd = widthInLocal() - 1;
	auto yEnd = heightInLocal() - 1;
	auto xMid = widthInLocal()/2;
	auto yMid = heightInLocal()/2;
	switch (dir)
	{
		case MoveUp: constraints = ItemRegion::Above; break;
		case MoveDown: constraints = ItemRegion::Below; break;
		case MoveLeft: constraints = ItemRegion::LeftOf; break;
		case MoveRight: constraints = ItemRegion::RightOf; break;

		case MoveOnPosition: constraints = ItemRegion::NoConstraints; break;
		case MoveOnTop: constraints = ItemRegion::NoConstraints; reference = {xMid, 0}; break;
		case MoveOnLeft: constraints = ItemRegion::NoConstraints; reference = {0, yMid}; break;
		case MoveOnBottom: constraints = ItemRegion::NoConstraints; reference = {xMid, yEnd}; break;
		case MoveOnRight: constraints = ItemRegion::NoConstraints; reference = {xEnd, yMid}; break;
		case MoveOnTopLeft: constraints = ItemRegion::NoConstraints; reference = {0, 0}; break;
		case MoveOnBottomRight: constraints = ItemRegion::NoConstraints; reference = {xEnd, yEnd}; break;
		case MoveOnCenter: constraints = ItemRegion::NoConstraints; reference = {xMid, yMid}; break;

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

	bool limitCursorJumpDistance = false;
	auto currentCursor = scene()->mainCursor();
	QPointF currentCursorPos;

	if (!options.testFlag(NoLimitDistance) &&
		 currentCursor && currentCursor->owner() != this && !currentCursor->owner()->isAncestorOf(this))
	{
		limitCursorJumpDistance = true;
		currentCursorPos = currentCursor->owner()->mapToItem(this, currentCursor->region().center());
	}

	for (auto r : matching.values())
	{
		// Ignore cursors which are very far away from the current cursor
		if (limitCursorJumpDistance && r->distanceTo(currentCursorPos) > MAX_CURSOR_JUMP_DISTANCE) continue;

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
			switch (dir)
			{
				case MoveUp: childDirection = MoveUpOf; break;
				case MoveDown: childDirection = MoveDownOf; break;
				case MoveLeft: childDirection = MoveLeftOf; break;
				case MoveRight: childDirection = MoveRightOf; break;

				case MoveOnPosition: childDirection = MoveOnPosition; break;
				case MoveOnTop: childDirection = MoveOnTop; break;
				case MoveOnLeft: childDirection = MoveOnLeft; break;
				case MoveOnBottom: childDirection = MoveOnBottom; break;
				case MoveOnRight: childDirection = MoveOnRight; break;
				case MoveOnTopLeft: childDirection = MoveOnTopLeft; break;
				case MoveOnBottomRight: childDirection = MoveOnBottomRight; break;
				case MoveOnCenter: childDirection = MoveOnCenter; break;

				case MoveUpOf: childDirection = MoveUpOf; break;
				case MoveDownOf: childDirection = MoveDownOf; break;
				case MoveLeftOf: childDirection = MoveLeftOf; break;
				case MoveRightOf: childDirection = MoveRightOf; break;
				case MoveDefault: childDirection = MoveDefault; break;
				default: throw VisualizationException{"Unknown move direction: " + QString::number(dir)};
			}
			if ( r->item()->moveCursor(childDirection, mapToItem(r->item(), reference).toPoint()))
			{
				canFocus = true;
				break;
			}
		}
		else
		{
			throw VisualizationException{"Encountered an item region with no cursor or child item."};
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
	auto allVis = findAllVisualizationsOf(node);
	if (allVis.size() > 0) return allVis[0];
	else return nullptr;
}

QList<Item*> Item::findAllVisualizationsOf(Model::Node* node)
{
	QList<Item*> result;
	if (this->node() == node) result.append(this);

	auto it = nodeItemsMap().find(node);
	auto end = Item::nodeItemsMap().end();
	while (it != end && it.key() == node)
	{
		auto item = it.value();
		if (isAncestorOf(item)) result.append(item);
		++it;
	}

	return result;
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

	return std::max(0, purpose());
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

int Item::semanticZoomLevel() const
{
	if (semanticZoomLevel_ >= 0) return semanticZoomLevel_;
	if (!parent()) return -1;

	if ( node() ) return parent()->childNodeSemanticZoomLevel( node() );
	else return parent()->semanticZoomLevel();
}


int Item::childNodeSemanticZoomLevel(const Model::Node* node) const
{
	auto c = childNodeSemanticZoomLevel_.find(node);
	if (c != childNodeSemanticZoomLevel_.end()) return *c;

	return std::max(0, semanticZoomLevel());
}

void Item::setChildNodeSemanticZoomLevel(const Model::Node* node, int semanticZoomLevel)
{
	childNodeSemanticZoomLevel_.insert(node, semanticZoomLevel);
	setUpdateNeeded(StandardUpdate);
}

void Item::clearChildNodeSemanticZoomLevel(const Model::Node* node)
{
	childNodeSemanticZoomLevel_.remove(node);
	setUpdateNeeded(StandardUpdate);
}

bool Item::definesChildNodeSemanticZoomLevel(const Model::Node* node) const
{
	return childNodeSemanticZoomLevel_.find(node) != childNodeSemanticZoomLevel_.end();
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
		else layout->removeAll(item, true);
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
	if (width != this->widthInLocal())
	{
		prepareGeometryChange();
		boundingRect_.setWidth(width);
	}
}
void Item::setHeight(int height)
{
	if (height != this->heightInLocal())
	{
		prepareGeometryChange();
		boundingRect_.setHeight(height);
	}
}
void Item::setSize(int width, int height)
{
	if (width != this->widthInLocal() || height != this->heightInLocal())
	{
		prepareGeometryChange();
		boundingRect_.setSize(QSizeF{(qreal) width, (qreal) height});
	}
}
void Item::setSize(const QSizeF& size)
{
	if (size != this->sizeInLocal())
	{
		prepareGeometryChange();
		boundingRect_.setSize(size);
	}
}

QList<Item*> Item::childItems() const
{
	// TODO: This is rather ugly and dangerous. Is there a better way to achieve this without a performance penalty?
	auto && children = QGraphicsItem::childItems();
	auto && ch = reinterpret_cast<QList<Item*>*>(&children);
	return *ch;
}

qreal Item::mainViewScalingFactor() const
{
	if (auto s = scene()) return s->mainViewScalingFactor();
	else return 1;
}

qreal Item::previousMainViewScalingFactor() const
{
	if (auto s = scene()) return s->previousMainViewScalingFactor();
	else return 1;
}

qreal Item::totalScale() const
{
	auto s = scale();
	auto p = parent();
	while (p)
	{
		s *= p->scale();
		p = p->parent();
	}

	return s;
}

QList<OverlayAccessor*> Item::overlays(QString overlayGroup) const
{
	QList<OverlayAccessor*> result;

	if (!overlayGroup.isEmpty())
	{
		auto group = scene()->overlayGroup(overlayGroup);
		if (group) result << group->overlaysForItem(this);
	}
	else
	{
		for (auto group : scene()->allOverlayGroups())
			result << result << group->overlaysForItem(this);
	}

	return result;
}

bool Item::ignoresCopyAndPaste()
{
	return false;
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
