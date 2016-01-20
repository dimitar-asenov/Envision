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

#include "VComment.h"
#include "VCommentBrowser.h"
#include "VCommentImage.h"
#include "VCommentDiagram.h"
#include "VCommentFreeNode.h"
#include "VCommentTable.h"
#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(VComment, "item")

VComment::VComment(Item* parent, NodeType* node) : Super(parent, node, itemStyles().get())
{
	editing_ = node->lines()->size() == 0 || (node->lines()->size() == 1 && node->lines()->at(0)->get().isEmpty());
	parseLines();
}

void VComment::determineChildren()
{
	//TODO: VComment currently does not call parseLines() when the tree changes. Thus operations like Undo or other
	//indirect ways to edit the comments, do not refresh the comment's appearance. We must call parseLines somewhere in
	//the update loop. The problem is that we must also make sure not to completely recreate all visual objects as we
	//do currently in parseLines. It is necessary to synchronize all objects and let them update if possible.
	Super::determineChildren();
}

// split up user-provided text into single elements
void VComment::parseLines()
{
	commentElements_.clear();
	bool isHTML = false;

	QStringList linesOfCurrentElement;
	QSet<QString> diagramNames{};
	QSet<QString> codeNames{};
	QSet<QString> tableNames{};
	int listCount = -1;
	int lineNumber = -1;
	QSize htmlSize;

	bool previousWasAPlainLine = false;
	for (auto nodeLine : *node()->lines())
	{
		QString line = nodeLine->get();
		++lineNumber;

		//************************************************************************
		// Bullet lists
		//************************************************************************
		// is this a new enumeration item?
		if (line.startsWith(" * "))
		{
			listCount++;
			// does this create a new list?
			if (listCount == 0) linesOfCurrentElement << "<ul><li>" << line.mid(3);
			// otherwise, just add another list item
			else linesOfCurrentElement << "</li><li>" << line.mid(3);

			continue;
		}

		// or is this extending an existing list?
		if (line.startsWith("   ") && listCount > -1)
		{
			linesOfCurrentElement << "<br/>&nbsp;&nbsp;" << line.mid(3);
			continue;
		}

		// if this is not an enumeration item, reset listCount
		if (listCount > -1)
		{
			linesOfCurrentElement << "</li></ul>";
			listCount = -1;
		}

		//************************************************************************
		// Embedded HTML
		//************************************************************************
		// is this HTML?
		if (line.startsWith("<html") && line.right(1) == ">" && (line.mid(5, 1) == " " || line.mid(5, 1) == ">"))
		{
			createTextualCommentElement(linesOfCurrentElement); // Flush any existing content
			isHTML = true;

			// invalidate htmlSize
			htmlSize.setWidth(-1);

			auto mid = line.mid(5+1, line.size()-5-1-1);
			if (line.mid(5, 1) == " " && mid.size() > 3)
				htmlSize = parseSize(mid);

			continue;
		}

		if (isHTML)
		{
			if (line == "</html>")
			{
				isHTML = false;
				auto browser = DCast<VCommentBrowser>(createTextualCommentElement(linesOfCurrentElement, true));

				if (browser != nullptr && htmlSize.isValid())
					browser->updateSize(htmlSize);
			}
			else linesOfCurrentElement << line;

			continue;
		}

		//************************************************************************
		// Lines
		//************************************************************************
		QRegExp rx("^={3,}|-{3,}|\\.{3,}$");

		if (rx.exactMatch(line))
		{
			createTextualCommentElement(linesOfCurrentElement); // Flush current text

			// A line consists of one of . - = that is repeated three times or more
			// The used character defines the strength of the header, i.e. one of three levels
			QString style;
			switch (line[0].toLatin1())
			{
				case '.': style = "single"; break;
				case '-': style = "double"; break;
				case '=': style = "triple"; break;
				default: Q_ASSERT_X(false, "", "Unknown line character");
			}

			commentElements_.append(new Line{this, Line::itemStyles().get(style)});
			continue;
		}

		//************************************************************************
		// Headers
		//************************************************************************
		rx.setPattern("^(#+)([^#].*)");
		// allow headers h1 to h6
		if (rx.exactMatch(line) && rx.cap(1).length() <= 6)
		{
			QString len = QString::number(rx.cap(1).length());
			linesOfCurrentElement << "<h" + len + ">" + rx.cap(2).simplified() + "</h" + len + ">";

			continue;
		}

		//************************************************************************
		// Diagrams
		//************************************************************************
		// is this a diagram? format: [diagram#diagramName]
		if (line.startsWith("[diagram#") && line.right(1) == "]" && line.size() > 9+1)
		{
			createTextualCommentElement(linesOfCurrentElement); // Flush current text

			QString name = line.mid(9, line.size()-9-1);
			diagramNames << name;

			auto diagram = node()->diagram(name);
			Q_ASSERT(diagram);

			commentElements_.append( new VCommentDiagram{nullptr, diagram} );
			continue;
		}

		//************************************************************************
		// Code
		//************************************************************************
		// is this code? format: [code#codeName]
		if (line.startsWith("[code#") && line.right(1) == "]" && line.size() > 6+1)
		{
			createTextualCommentElement(linesOfCurrentElement); // Flush current text

			QString name = line.mid(6, line.size()-6-1);
			codeNames << name;

			auto code = node()->code(name);
			Q_ASSERT(code);

			commentElements_.append(new VCommentFreeNode{nullptr, code} );

			continue;
		}

		//************************************************************************
		// Table
		//************************************************************************
		// is this a table? format: [table#tableName#rowCount#columnCount] or [table#tableName]
		if (line.startsWith("[table#") && line.right(1) == "]" && line.size() > 7+1)
		{
			createTextualCommentElement(linesOfCurrentElement); // Flush current text

			QString name;
			if (line.count('#')==3)
				name = line.mid(7, (line.indexOf('#', 7)-line.indexOf('#'))-1);
			else
				name =  line.mid(7, line.size()-7-1);
			tableNames << name;

			auto table = node()->table(name);

			if (table != nullptr)
				commentElements_.append(new VCommentTable{nullptr, table} );
			continue;
		}

		//************************************************************************
		// Browsers
		//************************************************************************
		// urls are specified as [browser#http://www.example.com]
		if (line.startsWith("[browser#") && line.right(1) == "]" && line.size() > 9+1)
		{
			createTextualCommentElement(linesOfCurrentElement); // Flush current text

			QString mid = line.mid(9, line.size()-9-1);
			// read width and height, if specified
			auto items = parseMarkdownArguments(mid);
			QString url = items.at(0).second;
			auto browser = new VCommentBrowser{nullptr, QUrl(url)};

			if (items.size() > 1)
			{
				QSize size = parseSize(items.at(1).second);
				browser->updateSize(size);
			}

			commentElements_.append( browser );
			continue;
		}

		//************************************************************************
		// Images
		//************************************************************************
		// images are specified as
		//   [image#/home/user/image.png]
		//   [image#image.png|300x300] to specify a size
		if (line.startsWith("[image#") && line.right(1) == "]" && line.size() > 7+1)
		{
			createTextualCommentElement(linesOfCurrentElement); // Flush current text

			QString mid = line.mid(7, line.size()-7-1);
			// read width and height, if specified
			auto items = parseMarkdownArguments(mid);

			QSize size(0, 0);
			if (items.size() > 1)
				size = parseSize(items.at(1).second);

			auto image = new VCommentImage{nullptr, items.at(0).second, size};
			image->setLineNumber(lineNumber);
			commentElements_.append( image );

			continue;
		}

		//************************************************************************
		// Plain lines
		//************************************************************************
		if (previousWasAPlainLine) linesOfCurrentElement << "<br />";
		linesOfCurrentElement << line;
		previousWasAPlainLine = true;
	}

	createTextualCommentElement(linesOfCurrentElement);
}

