/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "HCommentText.h"
#include "../items/VCommentText.h"
#include "../items/VCommentDiagramShape.h"
#include "../nodes/CommentFreeNode.h"

#include "OOModel/src/allOOModelNodes.h"

using namespace OOModel;
using namespace Visualization;

namespace Comments {

HCommentText::HCommentText()
{}

HCommentText* HCommentText::instance()
{
	static HCommentText h;
	return &h;
}

void HCommentText::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Return)
	{
		auto aText = static_cast<VCommentText*>(target)->node();
		auto aNode = DCast<CommentFreeNode>(target->node()->parent());

		if (aNode)
		{
			Model::Node* newNode = nullptr;
			if (aText->get() == "comment") newNode = new CommentNode{};
			else if (aText->get() == "class") newNode = new Class{};
			else if (aText->get() == "method") newNode = new Method{};
			else if (aText->get() == "statement") newNode = new Statement{};
			else if (aText->get() == "block") newNode = new Block{};
			else if (aText->get() == "foreach") newNode = new ForEachStatement{};
			else if (aText->get() == "if") newNode = new IfStatement{};
			else if (aText->get() == "loop") newNode = new LoopStatement{};
			else if (aText->get() == "switch") newNode = new SwitchStatement{};
			else if (aText->get() == "expression") newNode = new ExpressionStatement{};
			else newNode = new CommentText{};

			aNode->beginModification("set node");
			aNode->setNode(newNode);
			aNode->endModification();
		}
	}
	else
		HText::keyPressEvent(target, event);
}

void HCommentText::mousePressEvent(Visualization::Item *target, QGraphicsSceneMouseEvent *event)
{
	event->ignore();
	auto vShape = DCast<VCommentDiagramShape>(target->parent());
	if (vShape)
	{
		auto vDiagram = vShape->diagram();
		vDiagram->toolbar()->setDiagram(vDiagram);
		vDiagram->toggleEditing();
		if (vDiagram->toolbar()->selectionMode())
		{
			vDiagram->toolbar()->setCurrentShape(target->parent());
		}
	}
	HText::mousePressEvent(target, event);
}

}
