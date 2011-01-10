/***********************************************************************************************************************
 * MiniMap.cpp
 *
 *  Created on: Jan 10, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "views/MiniMap.h"
#include "Scene.h"

namespace Visualization {

MiniMap::MiniMap(Scene *scene, View *parent_) : View(scene, parent_), parent(parent_), margin(DEFAULT_MARGIN)
{
	setInteractive(false);
	updatePosition();
	sceneRectChanged(scene->sceneRect());
	visibleRectChanged();
	setRenderHint(QPainter::Antialiasing);

	connect(scene, SIGNAL(sceneRectChanged(const QRectF &)), this, SLOT(sceneRectChanged(const QRectF &)));
}

MiniMap::~MiniMap()
{
	// TODO Auto-generated destructor stub
}

void MiniMap::updatePosition()
{
	move(frameWidth(),parentWidget()->height() - height()-frameWidth());
}

void MiniMap::resizeEvent( QResizeEvent *event )
{
	View::resizeEvent(event);
	updatePosition();
}

void MiniMap::sceneRectChanged(const QRectF & rect)
{
	sceneRect = rect;
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

void MiniMap::updateMap()
{
	QRectF maxRect;
	maxRect.setLeft( sceneRect.x() < visibleRect.x() ? sceneRect.x() : visibleRect.x() );
	maxRect.setTop( sceneRect.y() < visibleRect.y() ? sceneRect.y() : visibleRect.y() );
	maxRect.setRight( (sceneRect.x() + sceneRect.width()) > (visibleRect.x()+visibleRect.width()) ? (sceneRect.x() + sceneRect.width()) : (visibleRect.x()+visibleRect.width()));
	maxRect.setBottom( (sceneRect.y() + sceneRect.height()) > (visibleRect.y()+visibleRect.height()) ? (sceneRect.y() + sceneRect.height()) : (visibleRect.y()+visibleRect.height()));

	qreal xScale = (width() - 2*frameWidth() - 2*margin) / maxRect.width();
	qreal yScale = (height() - 2*frameWidth() - 2*margin) / maxRect.height();
	qreal scale = xScale < yScale ? xScale : yScale;

	qreal rectX = margin + (visibleRect.x() - maxRect.x())*scale;
	qreal rectY = margin + (visibleRect.y() - maxRect.y())*scale;

	if (xScale < yScale) rectY += (height() - 2*(frameWidth() + margin) - visibleRect.height()*scale) / 2;
	else rectX += (width() - 2*(frameWidth() + margin) - visibleRect.width()*scale) / 2;

	drawnRect.setRect(rectX, rectY, visibleRect.width()*scale, visibleRect.height()*scale);
	setTransform(QTransform::fromScale(scale,scale).translate(margin, margin));
}

}
