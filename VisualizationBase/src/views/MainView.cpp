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

#include "MainView.h"
#include "../Scene.h"
#include "../cursor/Cursor.h"
#include "../items/Item.h"
#include "../VisualizationBasePlugin.h"
#include "../VisualizationManager.h"

#include "Logger/src/Timer.h"
#include "Logger/src/Log.h"

namespace Visualization {

MainView::MainView(Scene *scene) :
	View(scene, nullptr), miniMap{new MiniMap{scene, this}}, scaleLevel{SCALING_FACTOR}
{
	setRenderHint(QPainter::Antialiasing);
	setRenderHint(QPainter::TextAntialiasing);

	// This is needed to resolve artifacts when zooming in/out and there are items sensitive to scale.
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	setMiniMapSize(MINIMAP_DEFAULT_WIDTH, MINIMAP_DEFAULT_HEIGHT);
}

MainView::~MainView()
{
	SAFE_DELETE(miniMap);
}

void MainView::setMiniMapSize(int width, int height)
{
	if ( miniMap )
	{
		miniMap->resize(width, height);
		miniMap->sceneRectChanged(scene()->sceneRect());
	}
}

bool MainView::event( QEvent *event )
{
	bool result = false;

	switch (event->type())
	{
		case QEvent::KeyPress :
		{
			QKeyEvent *k = (QKeyEvent *)event;
			keyPressEvent(k);
			if (k->key() == Qt::Key_Backtab || k->key() == Qt::Key_Tab ) event->accept();
			result = true;
		}
		break;
		case QEvent::KeyRelease :
		{
			QKeyEvent *k = (QKeyEvent *)event;
			keyReleaseEvent(k);
			if (k->key() == Qt::Key_Backtab || k->key() == Qt::Key_Tab ) event->accept();
			result = true;
		}
		break;
		default: result = View::event( event );
		break;
	}

	// The checks below are necessary to avoid infinite recursion.
	if ( event->type() != QEvent::ChildAdded
			&& event->type() != QEvent::ChildRemoved
			&& event->type() != QEvent::ChildPolished) updateInfoLabels();
	return result;
}

void MainView::resizeEvent(QResizeEvent *event)
{
	View::resizeEvent(event);
	if ( miniMap )
	{
		miniMap->updatePosition();
		miniMap->visibleRectChanged();
	}
}

void MainView::wheelEvent(QWheelEvent *event)
{
	// Zoom
	if (event->modifiers() == Qt::ControlModifier)
	{
		QPointF eventToViewportMiddleOffset;
		QGraphicsItem* itemUnderCursor = nullptr;
		QRectF boundingRectBeforeZoom;
		QPointF eventPosInItem;

		if (ITEM_STRUCTURE_AWARE_ZOOM_ANCHORING)
		{
			// find offset of event pos from middle of viewport
			eventToViewportMiddleOffset = event->posF() - QPointF(viewport()->width()/2.0, viewport()->height()/2.0);
			auto scenePos = mapToScene(event->pos());
			auto itemsAtEvent = items(event->pos());
			auto iter = itemsAtEvent.begin();
			// avoid zooming in on Overlays
			while (iter != itemsAtEvent.end() && (*iter)->acceptedMouseButtons() == Qt::NoButton) iter++;
			if (iter != itemsAtEvent.end())
			{
				itemUnderCursor = *iter;
				boundingRectBeforeZoom = itemUnderCursor->boundingRect();
				eventPosInItem = itemUnderCursor->mapFromScene(scenePos);
			}
		}

		if ( event->delta() > 0 ) --scaleLevel;
		else ++scaleLevel;

		if ( scaleLevel <= 0 )
		{
			// Nothing changes, we're already at the bottom
			scaleLevel = 1;
			return;
		}
		qreal factor = scaleFactor();

		// Prevent zooming-out so far that the scrollbars disappear.
		// This is OK, since the scene's extent is artifically enlarged and it is still possible to see the
		// entire scene at once, even with scrollbars.
		if (sceneRect().width()*factor <= viewport()->width() || sceneRect().height()*factor <= viewport()->height())
		{
			// Nothing changes, we're already at the largest zoom level.
			--scaleLevel;
			return;
		}

		setTransform(QTransform::fromScale(factor, factor));

		if ( miniMap ) miniMap->visibleRectChanged();

		scene()->setMainViewScalingFactor(factor);

		if (ITEM_STRUCTURE_AWARE_ZOOM_ANCHORING && itemUnderCursor)
		{
			qreal scalingX = 1.0;
			qreal scalingY = 1.0;
			// if the item size has changed, compute scaling to update coordinates in item
			if (itemUnderCursor->boundingRect() != boundingRectBeforeZoom)
			{
				scalingX = qreal(itemUnderCursor->boundingRect().width()) / qreal(boundingRectBeforeZoom.width());
				scalingY = qreal(itemUnderCursor->boundingRect().height()) / qreal(boundingRectBeforeZoom.height());
			}
			// center on the item pos using scaling
			centerOn(itemUnderCursor->mapToScene(QPointF{eventPosInItem.x()*scalingX, eventPosInItem.y()*scalingY}));
			// scroll view such that the item pos is again under the original event pos
			horizontalScrollBar()->setValue(horizontalScrollBar()->value() - eventToViewportMiddleOffset.x());
			verticalScrollBar()->setValue(verticalScrollBar()->value() - eventToViewportMiddleOffset.y());
		}

	}
	// Scroll
	else if (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier)
	{
		auto bar = event->orientation() == Qt::Vertical && event->modifiers() == Qt::NoModifier
				? verticalScrollBar() : horizontalScrollBar();

		bar->setValue( bar->value() - event->delta() );
	}
	else
		View::wheelEvent(event);
}

qreal MainView::scaleFactor() const
{
	if (scaleLevel < SCALING_FACTOR)
		return SCALING_FACTOR / (qreal) scaleLevel;
	else return std::pow(2, SCALING_FACTOR - scaleLevel);
}

void MainView::scrollContentsBy(int dx, int dy)
{
	View::scrollContentsBy(dx, dy);
	if ( miniMap ) miniMap->visibleRectChanged();
}

void MainView::keyPressEvent(QKeyEvent *event)
{
	if ( (event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::ShiftModifier)
			&& event->key() == Qt::Key_Print)
	{
		event->accept();
		scene()->setHiddenItemCategories(0);
		if (scene()->mainCursor() && scene()->mainCursor()->visualization())
			scene()->mainCursor()->visualization()->hide();

		QPrinter printer;
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setFullPage(true);
		printer.setResolution(300);

		QSvgGenerator svggen;
		svggen.setResolution(90);

		bool ownerIgnoresScale = setCursorAndOwnerIgnoreScaleForScreenShot(false, false);

		if (event->modifiers() & Qt::ShiftModifier)
		{
			// Print scene
			VisualizationBasePlugin::log().info("Capturing a screenshot of the entire scene.");

			printer.setOutputFileName("screenshot-scene.pdf");
			printer.setPaperSize(scene()->sceneRect().size().toSize(), QPrinter::Point);
			QPainter painter{&printer};
			painter.setRenderHint(QPainter::Antialiasing);
			scene()->render( &painter );

			svggen.setFileName("screenshot-scene.svg");
			svggen.setSize(scene()->sceneRect().size().toSize());
			QPainter svgPainter{&svggen};
			svgPainter.setRenderHint(QPainter::Antialiasing);
			scene()->render(&svgPainter);

			QImage image{PNG_SCREENSHOT_SCALE * scene()->sceneRect().size().toSize(), QImage::Format_ARGB32};
			image.fill(Qt::transparent);
			QPainter pmapPainter{&image};
			pmapPainter.setRenderHint(QPainter::Antialiasing);
			//pmapPainter.scale(2, 2);
			scene()->render(&pmapPainter);
			image.save("screenshot-scene.png");
		}
		else
		{
			// Print view
			VisualizationBasePlugin::log().info("Capturing a screenshot of the current view.");

			printer.setOutputFileName("screenshot-view.pdf");
			printer.setPaperSize(viewport()->rect().size(), QPrinter::Point);
			QPainter painter{&printer};
			painter.setRenderHint(QPainter::Antialiasing);
			render(&painter);

			svggen.setFileName("screenshot-view.svg");
			svggen.setSize(viewport()->rect().size());
			QPainter svgPainter{&svggen};
			svgPainter.setRenderHint(QPainter::Antialiasing);
			render(&svgPainter);

			QImage image{PNG_SCREENSHOT_SCALE * viewport()->rect().size(), QImage::Format_ARGB32};
			image.fill(Qt::transparent);
			QPainter pmapPainter{&image};
			pmapPainter.setRenderHint(QPainter::Antialiasing);
			render(&pmapPainter);
			image.save("screenshot-view.png");
		}

		setCursorAndOwnerIgnoreScaleForScreenShot(true, ownerIgnoresScale);
	}
	else if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier) && event->key() == Qt::Key_P)
	{
		// Toggle presentation mode (full screen + no scroll bars + no minimap)

		event->accept();
		QWidget* window = this;
		while (window->parentWidget()) window = window->parentWidget();

		if (window->isFullScreen())
		{
			window->showNormal();
			setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
			setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
			miniMap->show();
		}
		else
		{
			window->showFullScreen();
			setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			miniMap->hide();
		}
	}
	else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_F11)
	{
		// Toggle fullscreen only
		event->accept();
		QWidget* window = this;
		while (window->parentWidget()) window = window->parentWidget();

		if (window->isFullScreen()) window->showNormal();
		else window->showFullScreen();
	}
	else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_F10)
	{
		event->accept();
		if (miniMap->width() == MINIMAP_DEFAULT_WIDTH && miniMap->height() ==  MINIMAP_DEFAULT_HEIGHT)
			setMiniMapSize(width(), height());
		else setMiniMapSize(MINIMAP_DEFAULT_WIDTH, MINIMAP_DEFAULT_HEIGHT);
	}
	else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_F9)
	{
		event->accept();
		if (miniMap->isVisible()) miniMap->hide();
		else miniMap->show();
	}
	else if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_F7)
	{
		event->accept();
		showTimers_ = ~showTimers_;
		updateInfoLabels();
	}
	else View::keyPressEvent(event);
}

