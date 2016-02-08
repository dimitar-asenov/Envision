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

#include "Mipmap.h"

namespace Visualization {

void Mipmap::setImage(const QPixmap& pixmap, qreal scaleFactor)
{
	scaleFactorImagesMap_.insert(scaleFactor, pixmap);
}

void Mipmap::setImage(const QPixmap& pixmap, const QPainter* painterToDetermineScaleFactor)
{
	scaleFactorImagesMap_.insert(painterToDetermineScaleFactor->worldTransform().m11(), pixmap);
}

void Mipmap::setImage(const QImage& image, qreal scaleFactor)
{
	scaleFactorImagesMap_.insert(scaleFactor, QPixmap::fromImage(image));
}

void Mipmap::setImage(const QImage& image, const QPainter* painterToDetermineScaleFactor)
{
	scaleFactorImagesMap_.insert(painterToDetermineScaleFactor->worldTransform().m11(), QPixmap::fromImage(image));
}

bool Mipmap::paint(QPainter* painter, int x, int y)
{
	qreal scaleFactor = painter->worldTransform().m11();

	if (scaleFactor < minimalScaleFactor_)
	{
		auto it = scaleFactorImagesMap_.begin();
		if (it == scaleFactorImagesMap_.end()) return false;

		auto scaledSize = QSizeF{it.value().size()} * (scaleFactor / it.key());

		if (scaledSize.width() > 0 && scaledSize.height() > 0)
		{
			QPixmap scaledMore = it.value().scaled(scaledSize.toSize());
			painter->setWorldMatrixEnabled(false);
			painter->drawPixmap(x + painter->worldTransform().dx(), y + painter->worldTransform().dy(), scaledMore);
			painter->setWorldMatrixEnabled(true);
		}
	}
	else
	{
		auto it = scaleFactorImagesMap_.find(scaleFactor);

		if (it == scaleFactorImagesMap_.end()) return false;

		painter->setWorldMatrixEnabled(false);
		painter->drawPixmap(x*scaleFactor + painter->worldTransform().dx(),
				y*scaleFactor + painter->worldTransform().dy(), it.value());
		painter->setWorldMatrixEnabled(true);
	}

	return true;
}

bool Mipmap::containsImageForScale(qreal scaleFactor)
{
	if (scaleFactor < minimalScaleFactor_) return !scaleFactorImagesMap_.isEmpty();
	else return scaleFactorImagesMap_.find(scaleFactor) != scaleFactorImagesMap_.end();
}

}
