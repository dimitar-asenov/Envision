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
 * MiniMap.cpp
 *
 *  Created on: Jan 10, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "views/MiniMap.h"

#include <QtGui/QMouseEvent>

#include <cmath>

namespace Visualization {

MiniMap::MiniMap(Scene *scene, View *parent_) : View(scene, parent_), parent(parent_), margin(DEFAULT_MARGIN)
{
	setInteractive(false);
	updatePosition();
	updateMap();
	visibleRectChanged();
	setRenderHint(QPainter::Antialiasing);

	setHiddenItemCategories(Scene::MenuItemCategory | Scene::CursorItemCategory);

	connect(scene, SIGNAL(sceneRectChanged(const QRectF &)), this, SLOT(sceneRectChanged(const QRectF &)));
}

void MiniMap::updatePosition()
{
	QGraphicsView* parent = static_cast<QGraphicsView*> (parentWidget());

	move(0,parent->viewport()->height() - height() + 2*frameWidth());
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
	visibleRect = parent->visibleRect();
	updateMap();
}

void MiniMap::paintEvent(QPaintEvent *event)
{
	View::paintEvent(event);
	QPainter painter(viewport());
	painter.setPen(Qt::red);
	painter.drawRect(drawnRect);
}

void MiniMap::mouseMoveEvent(QMouseEvent *event)
{
	View::mouseMoveEvent(event);
	parent->centerOn(mapToScene(QRect(event->pos(), QSize(2, 2))).boundingRect().center());
}

void MiniMap::mousePressEvent(QMouseEvent *event)
{
	View::mousePressEvent(event);
	parent->centerOn(mapToScene(QRect(event->pos(), QSize(2, 2))).boundingRect().center());
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

	qreal xScale = (width() - 2*frameWidth() - 2*margin) / maxRect.width();
	qreal yScale = (height() - 2*frameWidth() - 2*margin) / maxRect.height();
	qreal scale = xScale < yScale ? xScale : yScale;

	qreal rectX = margin + (visibleRect.x() - maxRect.x())*scale;
	qreal rectY = margin + (visibleRect.y() - maxRect.y())*scale;

	if (xScale < yScale) rectY += (height() - 2*(frameWidth() + margin) - maxRect.height()*scale) / 2;
	else rectX += (width() - 2*(frameWidth() + margin) - maxRect.width()*scale) / 2;

	// Below we subtract 0.5 and take the ceiling. This rounds the number. We further subtract 1 in order to compensate
	// for the pen width of the drawn rectangle.
	drawnRect.setRect(rectX, rectY, ceil(visibleRect.width()*scale - 1.5), ceil(visibleRect.height()*scale - 1.5));

	setTransform(QTransform::fromScale(scale,scale).translate(margin, margin));

	viewport()->update();
}

}
