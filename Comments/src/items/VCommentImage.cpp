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

#include "VCommentImage.h"
#include "VisualizationBase/src/items/ItemStyle.h"

namespace Comments {

/*
 * Padding of text error
 */
unsigned int VCommentImage::errorTextPadding = 10;

ITEM_COMMON_DEFINITIONS(VCommentImage, "item")

VCommentImage::VCommentImage(Visualization::Item* parent, const QString& path, const StyleType* style)
	: Super(parent, style)
{
	path_ = path;
	image_ = new QImage(path);
	if(!image_->isNull())
		size_ = image_->size();
}

VCommentImage::VCommentImage(Visualization::Item* parent, const QString& text, QSize size, const StyleType* style)
	: VCommentImage(parent, text, style)
{
	updateSize(size);
}

VCommentImage::~VCommentImage()
{
	SAFE_DELETE(image_);
}

void VCommentImage::determineChildren()
{
}

void VCommentImage::updateGeometry(int, int)
{
	if(image_->isNull())
	{
		text_ = "This image could not be loaded: "+path_;
		QRect rect = textDimensions(scene()->font(), text_);
		size_ = rect.size()+QSize(2*errorTextPadding, 2*errorTextPadding);
	}
	setSize(size_);
}

QList<Visualization::Item*> VCommentImage::childItems() const
{
	return {};
}

QRect VCommentImage::textDimensions(QFont font, const QString& text)
{
	float minRatio = 1.5, maxRatio = 4.0;

	QFontMetrics qfm(font);
	// compute reasonable text dimensions
   int width = 0;
   QRect rect = qfm.boundingRect(0, 0, width, 0, Qt::AlignCenter | Qt::TextWordWrap, text);
   float ratio = float(rect.width()) / float(rect.height());
   // if the ratio is initially already above the max ratio, there is nothing we can do,
   // as this means it contains one huge word that requires at least this much width...
   // i.e. we can not reduce the width to improve the ratio
   int i = 0;
   if(ratio < maxRatio)
   {
   	// let's make this defensive... I don't think this can possibly
   	// get stuck in an infinite loop, but I don't trust my gut.
   	// => limit it to 100 iterations
		while(i < 100 && (ratio < minRatio || ratio > maxRatio))
		{
			i++;
			if (ratio < 1.5)
				width += 30;
			else
				width -= 30;

			rect = qfm.boundingRect(0, 0, width, 0, Qt::AlignCenter | Qt::TextWordWrap, text);
			ratio = float(rect.width())/float(rect.height());
		}
   }
   // move rect to (0,0) as boundingRect may return negative coordinates for the top left point
   rect.translate(-rect.topLeft());
   return rect;
}

void VCommentImage::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	QRectF rect(QPointF(0,0), size());
	if(!image_->isNull())
		painter->drawImage(rect, *image_);
	else
	{
		QRectF padded(QPointF(errorTextPadding, errorTextPadding), size()-QSize(2*errorTextPadding, 2*errorTextPadding));
		// draw text with some padding
		painter->fillRect(rect, QBrush(QColor(255, 200, 200)));
		painter->drawText(padded, Qt::AlignCenter | Qt::TextWordWrap, text_);
	}
}

void VCommentImage::updateSize(QSize size)
{
	if(!image_->isNull())
	{
		QSize imageSize = image_->size();

		if(size.width() == 0 && size.height() == 0)
			size = imageSize;
		else if(size.width() == 0)
			size.setWidth(imageSize.width() * ((double)size.height() / imageSize.height()));
		else if(size.height() == 0)
			size.setHeight(imageSize.height() * ((double)size.width() / imageSize.width()));

		size_ = size;
	}
}

void VCommentImage::resizeBy(QPoint diff)
{
	if(image_->isNull())
		return;

	size_ = QSize(size_.width() + diff.x(), size_.height() + diff.y());

	if(lineNumber_ > -1)
	{
		auto node = dynamic_cast<VComment*>(parent())->node();
		auto child = node->lines()->at(lineNumber_);
		auto line = child->get();
		// does the line contain a pipe?
		auto pipe = line.lastIndexOf('|');
		if(pipe == -1)
			pipe = line.size() - 1;

		auto updated = line.left(pipe) +
				"|" + QString::number(size_.width()) + "x" + QString::number(size_.height()) + "]";

		node->beginModification("Updating image dimensions");
		node->lines()->replaceChild(child, new Model::Text(updated));
		node->endModification();
	}

	setUpdateNeeded(StandardUpdate);
}

} /* namespace Comments */

