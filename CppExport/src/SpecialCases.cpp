/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#include "SpecialCases.h"

#include "ExportHelpers.h"
#include "CodeComposite.h"
#include "visitors/CppPrintContext.h"
#include "visitors/DeclarationVisitor.h"
#include "visitors/ExpressionVisitor.h"
#include "visitors/ElementVisitor.h"
#include "visitors/StatementVisitor.h"

#include "Export/src/tree/CompositeFragment.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/TypeAlias.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/MetaDefinition.h"
#include "OOModel/src/expressions/MetaCallExpression.h"
#include "OOModel/src/expressions/BooleanLiteral.h"
#include "OOModel/src/expressions/ReferenceExpression.h"

namespace CppExport {

void SpecialCases::handleQT_Flags(OOModel::Class* classs, Export::CompositeFragment* fragment)
{
	auto specialCaseFragment = fragment->append(new Export::CompositeFragment{classs});
	for (auto subDeclaration : *classs->subDeclarations())
		if (auto typeAlias = DCast<OOModel::TypeAlias>(subDeclaration))
			if (auto reference = DCast<OOModel::ReferenceExpression>(typeAlias->typeExpression()))
				if (reference->name() == "QFlags")
					*specialCaseFragment << "Q_DECLARE_OPERATORS_FOR_FLAGS(" << classs->name() << "::"
												<< typeAlias->name() << ")";
}

bool SpecialCases::isTestClass(OOModel::Class* classs)
{
	if (!classs) return false;
	return classs->methods()->size() == 1 &&
			(classs->methods()->first()->name() == "test" || classs->methods()->first()->name() == "init");
}

void SpecialCases::overrideFlag(OOModel::Method* method, Export::CompositeFragment* fragment)
{
	for (auto expression : *method->metaCalls())
		if (auto metaCall = DCast<OOModel::MetaCallExpression>(expression))
			if (auto reference = DCast<OOModel::ReferenceExpression>(metaCall->callee()))
				if (reference->name() == "SET_OVERRIDE_FLAG")
				{
					*fragment << " OVERRIDE";
					return;
				}
}

Export::CompositeFragment* SpecialCases::overrideFlagArgumentTransformation(OOModel::MetaCallExpression* metaCall)
{
	Export::CompositeFragment* fragment{};
	if (auto metaDefinition = metaCall->metaDefinition())
		if (metaDefinition->arguments()->size() == 1 && metaCall->arguments()->size() == 1)
			if (metaDefinition->arguments()->first()->name() == "OVERRIDE")
					if (auto booleanLiteral = DCast<OOModel::BooleanLiteral>(metaCall->arguments()->first()))
					{
						fragment = new Export::CompositeFragment{metaCall->arguments(), "argsList"};
						*fragment << (booleanLiteral->value() ? "override" : "");
					}
	return fragment;
}

bool SpecialCases::hasTemplatePrefixArgument(OOModel::MetaDefinition* metaDefinition)
{
	for (auto argument : *metaDefinition->arguments())
		if (argument->name() == "templatePrefix")
			return true;
	return false;
}

bool SpecialCases::isMainMethod(OOModel::Method* method)
{
	return method && method->symbolName() == "main";
}

bool SpecialCases::isTemplateArgumentNameOnlyDependency(OOModel::ReferenceExpression* parentReference,
																		  OOModel::ReferenceExpression*)
{
	return parentReference->name() == "unique_ptr" || parentReference->name() == "shared_ptr";
}

Export::SourceFragment* SpecialCases::printXMacroUsage(OOModel::MetaCallExpression* metaCall)
{
	auto reference = DCast<OOModel::ReferenceExpression>(metaCall->callee());
	Q_ASSERT(reference->name().startsWith("BEGIN_"));

	auto fragment = new Export::CompositeFragment{metaCall, "sections"};
	auto beginCallFragment = fragment->append(new Export::CompositeFragment{metaCall});
	*beginCallFragment << reference->name();
	auto argumentsFragment =
			beginCallFragment->append(new Export::CompositeFragment{metaCall->arguments(), "argsList"});

	CppPrintContext printContext{nullptr};
	for (auto i = 0; i < metaCall->arguments()->size() - 1; i++)
		if (auto expression = DCast<OOModel::Expression>(metaCall->arguments()->at(i)))
			*argumentsFragment << ExpressionVisitor{printContext}.visit(expression);
		else
			Q_ASSERT(false);

	Q_ASSERT(metaCall->arguments()->size() > 0);
	auto childMetaCallsList = DCast<Model::List>(metaCall->arguments()->last());
	auto childMetaCallsFragment = fragment->append(new Export::CompositeFragment{childMetaCallsList, "bodyNoBraces"});
	Q_ASSERT(childMetaCallsList);
	for (auto childMetaCallCandidate : *childMetaCallsList)
	{
		auto childMetaCall = DCast<OOModel::MetaCallExpression>(childMetaCallCandidate);
		Q_ASSERT(childMetaCall);
		*childMetaCallsFragment << ExpressionVisitor{printContext}.visit(childMetaCall);
	}

	*fragment << "END_STANDARD_EXPRESSION_VISUALIZATION";

	return fragment;
}

Export::CompositeFragment* SpecialCases::printPartialBeginMacroSpecialization(OOModel::MetaDefinition* metaDefinition,
																										bool isHeaderFile)
{
	Q_ASSERT(metaDefinition->name().startsWith("BEGIN_") && metaDefinition->metaBindings()->isEmpty());

	auto macro = new Export::CompositeFragment{metaDefinition, "macro"};
	auto fragment = macro->append(new Export::CompositeFragment{metaDefinition, "sections"});
	auto metaDefinitionFragment = fragment->append(new Export::CompositeFragment{metaDefinition});
	*metaDefinitionFragment << "#define " << metaDefinition->name();
	auto argumentsFragment = metaDefinitionFragment->append(new Export::CompositeFragment{metaDefinition->arguments(),
																													  "argsList"});
	CppPrintContext printContext{nullptr};
	for (auto i = 0; i < metaDefinition->arguments()->size() - 1; i++)
		*argumentsFragment << ElementVisitor{printContext}.visit(metaDefinition->arguments()->at(i));

	if (auto metaCall = DCast<OOModel::MetaCallExpression>(metaDefinition->context()->metaCalls()->first()))
	{
		auto metaCallFragment = fragment->append(new Export::CompositeFragment{metaCall});
		*metaCallFragment << ExpressionVisitor{printContext}.visit(metaCall->callee());
		auto argumentsFragment =
				metaCallFragment->append(new Export::CompositeFragment{metaCall->arguments(), "argsList"});
		for (auto i = 0; i < metaCall->arguments()->size() - (isHeaderFile ? 2 : 3); i++)
			if (auto argument = DCast<OOModel::Expression>(metaCall->arguments()->at(i)))
				*argumentsFragment << ExpressionVisitor{printContext}.visit(argument);

		if (!isHeaderFile)
		{
			Q_ASSERT(metaCall->arguments()->size() > 0);
			auto statementList = DCast<Model::List>(metaCall->arguments()->at(metaCall->arguments()->size() - 2));
			Q_ASSERT(statementList);
			auto statementListFragment = fragment->append(new Export::CompositeFragment{statementList, "bodyNoBraces"});
			for (auto statementCandidate : *statementList)
				if (auto statement = DCast<OOModel::StatementItem>(statementCandidate))
					*statementListFragment << StatementVisitor{printContext}.visit(statement);
				else
					Q_ASSERT(false);
		}
	}
	else
		Q_ASSERT(false);

	return macro;
}

Export::CompositeFragment* SpecialCases::printPartialBeginMacroBase(OOModel::MetaDefinition* metaDefinition,
																						  bool isHeaderFile)
{
	Q_ASSERT(metaDefinition->name().startsWith("BEGIN_") && !metaDefinition->metaBindings()->isEmpty());

	auto fragment = new Export::CompositeFragment{metaDefinition, "macro"};
	auto context = DCast<OOModel::Module>(metaDefinition->context());
	auto classs = context->classes()->first();
	CppPrintContext printContext{isHeaderFile ? classs : nullptr, isHeaderFile ? CppPrintContext::IsHeaderPart :
																										  CppPrintContext::None
																					  | CppPrintContext::XMacro};
	auto metaDefinitionFragment = fragment->append(new Export::CompositeFragment{metaDefinition});
	*metaDefinitionFragment << "#define " << metaDefinition->name();
	auto argumentsFragment = metaDefinitionFragment->append(new Export::CompositeFragment{metaDefinition->arguments(),
																													  "argsList"});
	for (auto i = 0; i < metaDefinition->arguments()->size() - (isHeaderFile ? 2 : 3); i++)
		*argumentsFragment << ElementVisitor{printContext}.visit(metaDefinition->arguments()->at(i));
	*metaDefinitionFragment << "\n";
	*fragment << DeclarationVisitor{printContext}.visit(classs);

	return fragment;
}

Export::SourceFragment* SpecialCases::includeXMacroData(CodeComposite* codeComposite,
																					Export::SourceFragment* baseFragment,
																					bool isSourceFile)
{
	auto fragment = new Export::CompositeFragment{baseFragment->node(), "sections"};
	*fragment << baseFragment
				 << "\n"
				 << "namespace OOVisualization {"
				 << "#include \"StandardExpressionDefinitions.h\""
				 << "}";
	for (auto unit : codeComposite->units())
		if (auto metaDefinition = DCast<OOModel::MetaDefinition>(unit->node()))
			if (metaDefinition->name().startsWith("BEGIN_") || (metaDefinition->name().endsWith("_CPP") == isSourceFile))
				*fragment << "#undef " + ExportHelpers::strip_CPPFromName(metaDefinition);
	return fragment;
}

}
