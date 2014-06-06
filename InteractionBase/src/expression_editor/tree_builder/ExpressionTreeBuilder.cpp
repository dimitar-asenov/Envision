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

#include "expression_editor/tree_builder/ExpressionTreeBuilder.h"

#include "expression_editor/tree_builder/ExpressionTreeBuildInstruction.h"
#include "expression_editor/tree_builder/AddErrorOperator.h"
#include "expression_editor/tree_builder/AddOperator.h"
#include "expression_editor/tree_builder/FinishOperator.h"

namespace Interaction {

ExpressionTreeBuilder::ExpressionTreeBuilder() : top_(nullptr), left_(nullptr)
{
}

Expression* ExpressionTreeBuilder::build(QVector<ExpressionTreeBuildInstruction*> instructions)
{
	Q_ASSERT(!instructions.isEmpty());

	// Concatenate errors in a single node
	for (int i = instructions.size() - 1; i > 0; --i)
	{
		AddErrorOperator* instr = dynamic_cast<AddErrorOperator*> (instructions[i]);
		AddErrorOperator* prev = dynamic_cast<AddErrorOperator*> (instructions[i-1]);

		if (instr && prev)
		{
			prev->setText(prev->text() + instr->text() );
			SAFE_DELETE(instructions[i]);
			instructions.remove(i);

			// The corresponding FinishOperator also needs to be removed
			int num_intermediate_ops = 0;
			for (int x = i; x < instructions.size(); ++x)
			{
				if (dynamic_cast<FinishOperator*> (instructions[x]))
				{
					if (num_intermediate_ops == 0)
					{
						SAFE_DELETE(instructions[x]);
						instructions.remove(x);
						break;
					}
					else --num_intermediate_ops;
				} else if ( dynamic_cast<AddOperator*> (instructions[x]) || dynamic_cast<AddErrorOperator*> (instructions[x]))
				{
					++num_intermediate_ops;
				}
			}
		}
	}

	// Build the tree
	for (ExpressionTreeBuildInstruction* i : instructions)
	{
		i->perform(*this);
		SAFE_DELETE(i);
	}

	return top_;
}


} /* namespace InteractionBase */
