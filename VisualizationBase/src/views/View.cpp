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

#include "View.h"
#include "../Scene.h"
#include "../VisualizationManager.h"

namespace Visualization {

View::View(Scene* scene, View *parent) :
	QGraphicsView(scene, parent), hiddenItemCategories_{}
{
	setOptimizationFlag(DontSavePainterState);
	if ( parent == nullptr )
	{
		setParent(VisualizationManager::instance().getMainWindow());
		VisualizationManager::instance().addTopLevelView(this);
	}
}

View::~View()
{
}

QRectF View::visibleRect()
{
	int h = horizontalScrollBar() ? horizontalScrollBar()->height() : 0;
	int w = verticalScrollBar() ? verticalScrollBar()->width() : 0;
	return mapToScene(viewport()->rect().adjusted(0, 0, -w, -h)).boundingRect();
}

Scene* View::scene()
{
	return static_cast<Scene*> (QGraphicsView::scene());
}

void View::paintEvent(QPaintEvent* event)
{
	scene()->setHiddenItemCategories(hiddenItemCategories_);
	scene()->setCurrentPaintView(this);
	QGraphicsView::paintEvent(event);
	scene()->setCurrentPaintView(nullptr);
}

}
