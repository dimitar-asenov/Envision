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

#include "VCommentImage.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/shapes/Shape.h"

namespace Comments {

ITEM_COMMON_DEFINITIONS(VCommentImage, "item")

const QSize VCommentImage::errorSize_{400, 400};

VCommentImage::VCommentImage(Visualization::Item* parent, const QString& path, const StyleType* style)
	: Super{parent, style}, path_{path}, image_{path}, size_{image_.isNull() ? errorSize_ : image_.size()}
{}

VCommentImage::VCommentImage(Visualization::Item* parent, const QString& path, QSize size, const StyleType* style)
	: VCommentImage{parent, path, style}
{
	Q_ASSERT(size.isValid());
	updateSize(size);
}

void VCommentImage::determineChildren()
{
}

void VCommentImage::updateGeometry(int, int)
{
	QSize minSize = image_.isNull() ? errorSize_ : size_;

	if (hasShape()) getShape()->setInnerSize(minSize.width(), minSize.height());
	else setSize(minSize);
}

void VCommentImage::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	int xOffset = 0;
	int yOffset = 0;
	if (hasShape())
	{
		xOffset = getShape()->contentLeft();
		yOffset = getShape()->contentTop();
	}

	if (image_.isNull())
	{
		painter->setPen(QPen{QColor{100, 100, 100}});
		painter->drawText(QRect{QPoint{xOffset, yOffset}, errorSize_}, Qt::AlignCenter | Qt::TextWordWrap,
								"This image could not be loaded: " + path_);
	}
	else
		painter->drawImage(QRect{QPoint{xOffset, yOffset}, size_}, image_);
}

bool VCommentImage::updateSize(QSize size)
{
	Q_ASSERT(size.isValid());

	if (!image_.isNull())
	{
		QSize imageSize = image_.size();

		if (size.width() == 0 && size.height() == 0)
			size = imageSize;
		else if (size.width() == 0)
			size.setWidth(imageSize.width() * ((double)size.height() / imageSize.height()));
		else if (size.height() == 0)
			size.setHeight(imageSize.height() * ((double)size.width() / imageSize.width()));

		size_ = size;
		setUpdateNeeded(StandardUpdate);
		return true;
	}
	else
	{
		size_ = errorSize_;
		setUpdateNeeded(StandardUpdate);
		return false;
	}
}

}
