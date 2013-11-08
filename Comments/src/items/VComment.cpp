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

#include "VComment.h"
#include "VCommentBrowser.h"
#include "VCommentImage.h"
#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(VComment, "item")

VComment::VComment(Item* parent, NodeType* node) : Super(parent, node, itemStyles().get())
{
	// import existing diagrams
	for(auto diagram : *node->diagrams())
	{
		diagrams_[diagram->name()] = diagram;
	}

	editing_ = node->lines()->size() == 0 || (node->lines()->size() == 1 && node->lines()->at(0)->get().isEmpty());
	parseLines();
}

// split up user-provided text into single elements
void VComment::parseLines()
{
	clearChildren();
	bool isHTML = false;

	QSet<QString> diagramNames{};
	int listCount = -1;

	for(auto nodeLine : *node()->lines())
	{
		QRegExp rx("^={3,}|-{3,}|\\.{3,}$");
		QString line = nodeLine->get();

		// is this a new enumeration item?
		if(line.left(3) == " * ")
		{
			listCount++;
			// does this create a new list?
			if(listCount == 0)
			{
				pushTextLine("<ol><li>");
				line = line.mid(3);
			}
			// otherwise, just add another list item
			else
			{
				pushTextLine("</li><li>");
				line = line.mid(3);
			}
		}
		// or is this extending an existing list?
		else if(line.left(3) == "   " && listCount > -1)
		{
			line = line.mid(3);
		}
		// if this is not an enumeration item, reset listCount
		else if(listCount > -1 && line.left(3) != " * " && line.left(3) != "   ")
		{
			pushTextLine("</li></ol>");
			listCount = -1;
		}

		// is this HTML?
		if(line == "<html>")
		{
			popLineBuffer();
			isHTML = true;
			continue;
		}
		else if(isHTML)
		{
			if(line == "</html>")
			{
				isHTML = false;
				popLineBuffer(true);
			}
			else
			{
				pushTextLine(line);
			}
			// don't process further
			continue;
		}

		if(rx.exactMatch(line))
		{
			// A line consists of one of . - = that is repeated three times or more
			// The used character defines the strength of the header, i.e. one of three levels
			QString style;
			switch(line[0].toAscii())
			{
				default:
				case '.': style = "single"; break;
				case '-': style = "double"; break;
				case '=': style = "triple"; break;
			}

			addChildItem(new Line(this, Line::itemStyles().get(style)));
			continue;
		}

		// is this a header? replace it right away with the appropriate tag
		rx.setPattern("^(#+)([^#].*)");
		// allow headers h1 to h6
		if(rx.exactMatch(line) && rx.cap(1).length() <= 6)
		{
			QString len = QString::number(rx.cap(1).length());
			pushTextLine("<h" + len + ">" + rx.cap(2).simplified() + "</h" + len + ">");
		}
		// is this a diagram? format: [diagram#diagramName]
		else if(line.left(9) == "[diagram#" && line.right(1) == "]" && line.size() > 9+1)
		{
			QString diagramName = line.mid(9,line.size()-9-1);
			diagramNames << diagramName;

			CommentDiagram* diagram = diagrams_.value(diagramName, nullptr);

			if(diagram == nullptr)
			{
				diagram = new CommentDiagram(nullptr, diagramName);
				diagrams_[diagramName] = diagram;
			}

			auto item = renderer()->render(this, diagram);
			addChildItem(item);
		}
		// urls are specified as [[http://www.google.com]]
		else if(line.left(2) == "[[" && line.right(2) == "]]" && line.size() > 2+2)
		{
			QString mid = line.mid(2, line.size()-2-2);
			// read width and height, if specified
			auto items = parseMarkdownArguments(mid);
			QString url = items->at(0).second;
			auto browser = new VCommentBrowser(this, url);

			if(items->size() > 1)
			{
				QSize size = parseSize(items->at(1).second);
				browser->updateSize(size);
			}

			addChildItem(browser);
			delete items;
		}
		// images are specified as
		//   [image#/home/user/image.png]
		//   [image#image.png|300x300] to specify a size
		else if(line.left(7) == "[image#" && line.right(1) == "]" && line.size() > 7+1)
		{
			QString mid = line.mid(7, line.size()-7-1);
			// read width and height, if specified
			auto items = parseMarkdownArguments(mid);
			QString path = items->at(0).second;
			QSize size(0,0);
			if(items->size() > 1)
				size = parseSize(items->at(1).second);

			addChildItem(new VCommentImage(this, items->at(0).second, size));
			delete items;
		}
		else
		{
			pushTextLine(line);
		}
	}

	popLineBuffer();
	synchroniseDiagrams(diagramNames);
}

