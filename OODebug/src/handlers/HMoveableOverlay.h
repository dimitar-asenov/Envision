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

#pragma once

#include "../oodebug_api.h"

#include "InteractionBase/src/handlers/GenericHandler.h"

namespace OODebug {

class ConsoleOverlay;

template <class OverlayType>
class OODEBUG_API HMoveableOverlay : public Interaction::GenericHandler
{
	public:
		static HMoveableOverlay* instance();

		virtual void mousePressEvent(Visualization::Item* target, QGraphicsSceneMouseEvent *event) override;
		virtual void mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event) override;

	private:
		QPointF consolePosition_;

		void move(OverlayType* overlay, const QPointF& to);
};

template <class OverlayType>
HMoveableOverlay<OverlayType>* HMoveableOverlay<OverlayType>::instance()
{
	static HMoveableOverlay inst;
	return &inst;
}

template <class OverlayType>
void HMoveableOverlay<OverlayType>::mousePressEvent(Visualization::Item* target, QGraphicsSceneMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier)
		if (auto overlay = DCast<OverlayType>(target))
			consolePosition_ = overlay->pos();
}

template <class OverlayType>
void HMoveableOverlay<OverlayType>::mouseMoveEvent(Visualization::Item* target, QGraphicsSceneMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		if (auto overlay = DCast<OverlayType>(target))
		{
			QPointF diff((event->scenePos() - event->buttonDownScenePos(Qt::LeftButton)));
			move(overlay, diff);
		}
	}
}

template <class OverlayType>
void HMoveableOverlay<OverlayType>::move(OverlayType* overlay, const QPointF& to)
{
	QPointF dest(consolePosition_ + to);
	if (dest.x() < 0) dest.setX(0);
	if (dest.y() < 0) dest.setY(0);

	overlay->setPos(dest);
}

} /* namespace OODebug */