void MainView::updateInfoLabels()
{
	if (!showTimers_)
	{
		if (!infoLabels_.isEmpty())
		{
			for (auto label : infoLabels_) SAFE_DELETE(label);
			infoLabels_.clear();
		}
		return;
	}

	int labelIndex = 0;
	int yPos = 10;

	// Add timers
	for (auto name : Logger::Timer::timerNames())
	{
		QLabel* label{};

		if (infoLabels_.size() <= labelIndex)
		{
			label = new QLabel{this};
			label->setAttribute(Qt::WA_TransparentForMouseEvents);
			infoLabels_.append(label);
			label->show();
			label->setStyleSheet("QLabel { color : red; }");
		}
		else label = infoLabels_.at(labelIndex);

		// Set the position of the label
		label->move(10, yPos);

		// Construct label text
		auto info = name + ":  ";
		for (auto val : Logger::Timer::timer(name)->values())
			info += QString::number(val) + "  ";

		label->setText(info);
		label->adjustSize();

		++labelIndex;
		yPos+=label->height();
	}

	// Remove unnecessary labels
	while (labelIndex < infoLabels_.size() )
	{
		SAFE_DELETE(infoLabels_.last());
		infoLabels_.removeLast();
	}
}

void MainView::paintEvent(QPaintEvent* event)
{
	auto t = Logger::Timer::start("Main view paint");
	View::paintEvent(event);
	t->tick();
}

