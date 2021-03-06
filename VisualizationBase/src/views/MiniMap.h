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

#include "../visualizationbase_api.h"

#include "View.h"

namespace Visualization {

class Scene;

class VISUALIZATIONBASE_API MiniMap : public View
{
	Q_OBJECT

	public:
		MiniMap(Scene* scene, View* parent);

		void updatePosition();

		void setMargin(qreal margin);

		constexpr static qreal DEFAULT_MARGIN = 2.0;

		void sceneRectChanged(const QRectF & rect);
		void visibleRectChanged();

	protected:
		virtual void resizeEvent(QResizeEvent* event) override;
		virtual void paintEvent(QPaintEvent* event) override;
		virtual void mouseMoveEvent(QMouseEvent* event) override;
		virtual void mousePressEvent(QMouseEvent* event) override;

	private:
		View *parent_;
		int margin_;

		QRectF visibleRect;
		QRect drawnRect;

		void updateMap();
};

inline void MiniMap::setMargin(qreal margin) { margin_ = margin; }

}
