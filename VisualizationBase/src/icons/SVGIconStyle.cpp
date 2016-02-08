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

#include "SVGIconStyle.h"
#include "../VisualizationException.h"

namespace Visualization {

SVGIconStyle::~SVGIconStyle(){}

void SVGIconStyle::load(StyleLoader& sl)
{
	Super::load(sl);
	if (!renderer_.load(filename())) throw VisualizationException{"Could not read SVG icon: " + filename()};

	// Pre-compute the normal and minimal icon sizes
	drawScaledPixmapInMipmap(1.0);
	drawScaledPixmapInMipmap(mipmap_.minimalScaleFactor());
}

void SVGIconStyle::paint(QPainter* painter, int x, int y) const
{
	// TODO: Check whether this work with Qt5. In theory uncommenting all lines below will make sure that we render a
	// vector version of the icon when generating PDFs or SVGs. However, there seems to be a problem when rendering
	// gradients - there is an additional black outline rendered in that case - seems to be a Qt issue.

//	auto paintEngineType = painter->device()->paintEngine()->type();
//	if (paintEngineType == QPaintEngine::SVG || paintEngineType == QPaintEngine::Pdf)
//	{
//		//Un-optimized version
//		renderer_.render(painter, QRectF{x, y, width_, height_});
//	}
//	else
//	{
		if (!mipmap_.paint(painter, x, y))
			if (drawScaledPixmapInMipmap(painter->worldTransform().m11()))
			{
				auto painted = mipmap_.paint(painter, x, y);
				Q_ASSERT(painted);
			}
//	}
}

bool SVGIconStyle::drawScaledPixmapInMipmap(qreal scaleFactor) const
{
	auto size = (QSizeF{(qreal) width(), (qreal) height()} * scaleFactor).toSize();
	if (size.width() > 0 && size.height() > 0)
	{
		QImage img = QImage{size, QImage::Format_ARGB32};
		img.fill(0);
		QPainter pai{&img};
		renderer_.render(&pai);

		mipmap_.setImage(img, scaleFactor);
		return true;
	}
	else return false;
}

}
