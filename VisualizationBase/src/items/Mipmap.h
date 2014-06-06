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

namespace Visualization {

class VISUALIZATIONBASE_API Mipmap {
	public:

		/**
		 * Paints a precached QPixmap using \a painter at the specified position.
		 *
		 * Returns true if an appropriately sized QPixmap was found and false otherwise.
		 */
		bool paint(QPainter* painter, int x, int y);

		void setImage(const QPixmap& pixmap, qreal scaleFactor);
		void setImage(const QPixmap& pixmap, const QPainter* painterToDetermineScaleFactor);
		void setImage(const QImage& image, qreal scaleFactor);
		void setImage(const QImage& image, const QPainter* painterToDetermineScaleFactor);

		/**
		 * Sets the minimum scaling factor for which a QPixmap will be precomputed. Smaller images will be based on the
		 * smallest precomputed pixmap.
		 */
		void setMinimalScaleFactor(qreal factor);
		qreal minimalScaleFactor();

		bool containsImageForScale(qreal scaleFactor);

	private:
		QMap<qreal, QPixmap> scaleFactorImagesMap_;
		qreal minimalScaleFactor_{0.1};
};

inline qreal Mipmap::minimalScaleFactor() { return minimalScaleFactor_; }
inline void Mipmap::setMinimalScaleFactor(qreal factor) { minimalScaleFactor_ = factor; }

}