QSize VComment::parseSize(const QString& str)
{
	int index = str.indexOf('x');
	bool ok{};

	int width = str.left(index).toInt(&ok);
	if (index > 0 && !ok) return QSize{}; // Invalid size

	int height = str.mid(index+1).toInt(&ok);
	if (index+1 < str.size()-1 && !ok) return QSize{}; // Invalid size

	return QSize(width, height);
}

QList<QPair<QString, QString> > VComment::parseMarkdownArguments(const QString& argString)
{
	// split string on all pipes
	auto lines = argString.split('|');
	// TODO: get rid of escaped pipes \| e.g. in case an url contains one

	auto pairs = QList<QPair<QString, QString>>();
	// read key/value pairs
	QRegExp rx("^[a-zA-Z]{,15}=");
	for (auto line : lines)
	{
		int index = rx.indexIn(line);
		if (index == -1) pairs.append(qMakePair(QString(), line));
		else pairs.append(qMakePair(line.left(index), line.mid(index+1)));
	}

	return pairs;
}

QString VComment::replaceMarkdown(QString str)
{
	QRegExp rx;

	rx.setPattern("\\*\\*([^\\*]+)\\*\\*");
	str.replace(rx, "<b>\\1</b>");

	rx.setPattern("\\*([^\\*]+)\\*");
	str.replace(rx, "<i>\\1</i>");

	return str;
}

Item* VComment::createTextualCommentElement(QStringList& contents, bool asHtml)
{
	Item* item = nullptr;

	if (contents.size() > 0)
	{
		auto joined = contents.join("\n");

		if (asHtml)
		{
			auto browser = new VCommentBrowser{this, joined};
			commentElements_.append(browser);
			item = browser;
		}
		else
		{
			auto text = new Text{this, Text::itemStyles().get("comment"), replaceMarkdown(joined)};
			text->setTextFormat(Qt::RichText);
			commentElements_.append(text);
			item = text;
		}

		contents.clear();
	}

	return item;
}

void VComment::toggleEditing()
{
	if (node()->lines()->size() == 0) editing_ = true;
	else editing_ = !editing_;

	if (!editing_)
	{
		node()->synchronizeDiagramsToText();
		node()->synchronizeCodesToText();
		node()->synchronizeTablesToText();
		parseLines();
	}

	setUpdateNeeded(StandardUpdate);
}

void VComment::initializeForms()
{
	addForm((new SequentialLayoutFormElement{})
				->setVertical()
				->setListOfItems([](Item* i) {
					auto vc = static_cast<VComment*>(i);
					return vc->commentElements_;
				}
	));

	addForm(item<Visualization::VList>(&I::editText_, [](I* v){ return v->node()->lines(); },
			[](I* v){return &v->style()->editList();}));
}

int VComment::determineForm()
{
	return editing_ ? 1 : 0;
}

}
