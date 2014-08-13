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

#include "DoxyCommentVisitor.h"
#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Module.h"
#include "VisualizationBase/src/renderer/ModelRenderer.h"
#include "Comments/src/nodes/CommentNode.h"

using namespace OOModel;
using namespace Comments;

namespace OOInteraction {

void DoxyCommentVisitor::init()
{
	Visitor::addType<Project>( [](DoxyCommentVisitor* v, Project* t) -> QString
	{
		QString res = "";
		if (t->comment() != nullptr)
		{
			res += DOXY_START;
			res += v->visit(t->comment());
			res += DOXY_END;
		}
		return res;
	});

	Visitor::addType<Module>( [](DoxyCommentVisitor* v, Module* t) -> QString
	{
		QString res = "";
		if (t->comment() != nullptr)
		{
			res += DOXY_START;
			res += v->visit(t->comment());
			res += DOXY_END;
		}
		return res;
	});

	Visitor::addType<Class>( [](DoxyCommentVisitor* v, Class* t) -> QString
	{
		QString res = "";
		if (t->comment() != nullptr)
		{
			res += DOXY_START;
			res += v->visit(t->comment());
			res += DOXY_END;
		}

		return res;
	});

	Visitor::addType<Method>( [](DoxyCommentVisitor* v, Method* t) -> QString
	{
		QString res = DOXY_START;
		if (t->comment() != nullptr)
		{
			res += v->visit(t->comment());
		}
		for (auto node : *t->arguments())
		{
			if (node->comment() != nullptr)
			{
				res += "\\param " + node->name() + "\n";
				res += v->visit(node->comment());
			}
		}
		res += DOXY_END;
		return res;
	});

	Visitor::addType<CommentNode>( [](DoxyCommentVisitor* v, CommentNode* t) -> QString
	{
		QString res = "";
		for (auto line : *t->lines())
		{
			if (line->get().startsWith(" * ")) // Doxygen doesnt like * for lists, so exchange with -
			{
				res += " - "+ line->get().mid(3) + "\n";
			}
			else if (line->get().startsWith("[browser#") && line->get().right(1) == "]" && line->get().size() > 9+1)
			{
				QString url = line->get().mid(9, line->get().size()-9-1);
				res += "["+ url + "](" + url + ")\n";
			}
			else if (line->get().startsWith("[diagram#") && line->get().right(1) == "]" && line->get().size() > 9+1)
			{
				QString name = line->get().mid(9, line->get().size()-9-1);
				res += v->visit(t->diagram(name)) + "\n\n";
			}
			else if (line->get().startsWith("[code#") && line->get().right(1) == "]" && line->get().size() > 6+1)
			{
				QString name = line->get().mid(6, line->get().size()-6-1);
				res += v->visit(t->code(name)) + "\n\n";
			}
			else if (line->get().startsWith("[table#") && line->get().right(1) == "]" && line->get().size() > 7+1)
			{
				QString name;
				if (line->get().count('#')==3)
					name = line->get().mid(7, (line->get().indexOf('#', 7)-line->get().indexOf('#'))-1);
				else
					name =  line->get().mid(7, line->get().size()-7-1);
				res += v->visit(t->table(name)) + "\n\n";
			}
			else
				res += line->get() + "\n";
		}
		return res;
	});

	Visitor::addType<CommentDiagram>( [](DoxyCommentVisitor*, CommentDiagram* t) -> QString
	{
		QString imageName =  "diagram_" + t->name() +".png";
		auto anItem = Visualization::ModelRenderer::renderToImage(t);
		anItem.save(QDir::currentPath() + "/doxygen/html/images/" + imageName);
		return QString("![](images/" + imageName + ")");
	});

	Visitor::addType<CommentFreeNode>( [](DoxyCommentVisitor* v, CommentFreeNode* t) -> QString
	{
		if (DCast<CommentText>(t->node()))
			return v->visit(t->node());
		else
		{
			QString imageName =  "freenode_" + t->name() +".png";
			auto anItem = Visualization::ModelRenderer::renderToImage(t);
			anItem.save(QDir::currentPath() + "/doxygen/html/images/" + imageName);
			return QString("![](images/" + imageName + ")");
		}
	});

	Visitor::addType<CommentText>( [](DoxyCommentVisitor*, CommentText* t) -> QString
	{
		return t->get();
	});

	Visitor::addType<CommentTable>( [](DoxyCommentVisitor* v, CommentTable* t) -> QString
	{
		QString res = "";
		for (int x = 0; x < t->rowCount(); x++)
		{
			if (x == 1)
			{
				res += "|";
				for (int y = 0; y < t->columnCount(); y++)
				{
					res += "-|";
				}
				res += "\n";
			}
			res += "| ";
			for (int y = 0; y < t->columnCount(); y++)
			{
				res += v->visit(t->nodeAt(x, y)) + " | ";
			}
			if (x != t->rowCount()-1)
				res += "\n";
		}
		return res;
	});

}

const QString DoxyCommentVisitor::DOXY_START = QString("/**\n");
const QString DoxyCommentVisitor::DOXY_END = QString(" */\n");

}
