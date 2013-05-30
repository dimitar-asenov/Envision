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

#include "TestBox.h"
#include "ModelBase/src/model/Model.h"

namespace Visualization {

ITEM_COMMON_DEFINITIONS(TestBox, "item")

TestBox::TestBox(Item* parent, NodeType* node): Super{parent, itemStyles().get(), node->label()}, node_{node}
{}

TestBox::TestBox(NodeType* node) : Super{nullptr, itemStyles().get(), node->label()}, node_{node}
{}

TestBox::TestBox(const QString& label, QColor color, bool sizeDependsOnParent)
: Super{nullptr, itemStyles().get(), label}, color_(color), sizeDependsOnParent_(sizeDependsOnParent)
{}

TestBox::TestBox(const QString& label, bool sizeDependsOnParent)
: Super{nullptr, itemStyles().get(), label}, sizeDependsOnParent_(sizeDependsOnParent)
{}

bool TestBox::hasNode() const
{
	return node_;
}

TestBox::NodeType* TestBox::node() const
{
	return node_;
}

int TestBox::revision() const
{
	return nodeRevision_;
}

void TestBox::setRevision(int newRevision)
{
	nodeRevision_ = newRevision;
}

bool TestBox::sizeDependsOnParent() const
{
	if (node_) return node_->sizeDependsOnParent();
	else return sizeDependsOnParent_;
}

bool TestBox::setText(const QString& newText)
{
	if (node_)
	{
		node_->model()->beginModification(node(), "Set TestBox text");
		node_->setLabel(newText);
		node_->model()->endModification();
	}

	return TextRenderer::setText(newText);
}

QString TestBox::currentText()
{
	if (node_) return node_->label();
	else return text();
}

bool TestBox::moveCursor(CursorMoveDirection dir, QPoint reference)
{
	if (dir == MoveDefault)
		return TextRenderer::moveCursor(MoveLeftOf, QPoint(xEnd()+1,0));
	else return TextRenderer::moveCursor(dir, reference);
}

void TestBox::determineChildren()
{}

void TestBox::updateGeometry(int availableWidth, int availableHeight)
{
	Q_ASSERT(!hasShape());

	TextRenderer::updateGeometry(availableWidth, availableHeight);
	if (sizeDependsOnParent() && (availableWidth > width() || availableHeight > height()))
	{
		int w = availableWidth > width() ? availableWidth : width();
		int h = availableHeight > height() ? availableHeight : height();
		setSize(w,h);
	}

	// Also update the color
	if (node_) color_ = QColor(node_->red(), node_->green(), node_->blue());
}

void TestBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->fillRect(0,0,width(), height(), color_);
	TextRenderer::paint(painter, option, widget);
}

} /* namespace Visualization */
