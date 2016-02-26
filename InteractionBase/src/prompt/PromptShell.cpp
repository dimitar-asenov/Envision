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

#include "PromptShell.h"
#include "PromptMode.h"

#include "VisualizationBase/src/declarative/DeclarativeItemDef.h"
#include "VisualizationBase/src/views/MainView.h"
#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;

namespace Interaction {

/**
 * Specifies the minimal height of the commandReceiver item which will make a new prompt appear in the center of the
 * item.
 *
 * If the receiver item's height is less than this number, then the prompt will be placed directly under the item.
 * Otherwise the prompt will be centered inside the item.
 */
constexpr int COMMAND_RECEIVER_ITEM_MIN_PROMPT_CENTER_HEIGHT = 50;

/**
 * The distance between a prompt which is shown under the command receiver item and that item.
 */
constexpr int PROMPT_TO_RECEIVER_DISTANCE = 3;

DEFINE_ITEM_COMMON(PromptShell, "item")

PromptShell::PromptShell(const QString& initialCommandText,
													Prompt::PromptOptions options,
													const StyleType* style)
	: Super{nullptr, style}, inputItem_{Prompt::mode()->createInputItem(initialCommandText)},
	  modeIconStyle_{Prompt::mode()->modeIcon()}, promptOptions_{options}
{
	Q_ASSERT(inputItem_);
	inputItem_->setParentItem(this);
	setDefaultMoveCursorProxy(inputItem_);

	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(ItemIgnoresTransformations);
	setZValue(LAYER_COMMAND);
	setItemCategory(Scene::MenuItemCategory);

	Prompt::commandReceiver()->scene()->addTopLevelItem(this);

	setShellPosition();
}

void PromptShell::initializeForms()
{
	auto icon = item(&I::modeIcon_, [](I* v){return v->modeIconStyle_;});
	auto inputItem = item(&I::inputItem_);
	auto prompt = grid({{icon, inputItem}})->setHorizontalSpacing(5)->setVerticalSpacing(10)
			->setVerticalAlignment(LayoutStyle::Alignment::Center);

	auto errors = (new SequentialLayoutFormElement{})
			->setVertical()->setSpaceBetweenElements(3)
			->setHasCursorWhenEmpty([](auto){return false; })
			->setListOfItems([](Item* i) { return static_cast<PromptShell*>(i)->errors_; });

	addForm(grid({{prompt}, {errors}}));
}

void PromptShell::setShellPosition()
{
	// Set the position
	QPointF shellPos = Prompt::commandReceiver()->mapToScene(0, 0);
	if (Prompt::commandReceiver()->heightInScene() < COMMAND_RECEIVER_ITEM_MIN_PROMPT_CENTER_HEIGHT)
	{
		// Show the prompt under the receiver item.
		shellPos.setY( shellPos.y() + Prompt::commandReceiver()->heightInScene()
							 + PROMPT_TO_RECEIVER_DISTANCE);
	}
	else
	{
		// If the item is rather large show the prompt at the cursor
		shellPos += Prompt::commandReceiverCursorPosition();
	}
	setPos(shellPos);

	// Center the view
	if (promptOptions_.testFlag(Prompt::CenterViewOnPrompt)) centerViewOnShell();
}

void PromptShell::centerViewOnShell() const
{
	for (auto v : Prompt::commandReceiver()->scene()->views())
		if (auto mainView = dynamic_cast<Visualization::MainView*>(v))
		{
			mainView->centerOn(pos());
			break;
		}
}

void PromptShell::setErrors(QList<Item*> errors)
{
	errors_=errors;
	setUpdateNeeded(StandardUpdate);
}

void PromptShell::determineChildren()
{
	Prompt::mode()->onShellUpdate();
	Super::determineChildren();
}

void PromptShell::changeGeometry(int availableWidth, int availableHeight)
{
	if (initialUpdate_)
	{
		// Select the input
		if (promptOptions_.testFlag(Prompt::InputHasHint))
			Prompt::mode()->setSelection(PromptMode::All);
		else
			Prompt::mode()->setSelection(PromptMode::AtEnd);

		initialUpdate_ = false;
	}

	Super::changeGeometry(availableWidth, availableHeight);
}

}
