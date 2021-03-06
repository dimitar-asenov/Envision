/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "CodeUnit.h"

#include "Config.h"
#include "visitors/DeclarationVisitor.h"
#include "visitors/ExpressionVisitor.h"
#include "visitors/CppPrintContext.h"
#include "SpecialCases.h"
#include "ExportHelpers.h"

#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/MetaDefinition.h"
#include "OOModel/src/expressions/MetaCallExpression.h"
#include "OOModel/src/declarations/TypeAlias.h"
#include "OOModel/src/declarations/NameImport.h"
#include "OOModel/src/declarations/ExplicitTemplateInstantiation.h"

namespace CppExport {

CodeUnit::CodeUnit(QString name, Model::Node* node) : name_{name}, node_{node}
{
	Q_ASSERT(!name.isEmpty());
}

void CodeUnit::calculateSourceFragments()
{
	OOModel::Declaration* printContextDeclaration = ExportHelpers::firstValidAncestorPrintContext(node());

	if (auto classs = DCast<OOModel::Class>(node()))
	{
		if (SpecialCases::isTestClass(classs))
		{
			CppPrintContext headerPartPrintContext{classs, CppPrintContext::IsHeaderPart |
																			CppPrintContext::PrintMethodBodyInline |
																			(isTemplateImplementationSeparateFile() ?
																				CppPrintContext::IsTemplateImplementationSeparateFile :
																				CppPrintContext::None)};
			sourcePart()->setFragment(DeclarationVisitor{headerPartPrintContext}.visitTopLevelClass(classs));
		}
		else
		{
			CppPrintContext headerPartPrintContext{classs, CppPrintContext::IsHeaderPart |
																			(isTemplateImplementationSeparateFile() ?
																				CppPrintContext::IsTemplateImplementationSeparateFile :
																				CppPrintContext::None)};
			headerPart()->setFragment(DeclarationVisitor{headerPartPrintContext}.visitTopLevelClass(classs));

			CppPrintContext sourcePartPrintContext{printContextDeclaration, CppPrintContext::PrintMethodBody |
																				(isTemplateImplementationSeparateFile() ?
																					CppPrintContext::IsTemplateImplementationSeparateFile :
																					CppPrintContext::None)};
			sourcePart()->setFragment(DeclarationVisitor{sourcePartPrintContext}.visitTopLevelClass(classs));
		}
	}
	else if (auto module = DCast<OOModel::Module>(node()))
	{
		CppPrintContext headerPartPrintContext{module, CppPrintContext::IsHeaderPart};
		auto fragment = new Export::CompositeFragment{node()};
		for (auto metaDefinition : Model::Node::childrenOfType<OOModel::MetaDefinition>(module))
			*fragment << DeclarationVisitor{headerPartPrintContext}.visit(metaDefinition);
		headerPart()->setFragment(fragment);
	}
	else if (auto method = DCast<OOModel::Method>(node()))
	{
		if (method->typeArguments()->isEmpty() && !method->modifiers()->isSet(OOModel::Modifier::Inline))
		{
			if (!printInCppOnly(method))
			{
				CppPrintContext headerPartPrintContext{printContextDeclaration, CppPrintContext::IsHeaderPart};
				headerPart()->setFragment(DeclarationVisitor{headerPartPrintContext}.visit(method));
			}

			if (!method->modifiers()->isSet(OOModel::Modifier::Abstract) &&
				 !method->modifiers()->isSet(OOModel::Modifier::Deleted))
			{
				CppPrintContext::Options printContextOptions = CppPrintContext::PrintMethodBody;
				if (printInCppOnly(method)) printContextOptions |= CppPrintContext::PrintDeclarationCommentWithMethodBody;

				CppPrintContext printContext{printContextDeclaration, printContextOptions};
				sourcePart()->setFragment(DeclarationVisitor{printContext}.visit(method));
			}
		}
		else
		{
			CppPrintContext headerPartPrintContext{printContextDeclaration, CppPrintContext::IsHeaderPart |
																									CppPrintContext::PrintMethodBody};
			headerPart()->setFragment(DeclarationVisitor{headerPartPrintContext}.visit(method));
		}
	}
	else if (auto variableDeclaration = DCast<OOModel::VariableDeclaration>(node()))
	{
		if (printInCppOnly(variableDeclaration))
			sourcePart()->setFragment(DeclarationVisitor{variableDeclaration}.visit(variableDeclaration));
		else
		{
			CppPrintContext headerPartPrintContext{variableDeclaration, CppPrintContext::IsHeaderPart};
			headerPart()->setFragment(DeclarationVisitor{headerPartPrintContext}.visit(variableDeclaration));
			sourcePart()->setFragment(DeclarationVisitor{printContextDeclaration}.visit(variableDeclaration));
		}
	}
	else if (auto typeAlias = DCast<OOModel::TypeAlias>(node()))
	{
		CppPrintContext headerPartPrintContext{typeAlias, CppPrintContext::IsHeaderPart};
		headerPart()->setFragment(DeclarationVisitor{headerPartPrintContext}.visit(typeAlias));
	}
	else if (auto metaCall = DCast<OOModel::MetaCallExpression>(node()))
	{
		auto ooReference = DCast<OOModel::ReferenceExpression>(metaCall->callee());
		if (ooReference->name().startsWith("BEGIN_"))
			headerPart()->setFragment(SpecialCases::printXMacroDataBlock(metaCall));
		else if (ooReference->name().startsWith("DEFINE_"))
			sourcePart()->setFragment(ExpressionVisitor{printContextDeclaration}.visit(metaCall));
		else
		{
			CppPrintContext headerPartPrintContext{printContextDeclaration, CppPrintContext::IsHeaderPart};
			headerPart()->setFragment(ExpressionVisitor{headerPartPrintContext}.visit(metaCall));
		}
	}
	else if (auto metaDefinition = DCast<OOModel::MetaDefinition>(node()))
	{
		if (metaDefinition->name().startsWith("BEGIN_") && metaDefinition->metaBindings()->isEmpty())
		{
			headerPart()->setFragment(SpecialCases::printPartialBeginMacroSpecialization(metaDefinition, true));
			sourcePart()->setFragment(SpecialCases::printPartialBeginMacroSpecialization(metaDefinition, false));
		}
		else if (metaDefinition->name().startsWith("BEGIN_") && !metaDefinition->metaBindings()->isEmpty())
		{
			headerPart()->setFragment(SpecialCases::printPartialBeginMacroBase(metaDefinition, true));
			sourcePart()->setFragment(SpecialCases::printPartialBeginMacroBase(metaDefinition, false));
		}
		else if (metaDefinition->name().endsWith("_CPP"))
			sourcePart()->setFragment(DeclarationVisitor{printContextDeclaration}.visit(metaDefinition));
		else
		{
			CppPrintContext headerPartPrintContext{printContextDeclaration, CppPrintContext::IsHeaderPart};
			headerPart()->setFragment(DeclarationVisitor{headerPartPrintContext}.visit(metaDefinition));
		}
	}
	else if (auto nameImport = DCast<OOModel::NameImport>(node()))
	{
		CppPrintContext headerPartPrintContext{printContextDeclaration, CppPrintContext::IsHeaderPart};
		headerPart()->setFragment(DeclarationVisitor{headerPartPrintContext}.visit(nameImport));
		sourcePart()->setFragment(DeclarationVisitor{printContextDeclaration}.visit(nameImport));
	}
	else if (auto explicitTemplateInstantiation = DCast<OOModel::ExplicitTemplateInstantiation>(node()))
	{
		CppPrintContext headerPartPrintContext{printContextDeclaration, CppPrintContext::IsHeaderPart |
																								CppPrintContext::PrintExternKeyword};
		headerPart()->setFragment(DeclarationVisitor{headerPartPrintContext}.visit(explicitTemplateInstantiation));
		sourcePart()->setFragment(DeclarationVisitor{printContextDeclaration}.visit(explicitTemplateInstantiation));
	}
	else
		Q_ASSERT(false);
}

bool CodeUnit::printInCppOnly(OOModel::Declaration* declaration) const
{
	if (declaration->modifiers()->isSet(OOModel::Modifier::Static) &&
		 declaration->firstAncestorOfType<OOModel::Class>() == nullptr &&
		 (DCast<OOModel::Method>(declaration) || DCast<OOModel::Field>(declaration)))
	return true;

	return SpecialCases::printInCppOnly(declaration);
}

void CodeUnit::calculateDependencies(QList<CodeUnit*>& allUnits)
{
	headerPart()->calculateDependencies(allUnits);
	sourcePart()->calculateDependencies(allUnits);
}

}
