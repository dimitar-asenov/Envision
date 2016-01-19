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
#include "HArrayInitializer.h"

#include "OOVisualization/src/expressions/VArrayInitializer.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "InteractionBase/src/events/SetCursorEvent.h"
#include "VisualizationBase/src/layouts/GridLayout.h"

namespace OOInteraction {

HArrayInitializer::HArrayInitializer() {}

HArrayInitializer* HArrayInitializer::instance()
{
	static HArrayInitializer h;
	return &h;
}

void HArrayInitializer::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	auto vArray = static_cast<OOVisualization::VArrayInitializer*>(target);

	if (vArray->isShownInMatrixForm() && event->modifiers() == Qt::NoModifier
			&& (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter))
	{

		auto focus = vArray->grid()->focusedElementIndex();
		auto newSubArray = new OOModel::ArrayInitializer{};
		auto empty = new OOModel::EmptyExpression{};
		newSubArray->values()->append(empty);

		vArray->node()->beginModification("create condition");
		vArray->node()->values()->insert(focus.y() + 1, newSubArray);
		vArray->node()->endModification();

		vArray->setUpdateNeeded(Visualization::Item::StandardUpdate);
		target->scene()->addPostEventAction( new Interaction::SetCursorEvent{target, empty});

	}
	else HExpression::keyPressEvent(target, event);
}

}
