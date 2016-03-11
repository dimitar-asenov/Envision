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

#include "ContractFilter.h"

#include "OOVisualization/src/elements/VStatementItemList.h"

#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/elements/StatementItemList.h"
#include "OOModel/src/statements/ExpressionStatement.h"

#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/expressions/ErrorExpression.h"
#include "OOModel/src/expressions/UnfinishedOperator.h"
#include "OOModel/src/expressions/ReferenceExpression.h"
#include "OOModel/src/typesystem/TypeArgumentBindings.h"

namespace ContractsLibrary {


OOModel::Class* ContractFilter::contractClass_{};

void ContractFilter::showOnlyContractsFilter(OOVisualization::VStatementItemList* list, int& begin, int& end)
{
	if (list->style() != Visualization::VList::itemStyles().get("contracts")) return;

	auto node = static_cast<OOModel::StatementItemList*>(list->node());
	int myEnd = indexOfLastContractStatement(node) + 1;
	if (myEnd < node->size() && isPotentiallyAContractStatement(node->at(myEnd))) ++myEnd;

	if (myEnd < begin) end = begin;
	else if (myEnd < end ) end = myEnd;

	if (end == begin) {
		end = 0;
		begin = 0;
	}
}

void ContractFilter::hideContractsFilter(OOVisualization::VStatementItemList* list, int& begin, int& end)
{
	if (list->style() == Visualization::VList::itemStyles().get("contracts")) return;

	auto node = static_cast<OOModel::StatementItemList*>(list->node());
	int myBegin = indexOfLastContractStatement(node) + 1;

	if (myBegin > end) begin = end;
	else if (myBegin > begin ) begin = myBegin;

	if (end == begin) {
			end = 0;
			begin = 0;
	}
}

int ContractFilter::indexOfLastContractStatement(OOModel::StatementItemList* statements)
{
	for (int i = statements->size()-1; i>=0; --i)
		if (isDefinitelyAContractStatement(statements->at(i))) return i;

	return -1;
}

bool ContractFilter::isDefinitelyAContractStatement(OOModel::StatementItem* statement)
{
	if ( auto sti = DCast<OOModel::ExpressionStatement>(statement) )
		if (auto call = DCast<OOModel::MethodCallExpression>(sti->expression()) )
			if ( auto methodDef = call->methodDefinition({}))
				if ( methodDef->parent()->parent() == contractClass_)
				return true;

	return false;
}

bool ContractFilter::isPotentiallyAContractStatement(OOModel::StatementItem* statement)
{
	if (isDefinitelyAContractStatement(statement)) return true;

	if ( auto sti = DCast<OOModel::ExpressionStatement>(statement) )
	{
		if (sti->expression()->typeId() == OOModel::EmptyExpression::typeIdStatic()) return true;
		if (sti->expression()->typeId() == OOModel::ErrorExpression::typeIdStatic()) return true;
		if (sti->expression()->typeId() == OOModel::UnfinishedOperator::typeIdStatic()) return true;
		if (sti->expression()->typeId() == OOModel::ReferenceExpression::typeIdStatic()) return true;
	}

	return false;
}

}