void VComment::synchroniseDiagrams(QSet<QString> itemDiagramNames)
{
	// get all diagrams from the node
	auto nodeDiagrams = node()->diagrams();
	// gather all names from the node diagrams for easier comparison
	QSet<QString> nodeDiagramNames{};
	for(auto diagram : *nodeDiagrams)
		nodeDiagramNames << diagram->name();

	// get intersection of two sets
	QSet<QString> intersection(itemDiagramNames);
	intersection.intersect(nodeDiagramNames);

	// new diagrams were already constructed inside of parseLines(),
	// they also need to be added to the model now
	auto newDiagramNames = itemDiagramNames - intersection;

	if(newDiagramNames.size() > 0)
	{
		node()->model()->beginModification(node(), "Adding new diagrams");
		for(auto diagramName : newDiagramNames)
		{
			auto diagram = diagrams_.value(diagramName);
			node()->diagrams()->append(diagram);
		}
		node()->model()->endModification();
	}

	// diagrams that are no longer referenced need to be removed from the model
	auto oldDiagramNames = nodeDiagramNames - intersection;

	if(oldDiagramNames.size() > 0)
	{
		node()->model()->beginModification(node(), "Removing unreferenced diagrams");
		for(auto diagramName : oldDiagramNames)
		{
			auto diagram = diagrams_.value(diagramName);
			node()->diagrams()->remove(diagram);
			diagrams_.remove(diagramName);
		}
		node()->model()->endModification();
	}
}

QMap<QString, CommentDiagram*> VComment::diagrams() const
{
	return diagrams_;
}

QSize VComment::parseSize(const QString& str)
{
	int index = str.indexOf('x');
	bool ok{};

	int width = str.left(index).toInt(&ok);
	if(index > 0 && !ok)
		qDebug() << "Invalid width specified in size string:" << str;

	int height = str.mid(index+1).toInt(&ok);
	if(index+1 < str.size()-1 && !ok)
		qDebug() << "Invalid height specified in size string:" << str;

	return QSize(width, height);
}

QVector<QPair<QString,QString>>* VComment::parseMarkdownArguments(const QString& argString)
{
	// split string on all pipes
	auto lines = argString.split('|');
	// TODO: get rid of escaped pipes \| e.g. in case an url contains one

	auto pairs = new QVector<QPair<QString,QString>>();
	// read key/value pairs
	QRegExp rx("^[a-zA-Z]{,15}=");
	for(auto line : lines)
	{
		int index = rx.indexIn(line);
		if(index == -1)
			pairs->push_back(qMakePair(QString(), line));
		else
			pairs->push_back(qMakePair(line.left(index), line.mid(index+1)));
	}

	return pairs;
}

QString VComment::replaceMarkdown(QString str)
{
	QRegExp rx;

	rx.setPattern("\\*\\*([^\\*]+)\\*\\*");
	str.replace(rx, "<i>\\1</i>");

	rx.setPattern("\\*([^\\*]+)\\*");
	str.replace(rx, "<b>\\1</b>");

	return str;
}

void VComment::pushTextLine(QString text)
{
	lineBuffer_.push_back(text);
}

void VComment::popLineBuffer(bool asHtml)
{
	if(lineBuffer_.size() > 0)
	{
		auto joined = lineBuffer_.join("\n");

		if(asHtml)
		{
			auto browser = new VCommentBrowser(this, joined);
			children_.push_back(browser);
		}
		else
		{
			auto text = new Text(this, Text::itemStyles().get("comment"), replaceMarkdown(joined));
			text->setTextFormat(Qt::RichText);
			children_.push_back(text);
		}

		lineBuffer_.clear();
	}
}

void VComment::addChildItem(Visualization::Item* item)
{
	popLineBuffer();
	children_.push_back(item);
}

void VComment::toggleEditing()
{
	if(node()->lines()->size() == 0)
		editing_ = true;
	else
		editing_ = !editing_;

	if(!editing_)
		parseLines();

	setUpdateNeeded(StandardUpdate);
}

bool VComment::editing() const
{
	return editing_;
}

void VComment::initializeForms()
{
	addForm((new SequentialLayoutFormElement())
				->setVertical()
				->setListOfItems([](Item* i) {
					auto vc = static_cast<VComment*>(i);
					return vc->children_;
				}
	));

	addForm(item(&I::editLabel_, [](I* v){
		return v->node()->lines();
	}));
}

int VComment::determineForm()
{
	return editing_ ? 1 : 0;
}

} /* namespace Comments */
