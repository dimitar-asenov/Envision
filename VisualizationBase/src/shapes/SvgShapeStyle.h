/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#pragma once

#include "../visualizationbase_api.h"

#include "ShapeStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API SvgShapeStyle : public ShapeStyle
{
	private:
		QString filename_;
		QSvgRenderer renderer_;
		double topContentMarginFraction_;
		double bottomContentMarginFraction_;
		double leftContentMarginFraction_;
		double rightContentMarginFraction_;

	public:
		virtual void load(StyleLoader& sl);

		const QString& filename() const;
		double topContentMarginFraction() const;
		double bottomContentMarginFraction() const;
		double leftContentMarginFraction() const;
		double rightContentMarginFraction() const;

		void paint(QPainter* painter, int x, int y, int width, int height) const;
};

inline const QString& SvgShapeStyle::filename() const { return filename_; }
inline double SvgShapeStyle::topContentMarginFraction() const { return topContentMarginFraction_; }
inline double SvgShapeStyle::bottomContentMarginFraction() const { return bottomContentMarginFraction_; }
inline double SvgShapeStyle::leftContentMarginFraction() const { return leftContentMarginFraction_; }
inline double SvgShapeStyle::rightContentMarginFraction() const { return rightContentMarginFraction_; }

} /* namespace Visualization */