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

#include "CCreateShape.h"
#include "../items/VCommentDiagram.h"
#include "../nodes/CommentDiagram.h"
#include "../nodes/CommentDiagramShape.h"

namespace Comments {

CCreateShape::CCreateShape() : Interaction::CreateNamedObjectWithAttributes{"shape",
		{{"ellipse", "diamond", "rectangle"}}}
{
}

Interaction::CommandResult* CCreateShape::executeNamed(Visualization::Item*, Visualization::Item* target,
		const std::unique_ptr<Visualization::Cursor>&,
		const QString&, const QStringList& attributes)
{
	auto vdiagram = DCast<VCommentDiagram>(target);
	auto last = vdiagram->lastRightClick();
	int x = std::max(0, last.x()-50), y = std::max(0, last.y()-50);

	auto diagram = DCast<CommentDiagram> (target->node());
	auto shape = new CommentDiagramShape{x, y, 100, 100, CommentDiagramShape::ShapeType::Rectangle};
	// what kind of shape?
	if (attributes.first() == "ellipse") shape->setShapeType(CommentDiagramShape::ShapeType::Ellipse);
	else if (attributes.first() == "diamond") shape->setShapeType(CommentDiagramShape::ShapeType::Diamond);

	diagram->beginModification("create shape");
	diagram->shapes()->append(shape);
	diagram->endModification();
	target->setUpdateNeeded(Visualization::Item::StandardUpdate);

	return new Interaction::CommandResult{};
}

}
