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

#include "ConsoleOverlay.h"

#include "VisualizationBase/src/items/StaticStyle.h"
#include "VisualizationBase/src/items/Text.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/TextStyle.h"
#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"

namespace OODebug {

ITEM_COMMON_DEFINITIONS(ConsoleOverlay, "item")

ConsoleOverlay::ConsoleOverlay(Visualization::Item* associatedItem, const StyleType* style)
	: Super{{associatedItem}, style}
{
	output_ = new Visualization::Text{this};
	output_->setTextFormat(Qt::RichText);
	// We don't want the text to grab away the buttons, but we want them on the overlay.
	output_->setAcceptedMouseButtons(Qt::NoButton);
	setAcceptedMouseButtons(Qt::AllButtons);

	if (!style->closeIcon().clickHandler())
	{
		style->closeIcon().setClickHandler([](Visualization::Static* staticParent)
		{
			// This indirection is needed since we can't destroy an item while we're in its even handler.
			staticParent->scene()->addPostEventAction(
						[staticParent](){	staticParent->parent()->hide();});
			return true;
		});
	}
}

void ConsoleOverlay::appendText(const QString& text)
{
	Q_ASSERT(output_);
	QString currentText = output_->text();
	output_->setText(currentText.append(QString(text).toHtmlEscaped().replace(QRegExp("\\r?\\n"), "<br>")));
}

void ConsoleOverlay::appendError(const QString& errorText)
{
	Q_ASSERT(output_);
	QString currentText = output_->text();
	QString appendText = "<font color=\"#FF0000\">" +
			errorText.toHtmlEscaped().replace(QRegExp("\\r?\\n"), "<br>") + "</font>";
	output_->setText(currentText.append(appendText));
}

void ConsoleOverlay::initializeForms()
{
	auto header = (new Visualization::GridLayoutFormElement{})
			->setSpacing(3)->setColumnStretchFactor(1, 1)
			->setNoBoundaryCursors([](Item*){return true;})->setNoInnerCursors([](Visualization::Item*){return true;})
			->put(0, 0, item<Visualization::Text>(&I::output_, &StyleType::output))
			->put(2, 0, item<Visualization::Static>(&I::closeIcon_, &StyleType::closeIcon));

	auto container = (new Visualization::GridLayoutFormElement{})
			->put(0, 0, header)
			->put(0, 1, item<Visualization::Item>(&I::content_));

	addForm(container);
}

}
