/***********************************************************************************************************************
**
** Copyright (c) 2016 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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

#include "ConflictUnitComponent.h"

#include "ListMergeComponentV2.h"

namespace FilePersistence {

bool ConflictUnitComponent::isConflictUnitType(const QString& type)
{
	// TODO: The types below sould be configured by plug-ins, not hardcoded here.
	static QStringList conflictUnitTypes{"Block",
													 "BreakStatement",
													 "CaseStatement",
													 "ContinueStatement",
													 "DeclarationStatement",
													 "ExpressionStatement",
													 "ForEachStatement",
													 "IfStatement",
													 "LoopStatement",
													 "ReturnStatement",
													 "Statement",
													 "SwitchStatement",
													 "TryCatchFinallyStatement",
													 "Class",
													 "Declaration",
													 "ExplicitTemplateInstantiation",
													 "Field",
													 "Method",
													 "Module",
													 "NameImport",
													 "Project",
													 "TypeAlias",
													 "VariableDeclaration",
													 "CommentStatementItem",
													 "CatchClause"};
	return conflictUnitTypes.contains(type) || ListMergeComponentV2::isList(type);
}

void ConflictUnitComponent::run(MergeData& /*mergeData*/)
{}

}
