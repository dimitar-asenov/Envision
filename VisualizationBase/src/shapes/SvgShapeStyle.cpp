/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "SvgShapeStyle.h"
#include "VisualizationException.h"

namespace Visualization {

void SvgShapeStyle::load(StyleLoader& sl)
{
	ShapeStyle::load(sl);
	sl.load("filename", filename_);
	sl.load("topContentMarginFraction", topContentMarginFraction_);
	sl.load("bottomContentMarginFraction", bottomContentMarginFraction_);
	sl.load("leftContentMarginFraction", leftContentMarginFraction_);
	sl.load("rightContentMarginFraction", rightContentMarginFraction_);

	if (filename_.isEmpty()) return;
	if (!renderer_.load(filename_)) throw VisualizationException("Could not read SVG shape: " + filename_);
}

void SvgShapeStyle::paint(QPainter* painter, int x, int y, int width, int height) const
{
	QSvgRenderer& r = const_cast<QSvgRenderer&> (renderer_);
	r.render(painter, QRectF(x, y, width, height));
}


} /* namespace Visualization */
