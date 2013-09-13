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

#include "VCommentBrowser.h"
#include "VisualizationBase/src/items/ItemStyle.h"

namespace Comments {

ITEM_COMMON_DEFINITIONS(VCommentBrowser, "item")

QSize VCommentBrowser::defaultSize = QSize(400, 300);

VCommentBrowser::VCommentBrowser(Visualization::Item* parent, const QString& text, const StyleType* style)
	: Super(parent, style)
{
	item_ = new QGraphicsWebView(this);
	item_->setUrl(QUrl(text));
	size_ = defaultSize;
}

VCommentBrowser::VCommentBrowser(Visualization::Item* parent, const QString& text, QSize size, const StyleType* style)
	: VCommentBrowser(parent, text, style)
{
	updateSize(size);
}

VCommentBrowser::~VCommentBrowser()
{
	SAFE_DELETE(item_);
}

void VCommentBrowser::determineChildren()
{
}

void VCommentBrowser::updateGeometry(int, int)
{
	setSize(size_);
	item_->setMaximumSize(size_);
}

QList<Visualization::Item*> VCommentBrowser::childItems() const
{
	return {};
}

void VCommentBrowser::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
	item_->paint(painter, style, widget);
}

void VCommentBrowser::updateSize(QSize size)
{
	if(size.width() == 0 || size.height() == 0)
	{
		qDebug() << "Notice: Invalid browser size" << size << "specified for url" << item_->url().toString();
		qDebug() << "Notice: Falling back to default size" << defaultSize;
		size_ = defaultSize;
	}
	else
		size_ = size;
}

} /* namespace Comments */

