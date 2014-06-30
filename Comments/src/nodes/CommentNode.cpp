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

#include "CommentNode.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(Comments::CommentNode)

namespace Comments {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(CommentNode)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(CommentNode)

REGISTER_ATTRIBUTE(CommentNode, lines, TypedListOfText, false, false, true)
REGISTER_ATTRIBUTE(CommentNode, diagrams, TypedListOfCommentDiagram, false, false, true)
REGISTER_ATTRIBUTE(CommentNode, codes, TypedListOfCommentFreeNode, false, false, true)
REGISTER_ATTRIBUTE(CommentNode, tables, TypedListOfCommentTable, false, false, true)

CommentNode::CommentNode(const QString& text)
: Super{nullptr, CommentNode::getMetaData()}
{
	QStringList linesList = text.split(QRegExp("\\r?\\n"));
	for (auto line : linesList)
		lines()->append(new Model::Text(line));
}

CommentDiagram* CommentNode::diagram(const QString& name)
{
	for (auto diagram : *diagrams())
		if (diagram->name() == name) return diagram;

	return nullptr;
}

CommentFreeNode* CommentNode::code(const QString& name)
{
	for (auto code : *codes())
		if (code->name() == name) return code;

	return nullptr;
}


CommentTable* CommentNode::table(const QString& name)
{
	for (auto table : *tables())
		if (table->name() == name) return table;

	return nullptr;
}

void CommentNode::synchronizeDiagramsToText()
{
	QStringList diagramNamesFromText;
	for (auto lineNode : *lines())
	{
		auto line = lineNode->get();
		if (line.startsWith("[diagram#") && line.right(1) == "]" && line.size() > 9+1)
			diagramNamesFromText << line.mid(9, line.size()-9-1);
	}

	diagramNamesFromText.removeDuplicates();

	// Remove old diagrams
	for (int i = diagrams()->size() - 1; i>=0; --i)
	{
		if (diagramNamesFromText.contains(diagrams()->at(i)->name()))
			diagramNamesFromText.removeOne(diagrams()->at(i)->name()); // There should be only one, we removed duplicates
		else diagrams()->remove(i);
	}

	// Add new diagrams
	for (auto newDiagram : diagramNamesFromText)
		diagrams()->append(new CommentDiagram(nullptr, newDiagram));
}

void CommentNode::synchronizeCodesToText()
{
	QStringList codeNamesFromText;
	for (auto lineNode : *lines())
	{
		auto line = lineNode->get();
		if (line.startsWith("[code#") && line.right(1) == "]" && line.size() > 6+1)
			codeNamesFromText << line.mid(6, line.size()-6-1);
	}

	codeNamesFromText.removeDuplicates();

	// Remove old codes
	for (int i = codes()->size() - 1; i>=0; --i)
	{
		if (codeNamesFromText.contains(codes()->at(i)->name()))
			codeNamesFromText.removeOne(codes()->at(i)->name()); // There should be only one, we removed duplicates
		else codes()->remove(i);
	}

	// Add new codes
	for (auto newCode : codeNamesFromText)
		codes()->append(new CommentFreeNode(nullptr, newCode));
}

void CommentNode::synchronizeTablesToText()
{
	QStringList tableNamesFromText;
	QMap<QString, int> mapRows;
	QMap<QString, int> mapColumns;
	for (auto lineNode : *lines())
	{
		auto line = lineNode->get();
		if (line.startsWith("[table#") && line.right(1) == "]" && line.size() > 7+1 && line.count('#')==3)
		{
			QStringList aList = line.split("#");
			tableNamesFromText << aList[1];
			mapRows.insert(aList[1], aList[2].toInt());
			mapColumns.insert(aList[1], aList[3].remove(']').toInt());
		}
	}

	tableNamesFromText.removeDuplicates();

	// Remove old tables
	for (int i = tables()->size() - 1; i>=0; --i)
	{
		if (tableNamesFromText.contains(tables()->at(i)->name()))
		{
			tableNamesFromText.removeOne(tables()->at(i)->name()); // There should be only one, we removed duplicates
			if (tables()->at(i)->rowCount()!= mapRows.value(tables()->at(i)->name()) ||
				 tables()->at(i)->columnCount() != mapColumns.value(tables()->at(i)->name()))
			{
				tables()->at(i)->resize(mapRows.value(tables()->at(i)->name()), mapColumns.value(tables()->at(i)->name()));
			}

		}
		else tables()->remove(i);
	}

	// Add new tables
	for (auto newTable : tableNamesFromText)
		tables()->append(new CommentTable(nullptr, newTable, mapRows.value(newTable), mapColumns.value(newTable)));
}


} /* namespace Comments */
