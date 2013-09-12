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
}

// split up user-provided text into single elements
QList<Item*> VComment::split()
{
	for(int i = 0; i < node()->lines()->size(); ++i)
	{
		QRegExp rx("^={3,}|-{3,}|\\.{3,}$");
		QString line = node()->lines()->at(i)->get();

		if(rx.exactMatch(line))
		{
			// A line consists of one character that is repeated over and over
			// This character defines the strength of the header, i.e. one of three levels
			QString style;
			switch(line[0].toAscii())
			{
				default:
				case '.': style = "single"; break;
				case '-': style = "double"; break;
				case '=': style = "triple"; break;
			}

			addChildItem(new Line(nullptr, Line::itemStyles().get(style)));
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
		// urls are specified as [[http://www.google.com]]
		else if(line.left(2) == "[[" && line.right(2) == "]]" && line.size() > 2+2)
		{
			QString url = line.mid(2, line.size()-2-2);
			addChildItem(new VCommentBrowser(this, url));
		}
		// images are specified as [image#/home/user/image.png]
		else if(line.left(7) == "[image#" && line.right(1) == "]" && line.size() > 7+1)
		{
			QString path = line.mid(7, line.size()-7-1);
			addChildItem(new VCommentImage(this, path));
		}
		else
		{
			pushTextLine(line.simplified());
		}
	}

	popLineBuffer();
	return children_;
}

QString VComment::replaceMarkdown(QString str)
{
	QRegExp rx("\\*\\*([^\\*]+)\\*\\*");
	str.replace(rx, "<i>\\1</i>");

	rx.setPattern("\\*([^\\*]+)\\*");
	str.replace(rx, "<b>\\1</b>");

	return str;
}

void VComment::pushTextLine(QString text)
{
	lineBuffer_.push_back(text);
}

void VComment::popLineBuffer()
{
	if(lineBuffer_.size() > 0)
	{
		auto text = new Text(nullptr, Text::itemStyles().get(), replaceMarkdown(lineBuffer_.join("<br>")));
		text->setTextFormat(Qt::RichText);
		children_.push_back(text);
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
	editing_ = !editing_;
	setUpdateNeeded(StandardUpdate);
}

void VComment::initializeForms()
{
	addForm((new SequentialLayoutFormElement())
				->setVertical()
				->setListOfItems([](Item* i) { return static_cast<VComment*>(i)->split(); }
	));

	addForm(item(&I::editLabel_, [](I* v){return v->node()->lines();}));
}

int VComment::determineForm()
{
	return editing_ ? 1 : 0;
}

} /* namespace Comments */