void MainView::mouseMoveEvent(QMouseEvent *event)
{
	if (!isPanning_) return View::mouseMoveEvent(event);

	event->accept();
	horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - panStartPos_.x()));
	verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - panStartPos_.y()));
	panStartPos_ = event->pos();
}

void MainView::mousePressEvent(QMouseEvent *event)
{
	if (event->modifiers() != Qt::ControlModifier) return View::mousePressEvent(event);

	event->accept();
	isPanning_ = true;
	panStartPos_ = event->pos();
	setCursor(Qt::ClosedHandCursor);
}

void MainView::mouseReleaseEvent(QMouseEvent *event)
{
	if (!isPanning_) return View::mouseReleaseEvent(event);

	event->accept();
	setCursor(Qt::ArrowCursor);
	isPanning_ = false;
}

bool MainView::setCursorAndOwnerIgnoreScaleForScreenShot(bool ignore, bool modifyOwner)
{
	bool ownerIgnoresScale = false;

	auto cursor = scene()->mainCursor();
	if (cursor)
	{
		if ( auto vis = cursor->visualization() ) vis->setFlag(Item::ItemIgnoresTransformations, ignore);
		auto owner = cursor->owner();
		if (owner)
		{
			while (owner->parent()) owner = owner->parent();
			if (!ignore)
			{
				ownerIgnoresScale = owner->flags() & Item::ItemIgnoresTransformations;
				if (ownerIgnoresScale) owner->setFlag(Item::ItemIgnoresTransformations, false);
			}
			else if (modifyOwner) owner->setFlag(Item::ItemIgnoresTransformations, true);
		}
	}

	return ownerIgnoresScale;
}

}
