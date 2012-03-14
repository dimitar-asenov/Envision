/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * FinishOperator.cpp
 *
 *  Created on: Jan 11, 2012
 *      Author: Dimitar Asenov
 */

#include "expression_editor/tree_builder/FinishOperator.h"

#include "expression_editor/tree_builder/ExpressionTreeBuilder.h"
#include "expression_editor/UnfinishedOperator.h"
#include "expression_editor/ExpressionTreeUtils.h"

#include "InteractionBaseException.h"

namespace Interaction {

void FinishOperator::perform(ExpressionTreeBuilder& tb)
{
	UnfinishedOperator* unf = tb.unfinished().last();
	tb.unfinished().removeLast();

	if (!unf->isComplete()) throw InteractionBaseException("Invalid try to finish an incomplete operator.");

	Operator* finished = unf->createFinished();
	delete ExpressionTreeUtils::replace(tb.top(), unf, finished );
	tb.left() = finished;
}

} /* namespace InteractionBase */
