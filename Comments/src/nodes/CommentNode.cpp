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

#include "CommentNode.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(Comments::CommentNode)

namespace Comments {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(CommentNode)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(CommentNode)

REGISTER_ATTRIBUTE(CommentNode, lines, TypedListOfText, false, false, true)
REGISTER_ATTRIBUTE(CommentNode, diagrams, TypedListOfCommentDiagram, false, false, true)

CommentNode::CommentNode(const QString& text)
: Super{nullptr, CommentNode::getMetaData()}
{
	QStringList linesList = text.split(QRegExp("\\r?\\n"));
	for(auto line : linesList)
		lines()->append(new Model::Text(line));
}

CommentDiagram* CommentNode::diagram(const QString& name)
{
	for(auto diagram : *diagrams())
		if(diagram->name() == name) return diagram;

	return nullptr;
}

void CommentNode::synchronizeDiagramsToText()
{
	QStringList diagramNamesFromText;
	for(auto lineNode : *lines())
	{
		auto line = lineNode->get();
		if (line.startsWith("[diagram#") && line.right(1) == "]" && line.size() > 9+1)
			diagramNamesFromText << line.mid(9, line.size()-9-1);
	}

	diagramNamesFromText.removeDuplicates();

	// Remove old diagrams
	for(int i = diagrams()->size() - 1; i>=0; --i)
	{
		if (diagramNamesFromText.contains(diagrams()->at(i)->name()))
			diagramNamesFromText.removeOne(diagrams()->at(i)->name()); // There should be only one, we removed duplicates
		else diagrams()->remove(i);
	}

	// Add new diagrams
	for(auto newDiagram : diagramNamesFromText)
		diagrams()->append(new CommentDiagram(nullptr, newDiagram));
}

} /* namespace Comments */
