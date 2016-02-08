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

#include "VCommentBrowser.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/shapes/Shape.h"
#include "VisualizationBase/src/VisualizationManager.h"

namespace Comments {

ITEM_COMMON_DEFINITIONS(VCommentBrowser, "item")

const QSize VCommentBrowser::defaultSize = QSize{400, 300};

VCommentBrowser::VCommentBrowser(Visualization::Item* parent, const QUrl& url, const StyleType* style)
	: VCommentBrowser{parent, url, defaultSize, style}
{}

VCommentBrowser::VCommentBrowser(Visualization::Item* parent, const QUrl& url, QSize size, const StyleType* style)
	: Super{parent, style}, browser_{new QGraphicsWebView{this}}, size_{size}
{
	browser_->setUrl(url);
}

VCommentBrowser::VCommentBrowser(Visualization::Item* parent, const QString& content, const StyleType* style)
	: Super{parent, style}, browser_{new QGraphicsWebView{this}}, size_{defaultSize}
{
	setContent(content);
}

VCommentBrowser::~VCommentBrowser()
{
	//TODO: Remove this dirty hack. This looks like a bug in QGraphicsWebView, but more investigation is needed.
	// This item is originally removed from the scene when SAFE_DELETE_ITEM is called. It should not be necessary to
	// add this item to the scene before destroying the browser_. However, the destructor of browser seems to
	// eventually call QGraphicsScene::removeItem(). But where does it get a scene from?! If we don't add 'this' to
	// the scene browser_->scene() returns nullptr. This resutls in a Segmentation Fault.
	// More debugging is required, preferrably with symbols for the source code that happens when the destructor of
	// QGraphicsWebView is called.
	Visualization::VisualizationManager::instance().mainScene()->addItem(this);
	SAFE_DELETE(browser_);
}

void VCommentBrowser::determineChildren()
{
	if (size_.isValid() && !heightResizesWithContent_) browser_->resize(size_);
}

void VCommentBrowser::updateGeometry(int, int)
{
	auto size = heightResizesWithContent_ ? QSize{size_.width(), browser_->page()->mainFrame()->contentsSize().height()}
													  : size_;

	if (hasShape())
	{
		getShape()->setInnerSize(size.width(), size.height());
		browser_->setPos(getShape()->contentLeft(), getShape()->contentTop());
	}
	else
	{
		setSize(size);
		browser_->setPos(0, 0);
	}
}

QList<Visualization::Item*> VCommentBrowser::childItems() const
{
	return {};
}

void VCommentBrowser::updateSize(QSize size)
{
	if (size.width() > 0 && size.height() > 0)
		size_ = size;
	setUpdateNeeded(StandardUpdate);
}

void VCommentBrowser::setContent(const QString &content)
{
	browser_->setHtml(content);
	if (!heightResizesWithContent_) browser_->resize(size_);
}

void VCommentBrowser::setHeightResizesWithContent(bool heightResizesWithContent)
{
	if (heightResizesWithContent_ != heightResizesWithContent)
	{
		heightResizesWithContent_ = heightResizesWithContent;

		if (heightResizesWithContent)
		{
			browser_->page()->setPreferredContentsSize({size_.width(), 10});
			browser_->setResizesToContents(true);
		}
		else
		{
			browser_->page()->setPreferredContentsSize({});
			browser_->resize(defaultSize);
		}

		setUpdateNeeded(StandardUpdate);
	}

	auto lambda = [this] { setUpdateNeeded(Visualization::Item::StandardUpdate); };

	if (heightResizesWithContent && !connection_)
		connection_ = QObject::connect(browser_, &QGraphicsWebView::geometryChanged, lambda);
	else if (!heightResizesWithContent)
		QObject::disconnect(connection_);
}

void VCommentBrowser::addJavascriptObject(const QString& name, QObject *object)
{
	browser_->page()->mainFrame()->addToJavaScriptWindowObject(name, object);
}

}
