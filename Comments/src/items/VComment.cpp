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
#include "MarkdownTextItem.h"
#include "VisualizationBase/src/items/Line.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

using namespace Visualization;

namespace Comments {

ITEM_COMMON_DEFINITIONS(VComment, "item")

VComment::VComment(Item* parent, NodeType* node) : Super(parent, node, itemStyles().get())
{
}

QList<Item*> VComment::parse()
{
	QStringList lines = node()->label().split(QRegExp("\\r?\\n"));
	QStringList lineBuffer;
	QList<Item*> result;

	for(int i = 0; i < lines.size(); ++i) {
		QRegExp rx("^={3,}|-{3,}|\\.{3,}$");
		QString *line = &lines[i];

		if(rx.exactMatch(*line))
		{
			// A line consists of one character that is repeated over and over
			// This character defines the strength of the header, i.e. one of three levels
			QString style;
			switch((*line)[0].toAscii())
			{
				case '=': style = "triple"; break;
				case '-': style = "double"; break;
				case '.': style = "single"; break;
				default:
					// TODO: catch impossible cases?
					Q_ASSERT("Something went wrong");
			}

			if(lineBuffer.size() > 0) {
				result.push_back(new MarkdownTextItem(this, MarkdownTextItem::itemStyles().get(), lineBuffer.join("\n")));
				lineBuffer.clear();
			}
			result.push_back(new Line(this, Line::itemStyles().get(style)));
			continue;
		}

		// is this a header? replace it right away with the appropriate tag
		rx.setPattern("^(#+)([^#].*)");
		// allow headers h1 to h6
		if(rx.exactMatch(*line) && rx.cap(1).length() <= 6) {
			QString len = QString::number(rx.cap(1).length());
			lineBuffer.push_back("<h" + len + ">" + rx.cap(2).simplified() + "</h" + len + ">");
		}
		else
		{
			lineBuffer.push_back((*line).simplified());
		}
	}

	if(lineBuffer.size() > 0) {
		result.push_back(new MarkdownTextItem(this, MarkdownTextItem::itemStyles().get(), lineBuffer.join("\n")));
	}

	return result;
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
				->setListOfItems([](Item* i) { return static_cast<VComment*>(i)->parse(); }
	));

	addForm(item(&I::editLabel_, [](I* v){return v->node()->labelNode();}));
}

int VComment::determineForm()
{
	return editing_ ? 1 : 0;
}

} /* namespace Comments */
