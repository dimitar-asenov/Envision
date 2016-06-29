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

using namespace Visualization;

namespace Comments {

QHash<QString, HCommentText::EmbedFunction>& HCommentText::embedKeywords()
{
	static QHash<QString, HCommentText::EmbedFunction> hash
		{{"comment", []()->Model::Node*{return new CommentNode{};}}};
	return hash;
}

void HCommentText::registerEmbedKeyword(const QString& keyword, EmbedFunction function)
{
	Q_ASSERT(!embedKeywords().contains(keyword));
	embedKeywords().insert(keyword, function);
}

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
			auto embedding = embedKeywords().find(aText->get());
			if (embedding != embedKeywords().end())
				newNode = embedding.value()();
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
