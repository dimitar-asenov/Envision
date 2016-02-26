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

#include "TextAndDescription.h"

namespace Interaction {

DEFINE_ITEM_COMMON(TextAndDescription, "item")

using namespace Visualization;

TextAndDescription::TextAndDescription(Item* parent, const StyleType* style)
	: Super{parent, style}, textVis_{}, descriptionVis_{}
{
}

TextAndDescription::TextAndDescription(const QString& text, const QString& description, const StyleType* style)
	: TextAndDescription{nullptr, style}
{
	setContents(text, description);
}

TextAndDescription::~TextAndDescription()
{
	// These will automatically be deleted by layout's destructor
	textVis_ = nullptr;
	descriptionVis_ = nullptr;
}

bool TextAndDescription::sizeDependsOnParent() const
{
	return true;
}

void TextAndDescription::setContents(const QString& text, const QString& description)
{
	text_ = text;
	description_ = description;
}

void TextAndDescription::determineChildren()
{
	layout()->synchronizeFirst(textVis_, !text_.isEmpty(), &style()->text());
	layout()->synchronizeLast(descriptionVis_, !description_.isEmpty(), &style()->description());

	layout()->setStyle( &style()->layout() );
	if (textVis_)
	{
		textVis_->setStyle( &style()->text() );
		textVis_->setText(text_);
	}
	if (descriptionVis_)
	{
		descriptionVis_->setStyle( &style()->description() );
		descriptionVis_->setText(description_);
	}
}

}
