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

#include "HCommentImage.h"
#include "../items/VCommentDiagram.h"

namespace Comments {

HCommentImage::HCommentImage()
{
}

HCommentImage* HCommentImage::instance()
{
	static HCommentImage h;
	return &h;
}

void HCommentImage::mousePressEvent(Visualization::Item* target, QGraphicsSceneMouseEvent *event)
{
	event->ignore();

	if (event->button() == Qt::RightButton && event->modifiers() == Qt::ShiftModifier)
	{
		event->accept();
		resizing_ = true;
	}

	if (!event->isAccepted())
		GenericHandler::mousePressEvent(target, event);
}

void HCommentImage::mouseReleaseEvent(Visualization::Item * /* target */, QGraphicsSceneMouseEvent *)
{
	if (resizing_) resizing_ = false;
}

void HCommentImage::mouseMoveEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	auto image = DCast<VCommentImage>(target);

	if (resizing_ && event->buttons() & Qt::RightButton)
	{
		QPoint diff((event->scenePos() - event->lastScenePos()).toPoint());
		auto newSize = image->imageSize() + QSize(diff.x(), diff.y());
		if (image->updateSize(newSize))
		{
			auto node = DCast<VComment>(image->parent())->node();
			Q_ASSERT(node);

			auto child = node->lines()->at(image->lineNumber());
			auto line = child->get();

			auto pipe = line.lastIndexOf('|');
			if (pipe == -1)
				pipe = line.size() - 1;

			auto updated = line.left(pipe) +
					"|" + QString::number(newSize.width()) + "x" + QString::number(newSize.height()) + "]";

			node->beginModification("Updating image dimensions");
			node->lines()->replaceChild(child, new Model::Text{updated});
			node->endModification();

			image->setUpdateNeeded(VCommentImage::StandardUpdate);
		}
	}
}

}
