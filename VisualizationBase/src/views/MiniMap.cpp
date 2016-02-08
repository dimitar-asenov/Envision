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

#include "MiniMap.h"
#include "../items/Item.h"

#include "Logger/src/Timer.h"

namespace Visualization {

MiniMap::MiniMap(Scene* scene, View* parent) : View{scene, parent}, parent_{parent}, margin_(DEFAULT_MARGIN)
{
	setInteractive(false);
	updatePosition();
	updateMap();
	visibleRectChanged();
	setRenderHint(QPainter::Antialiasing);

	setHiddenItemCategories(Scene::MenuItemCategory | Scene::CursorItemCategory);

	connect(scene, &Scene::sceneRectChanged, this, &MiniMap::sceneRectChanged);
}

void MiniMap::updatePosition()
{
	QGraphicsView* parent = static_cast<QGraphicsView*> (parentWidget());

	move(0, parent->viewport()->height() - height() + 2*frameWidth());
}

void MiniMap::resizeEvent( QResizeEvent *event )
{
	View::resizeEvent(event);
	updatePosition();
}

void MiniMap::sceneRectChanged(const QRectF & /*rect*/)
{
	updateMap();
}

void MiniMap::visibleRectChanged()
{
	visibleRect = parent_->visibleRect();
	updateMap();
}

void MiniMap::paintEvent(QPaintEvent *event)
{
	auto t = Logger::Timer::start("Minimap paint");
	View::paintEvent(event);

	// Paint orientation rectangle and circle
	QPainter painter(viewport());
	painter.setPen(Qt::red);
	painter.drawRect(drawnRect);
	if (drawnRect.width() <= 2 || drawnRect.height() <= 2)
		painter.drawEllipse(drawnRect.center(), 5, 5);

	constexpr double SMALL_RECT = 10;
	// Indicate clearly selected items
	for (auto sel : scene()->selectedItems())
	{
		auto rect = mapFromScene(sel->sceneBoundingRect()).boundingRect();

		// TODO: The stuff below is a bit of a hack to make sure the selection of an entire project is properly
		// shown and updated. Without those two lines, the right & bottom parts of the rectangle are outside of the
		// area that is updated and are not drawn (or not cleared). The numbers are "empirical" and should be inferred
		// somehow
		if (rect.width() >= viewport()->width() - 10) rect.setWidth( rect.width() - 2);
		if (rect.height() >= viewport()->height() - 10) rect.setHeight( rect.height() - 2);

		// Make sure that no matter how small a selection is, it's always visible on the minimap
		if (rect.width() < 1) rect.setWidth(1);
		if (rect.height() < 1) rect.setHeight(1);
		painter.setBrush(Qt::NoBrush);

		if (rect.width() < SMALL_RECT && rect.height() < SMALL_RECT)
			painter.setPen(Qt::red);
		else
			painter.setPen(QPen{Qt::red, 1, Qt::DashLine});
		painter.drawRect(rect);
	}

	t->tick();
}

void MiniMap::mouseMoveEvent(QMouseEvent *event)
{
	// TODO: The next line should not be necessary, this seems like a Qt bug. Presently if the line is removed:
	// - Starting a test like OOVisualization, runs and behaves normally
	// - Starting Envision without a test, and opening a project later, causes the minimap to always scroll when the
	//   mouse is moved above it (even without a pressed key). Those mouseMoveEvents should not be received since
	//   mouse tracking is disabled.
	if (event->buttons() == Qt::NoButton) return;
	View::mouseMoveEvent(event);
	parent_->centerOn(mapToScene(QRect{event->pos(), QSize{2, 2}}).boundingRect().center());
}

void MiniMap::mousePressEvent(QMouseEvent *event)
{
	View::mousePressEvent(event);
	parent_->centerOn(mapToScene(QRect{event->pos(), QSize{2, 2}}).boundingRect().center());
}

void MiniMap::updateMap()
{
	QRectF maxRect;
	maxRect.setLeft( sceneRect().x() < visibleRect.x() ? sceneRect().x() : visibleRect.x() );
	maxRect.setTop( sceneRect().y() < visibleRect.y() ? sceneRect().y() : visibleRect.y() );
	maxRect.setRight( (sceneRect().x() + sceneRect().width()) > (visibleRect.x()+visibleRect.width())
			? (sceneRect().x() + sceneRect().width()) : (visibleRect.x()+visibleRect.width()));
	maxRect.setBottom( (sceneRect().y() + sceneRect().height()) > (visibleRect.y()+visibleRect.height())
			? (sceneRect().y() + sceneRect().height()) : (visibleRect.y()+visibleRect.height()));

	qreal xScale = (width() - 2*frameWidth() - 2*margin_) / maxRect.width();
	qreal yScale = (height() - 2*frameWidth() - 2*margin_) / maxRect.height();
	qreal scale = xScale < yScale ? xScale : yScale;

	qreal rectX = margin_ + (visibleRect.x() - maxRect.x())*scale;
	qreal rectY = margin_ + (visibleRect.y() - maxRect.y())*scale;

	if (xScale < yScale) rectY += (height() - 2*(frameWidth() + margin_) - maxRect.height()*scale) / 2;
	else rectX += (width() - 2*(frameWidth() + margin_) - maxRect.width()*scale) / 2;

	// Below we subtract 0.5 and take the ceiling. This rounds the number. We further subtract 1 in order to compensate
	// for the pen width of the drawn rectangle.
	drawnRect.setRect(rectX, rectY, ceil(visibleRect.width()*scale - 1.5), ceil(visibleRect.height()*scale - 1.5));

	setTransform(QTransform::fromScale(scale, scale).translate(margin_, margin_));

	viewport()->update();
}

}
