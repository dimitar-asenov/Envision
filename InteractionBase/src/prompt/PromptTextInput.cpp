/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "PromptTextInput.h"

#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"
#include "VisualizationBase/src/cursor/TextCursor.h"

namespace Interaction {

DEFINE_ITEM_COMMON(PromptTextInput, "item")

PromptTextInput::PromptTextInput(Item* parent, const QString& initialCommandText,
															  const StyleType* style)
	: Super{parent, style}, text_{ new Visualization::Text{this, &style->text()}}
{
	text_->setText(initialCommandText);
	setDefaultMoveCursorProxy(text_);
}

void PromptTextInput::initializeForms()
{
	addForm(item(&I::text_, [](I* v){return &v->style()->text();}));
}

void PromptTextInput::determineChildren()
{
	Super::determineChildren();
	text_->setEditable(true);
}

void PromptTextInput::setSelection(PromptMode::InputSelection selection)
{
	switch (selection){
		case PromptMode::None: return;
		case PromptMode::All:
		{
			text_->moveCursor();
			text_->correspondingSceneCursor<Visualization::TextCursor>()->selectAll();
		} break;
		case PromptMode::AtEnd:
		{
			text_->moveCursor();
			text_->correspondingSceneCursor<Visualization::TextCursor>()->setCaretPosition(text_->text().size());
		} break;
		default:
			Q_ASSERT(false);
	}
}

QString PromptTextInput::text() const
{
	return text_->text();
}

void PromptTextInput::setText(const QString& text)
{
	text_->setText(text);
}

}
