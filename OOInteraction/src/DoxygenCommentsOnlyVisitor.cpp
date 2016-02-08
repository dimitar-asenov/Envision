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

#include "DoxygenCommentsOnlyVisitor.h"
#include "OOModel/src/declarations/Declaration.h"
#include "OOModel/src/declarations/Method.h"
#include "VisualizationBase/src/renderer/ModelRenderer.h"
#include "Comments/src/nodes/CommentNode.h"

using namespace OOModel;
using namespace Comments;

namespace OOInteraction {

void DoxygenCommentsOnlyVisitor::init()
{
	Visitor::addType<Declaration>( [](DoxygenCommentsOnlyVisitor* v, Declaration* t) -> QString
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

	Visitor::addType<Method>( [](DoxygenCommentsOnlyVisitor* v, Method* t) -> QString
	{
		QString res = "";
		bool commentsAvailable = t->comment();
		for (auto node : *t->arguments()) commentsAvailable = commentsAvailable || node->comment();
		for (auto node : *t->results()) commentsAvailable = commentsAvailable || node->comment();
		if (commentsAvailable)
		{
			res += DOXY_START;
			if (t->comment() != nullptr)
			{
				res += v->visit(t->comment());
			}
			for (auto node : *t->arguments())
			{
				if (node->comment() != nullptr)
				{
					res += "\\param " + node->name() + "\n";
					res += "\\parblock\n";
					res += v->visit(node->comment());
					res += "\\endparblock\n";
				}
			}
			for (auto node : *t->results())
			{
				if (node->comment() != nullptr)
				{
					res += "\\result\n";
					res += v->visit(node->comment());
				}
			}
			res += DOXY_END;
		}
		return res;
	});

	Visitor::addType<CommentNode>( [](DoxygenCommentsOnlyVisitor* v, CommentNode* t) -> QString
	{
		QString res = "";
		int listCount = -1;
		QString inlineHTML = "";
		for (auto line : *t->lines())
		{
			if (line->get().startsWith(" * "))
			{
				listCount++;
				if (listCount == 0)
					res += "<ul>\n<li>" + line->get().mid(3);
				else
					res += "</li>\n<li>" + line->get().mid(3);

				continue;
			}
			if (line->get().startsWith("   ") && listCount > -1)
			{
				res += "<br/>&nbsp;&nbsp;" + line->get().mid(3);
				continue;
			}
			if (listCount > -1)
			{
				listCount = -1;
				res += "</li>\n</ul>\n";
			}
			static QRegExp headerRegexp{"^(#+)([^#].*)"};
			if (headerRegexp.exactMatch(line->get()) && headerRegexp.cap(1).length() <= 6)
			{
				QString len = QString::number(headerRegexp.cap(1).length());
				res += "<h" + len + ">" + headerRegexp.cap(2).simplified() + "</h" + len + ">\n";

				continue;
			}
			static QRegExp lineRegexp{"^={3,}|-{3,}|\\.{3,}$"};
			if (lineRegexp.exactMatch(line->get()))
			{
				res += "\\htmlonly\n";
				res += "<hr style=\"";
				switch (line->get()[0].toLatin1())
				{
					case '.': res += "height:1px; "; break;
					case '-': res += "height:2px; "; break;
					case '=': res += "height:4px; "; break;
				}
				res += "border-color:black; background-color:black\">\n";
				res += "\\endhtmlonly\n";
				continue;
			}
			if (line->get().startsWith("<html") && line->get().right(1) == ">" &&
				 (line->get().mid(5, 1) == " " || line->get().mid(5, 1) == ">"))
			{
				inlineHTML += line->get() + "\n";
				continue;
			}
			if (inlineHTML != "")
			{
				inlineHTML += line->get() + "\n";
				if (line->get() == "</html>")
				{
					res += "\\htmlonly\n";
					res += inlineHTML;
					res += "\\endhtmlonly\n";
					inlineHTML = "";
				}
				continue;
			}
			if (line->get().startsWith("[browser#") && line->get().right(1) == "]" && line->get().size() > 9+1)
			{
				QString url = line->get().mid(9, line->get().size()-9-1);
				res += "\\htmlonly\n";
				res += "<iframe src=\"" + url + "\" width=\"768\" height=\"432\">\n";
				res += "<p>your browser does not support iframes</p>";
				res += "</iframe>\n";
				res += "\\endhtmlonly\n";
				continue;
			}
			if (line->get().startsWith("[diagram#") && line->get().right(1) == "]" && line->get().size() > 9+1)
			{
				QString name = line->get().mid(9, line->get().size()-9-1);
				res += v->visit(t->diagram(name)) + "\n";
				continue;
			}
			if (line->get().startsWith("[code#") && line->get().right(1) == "]" && line->get().size() > 6+1)
			{
				QString name = line->get().mid(6, line->get().size()-6-1);
				res += v->visit(t->code(name)) + "\n";
				continue;
			}
			if (line->get().startsWith("[table#") && line->get().right(1) == "]" && line->get().size() > 7+1)
			{
				QString name;
				if (line->get().count('#')==3)
					name = line->get().mid(7, (line->get().indexOf('#', 7)-line->get().indexOf('#'))-1);
				else
					name =  line->get().mid(7, line->get().size()-7-1);
				res += v->visit(t->table(name)) + "\n";
				continue;
			}
			if (line->get().startsWith("[image#") && line->get().right(1) == "]" && line->get().size() > 7+1)
			{
				QString path;
				if (line->get().contains('|'))
					path = line->get().mid(7, (line->get().indexOf('|')-7));
				else
					path =  line->get().mid(7, line->get().size()-7-1);

				QFile file{path};
				QString fileName = file.fileName().section("/", -1, -1);
				file.copy("doxygen/html/images/" + fileName);
				res += QString{"<img src=images/" + fileName + ">"};
				continue;
			}
			res += line->get() + "<br>\n";
		}
		if (listCount > -1)
		{
			listCount = -1;
			res += "</li>\n</ul>\n";
		}
		return replaceMarkdown(res);
	});

	Visitor::addType<CommentDiagram>( [](DoxygenCommentsOnlyVisitor*, CommentDiagram* t) -> QString
	{
		QString imageName = "diagram_" + t->name();
		if (USE_SVG)
		{
			imageName += ".svg";
			Visualization::ModelRenderer::renderToSVG(t, QDir::currentPath() + "/doxygen/html/images/" + imageName);
		}
		else
		{
			imageName += ".png";
			auto anImage = Visualization::ModelRenderer::renderToImage(t);
			anImage.save(QDir::currentPath() + "/doxygen/html/images/" + imageName);
		}

		return QString{"<img src=images/" + imageName + ">"};
	});

	Visitor::addType<CommentFreeNode>( [](DoxygenCommentsOnlyVisitor* v, CommentFreeNode* t) -> QString
	{
		if (DCast<CommentText>(t->node()) || DCast<CommentNode>(t->node()))
			return v->visit(t->node());
		else
		{
			QString imageName =  "freenode_" + t->name();
			if (USE_SVG)
			{
				imageName += ".svg";
				Visualization::ModelRenderer::renderToSVG(t, QDir::currentPath() + "/doxygen/html/images/" + imageName);
			}
			else
			{
				imageName += ".png";
				auto anImage = Visualization::ModelRenderer::renderToImage(t);
				anImage.save(QDir::currentPath() + "/doxygen/html/images/" + imageName);
			}
			return QString{"<img src=images/" + imageName + ">"};
		}
	});

	Visitor::addType<CommentText>( [](DoxygenCommentsOnlyVisitor*, CommentText* t) -> QString
	{
		return t->get();
	});

	Visitor::addType<CommentTable>( [](DoxygenCommentsOnlyVisitor* v, CommentTable* t) -> QString
	{
		QString res = "<table>\n";
		for (int x = 0; x < t->rowCount(); x++)
		{
			res += "<tr>";
			for (int y = 0; y < t->columnCount(); y++)
			{
				if (x == 0)
					res += "<th>" + v->visit(t->nodeAt(x, y)) + "</th>";
				else
					res += "<td>" + v->visit(t->nodeAt(x, y)) + "</td>";
			}
			res += "</tr>\n";
		}
		res += "</table>";
		return res;
	});
}

QString DoxygenCommentsOnlyVisitor::replaceMarkdown(QString str)
{
	static QRegExp boldRegexp {"\\*\\*([^\\*]+)\\*\\*"};
	str.replace(boldRegexp, "<b>\\1</b>");

	static QRegExp italicRegexp {"\\*([^\\*]+)\\*"};
	str.replace(italicRegexp, "<i>\\1</i>");

	return str;
}

const QString DoxygenCommentsOnlyVisitor::DOXY_START = QString{"/**\n"};
const QString DoxygenCommentsOnlyVisitor::DOXY_END = QString{" */\n"};

}
