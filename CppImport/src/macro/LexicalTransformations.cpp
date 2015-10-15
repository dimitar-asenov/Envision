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

#include "LexicalTransformations.h"

#include "MacroExpansions.h"
#include "NodeHelpers.h"

namespace CppImport {

LexicalTransformations::LexicalTransformations(const ClangHelpers& clang, const MacroExpansions& macroExpansions)
	: clang_(clang), macroExpansions_(macroExpansions) {}

bool LexicalTransformations::isConcatenationOrStringification(clang::SourceLocation loc) const
{
	if (loc.isMacroID())
		if (auto immediateExpansion = macroExpansions_.immediateExpansion(loc))
			return clang_.sourceManager()->getImmediateExpansionRange(loc).first !=
					immediateExpansion->range.getBegin();

	return false;
}

clang::SourceRange LexicalTransformations::unexpandedSourceRange(clang::SourceRange range) const
{
	clang::SourceLocation start, end;

	if (isConcatenationOrStringification(range.getBegin()))
		start = clang_.sourceManager()->getImmediateExpansionRange(range.getBegin()).first;
	else
		start = range.getBegin();

	if (isConcatenationOrStringification(range.getEnd()))
		end = clang_.sourceManager()->getImmediateExpansionRange(range.getEnd()).second;
	else
		end = range.getEnd();

	return clang::SourceRange(start, end);
}

QString LexicalTransformations::unexpandedSpelling(clang::SourceRange range) const
{
	auto result = clang_.spelling(unexpandedSourceRange(range));
	while (result.startsWith("\\")) result = result.right(result.length() - 1);

	return result.trimmed();
}

void LexicalTransformations::processDeclaration(clang::Decl* clangAstNode, Model::Node* envisionAstNode)
{
	clang::SourceRange spellingRange = clangAstNode->getSourceRange();

	if (auto funcDecl = clang::dyn_cast<clang::FunctionDecl>(clangAstNode))
	{
		spellingRange = funcDecl->getNameInfo().getSourceRange();

		auto ooMethod = DCast<OOModel::Method>(envisionAstNode);
		Q_ASSERT(ooMethod);

		if (ooMethod->results()->size() > 0)
			processSourceRange(funcDecl->getReturnTypeSourceRange(), ooMethod->results()->first());

		int i = 0;
		for (auto it = funcDecl->param_begin(); it != funcDecl->param_end(); it++)
		{
			auto varDecl = clang::dyn_cast<clang::VarDecl>(*it);
			Q_ASSERT(varDecl);

			auto start = varDecl->getLocation();
			auto end = varDecl->hasInit() ? varDecl->getInit()->getSourceRange().getBegin() :
													  varDecl->getSourceRange().getEnd();

			auto ooVarDecl = DCast<OOModel::VariableDeclaration>(ooMethod->arguments()->at(i));

			processSourceRange(varDecl->getTypeSourceInfo()->getTypeLoc().getSourceRange(),
												ooVarDecl->typeExpression());
			processSourceRange(clang::SourceRange(start, end), ooVarDecl);
			i++;
		}
	}
	else if (auto varDecl = clang::dyn_cast<clang::VarDecl>(clangAstNode))
	{
		auto ooVarDecl = DCast<OOModel::VariableDeclaration>(envisionAstNode);
		Q_ASSERT(ooVarDecl);

		auto start = varDecl->getLocation();
		auto end = varDecl->getSourceRange().getEnd();

		processSourceRange(varDecl->getTypeSourceInfo()->getTypeLoc().getSourceRange(),
											ooVarDecl->typeExpression());

		spellingRange = clang::SourceRange(start, end);
	}
	else if (auto namedDecl = clang::dyn_cast<clang::NamedDecl>(clangAstNode))
	{
		spellingRange = clang::SourceRange(namedDecl->getLocation(), namedDecl->getSourceRange().getEnd());
	}

	processSourceRange(spellingRange, envisionAstNode);
}

void LexicalTransformations::processStatement(clang::Stmt* clangAstNode, Model::Node* envisionAstNode)
{
	if (auto callExpr = clang::dyn_cast<clang::CallExpr>(clangAstNode))
		processSourceRange(callExpr->getCallee()->getSourceRange(),
											envisionAstNode);
	else if (auto constrExpr = clang::dyn_cast<clang::CXXConstructExpr>(clangAstNode))
	{
		processSourceRange(
				clang::SourceRange(constrExpr->getLocStart(),
										 constrExpr->getParenOrBraceRange().getBegin()),
											envisionAstNode);
	}
	else
		processSourceRange(clangAstNode->getSourceRange(), envisionAstNode);
}

void LexicalTransformations::processSourceRange(clang::SourceRange range, Model::Node* original)
{
	if (DCast<OOModel::ReturnStatement>(original)) return;
	if (DCast<OOModel::NewExpression>(original)) return;
	if (DCast<OOModel::BinaryOperation>(original)) return;
	if (DCast<OOModel::IfStatement>(original)) return;

	if (!clang_.isMacroRange(range)) return;

	auto transformed = unexpandedSpelling(range);

	if (DCast<OOModel::CastExpression>(original))
	{
		QRegularExpression regularExpression("^static_cast<\\s*((\\w|\\*|##)+)\\s*>");
		auto match = regularExpression.match(transformed);

		if (match.hasMatch())
			transformed = match.captured(1);
		else
		{
			qDebug() << "unmatched regex in cast expression" << transformed;
			return;
		}
	}
	else if (DCast<OOModel::MethodCallExpression>(original))
	{
		if (transformed.endsWith("(")) transformed = transformed.left(transformed.length() - 1);
	}
	else if (DCast<OOModel::VariableDeclaration>(original))
	{
		QRegularExpression regularExpression("^((\\w|##)+)");
		auto match = regularExpression.match(transformed);

		if (match.hasMatch())
			transformed = match.captured(1);
		else
		{
			qDebug() << "unmatched regex in vardecl" << transformed;
			return;
		}
	}
	else if (DCast<OOModel::Class>(original))
	{
		QRegularExpression regularExpression("^((\\w|##)+)");
		auto match = regularExpression.match(transformed);

		if (match.hasMatch())
			transformed = match.captured(1);
		else
		{
			qDebug() << "unmatched regex in class" << transformed;
			return;
		}
	}

	transformations_.insert(original, transformed);
}

bool LexicalTransformations::contains(clang::SourceRange range1, clang::SourceRange range2) const
{
	auto unexpandedRange1 = unexpandedSourceRange(range1);
	auto unexpandedRange2 = unexpandedSourceRange(range2);

	auto start1 = clang_.sourceManager()->getSpellingLoc(unexpandedRange1.getBegin()).getPtrEncoding();
	auto end1 = clang_.sourceManager()->getSpellingLoc(unexpandedRange1.getEnd()).getPtrEncoding();
	auto start2 = clang_.sourceManager()->getSpellingLoc(unexpandedRange2.getBegin()).getPtrEncoding();

	return start1 <= start2 && start2 <= end1;
}

const QString LexicalTransformations::transformation(Model::Node* node) const
{
	auto it = transformations_.find(node);
	return it != transformations_.end() ? *it : QString();
}

}
