/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * TextAndDescription.cpp
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "vis/TextAndDescription.h"

namespace Interaction {

ITEM_COMMON_DEFINITIONS(TextAndDescription, "item")

using namespace Visualization;

TextAndDescription::TextAndDescription(Item* parent, const StyleType* style) :
	Item(parent, style), layout(new SequentialLayout(this, &style->layout())), text( new Text(nullptr, &style->text())),
	description( new Text(layout, &style->description()))
{
	layout->append(text);
	layout->append(description);
}

TextAndDescription::TextAndDescription(const QString& text, const QString& description, const StyleType* style)
	: TextAndDescription(nullptr, style)
{
	setContents(text, description);
}

TextAndDescription::~TextAndDescription()
{
	SAFE_DELETE_ITEM(layout);

	// These will automatically be deleted by layout's destructor
	text = nullptr;
	description = nullptr;
}

bool TextAndDescription::sizeDependsOnParent() const
{
	return true;
}

void TextAndDescription::setContents(const QString& text_, const QString& description_)
{
	text->setText(text_);
	description->setText(description_);
}

void TextAndDescription::determineChildren()
{
}

void TextAndDescription::updateGeometry(int availableWidth, int availableHeight)
{
	Item::updateGeometry(layout, availableWidth, availableHeight);
}

}
