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

#include "LexicalHelper.h"

#include "ExpansionManager.h"
#include "StaticStuff.h"

namespace CppImport {

LexicalHelper::LexicalHelper(ClangHelper* clang, ExpansionManager* expansionManager)
	: clang_(clang), expansionManager_(expansionManager) {}

bool LexicalHelper::isConcatenationStringifycation(clang::SourceLocation loc)
{
	if (loc.isMacroID())
		if (auto immediateExpansion = expansionManager_->immediateExpansion(loc))
			return clang_->sourceManager()->getImmediateExpansionRange(loc).first !=
					immediateExpansion->range.getBegin();

	return false;
}

clang::SourceRange LexicalHelper::unexpandedSourceRange(clang::SourceRange range)
{
	clang::SourceLocation start, end;

	if (isConcatenationStringifycation(range.getBegin()))
		start = clang_->sourceManager()->getImmediateExpansionRange(range.getBegin()).first;
	else
		start = range.getBegin();

	if (isConcatenationStringifycation(range.getEnd()))
		end = clang_->sourceManager()->getImmediateExpansionRange(range.getEnd()).second;
	else
		end = range.getEnd();

	return clang::SourceRange(start, end);
}

QString LexicalHelper::unexpandedSpelling(clang::SourceRange range)
{
	auto result = clang_->spelling(unexpandedSourceRange(range));
	while (result.startsWith("\\")) result = result.right(result.length() - 1);

	return result.trimmed();
}

void LexicalHelper::correctNode(clang::Decl* clangAstNode, Model::Node* envisionAstNode)
{
	clang::SourceRange spellingRange = clangAstNode->getSourceRange();

	if (auto fDecl = clang::dyn_cast<clang::FunctionDecl>(clangAstNode))
	{
		spellingRange = fDecl->getNameInfo().getSourceRange();

		auto ooMethod = DCast<OOModel::Method>(envisionAstNode);
		Q_ASSERT(ooMethod);

		if (ooMethod->results()->size() > 0)
			correctNode(fDecl->getReturnTypeSourceRange(), ooMethod->results()->first());

		int i = 0;
		for (auto it = fDecl->param_begin(); it != fDecl->param_end(); it++)
		{
			auto varDecl = clang::dyn_cast<clang::VarDecl>(*it);
			Q_ASSERT(varDecl);

			auto start = varDecl->getLocation();
			auto end = varDecl->hasInit() ? varDecl->getInit()->getSourceRange().getBegin() :
													  varDecl->getSourceRange().getEnd();

			auto ooVarDecl = DCast<OOModel::VariableDeclaration>(ooMethod->arguments()->at(i));

			correctNode(varDecl->getTypeSourceInfo()->getTypeLoc().getSourceRange(),
												ooVarDecl->typeExpression());
			correctNode(clang::SourceRange(start, end), ooVarDecl);
			i++;
		}
	}
	else if (auto varDecl = clang::dyn_cast<clang::VarDecl>(clangAstNode))
	{
		auto ooVarDecl = DCast<OOModel::VariableDeclaration>(envisionAstNode);
		Q_ASSERT(ooVarDecl);

		auto start = varDecl->getLocation();
		auto end = varDecl->getSourceRange().getEnd();

		correctNode(varDecl->getTypeSourceInfo()->getTypeLoc().getSourceRange(),
											ooVarDecl->typeExpression());

		spellingRange = clang::SourceRange(start, end);
	}
	else if (auto namedDecl = clang::dyn_cast<clang::NamedDecl>(clangAstNode))
	{
		spellingRange = clang::SourceRange(namedDecl->getLocation(), namedDecl->getSourceRange().getEnd());
	}

	correctNode(spellingRange, envisionAstNode);
}

void LexicalHelper::correctNode(clang::Stmt* clangAstNode, Model::Node* envisionAstNode)
{
	if (auto callExpr = clang::dyn_cast<clang::CallExpr>(clangAstNode))
		correctNode(callExpr->getCallee()->getSourceRange(),
											envisionAstNode);
	else if (auto constrExpr = clang::dyn_cast<clang::CXXConstructExpr>(clangAstNode))
	{
		correctNode(
				clang::SourceRange(constrExpr->getLocStart(),
										 constrExpr->getParenOrBraceRange().getBegin()),
											envisionAstNode);
	}
	else
		correctNode(clangAstNode->getSourceRange(), envisionAstNode);
}

void LexicalHelper::correctNode(clang::SourceRange range, Model::Node* original)
{
	if (DCast<OOModel::ReturnStatement>(original)) return;
	if (DCast<OOModel::NewExpression>(original)) return;
	if (DCast<OOModel::BinaryOperation>(original)) return;
	if (DCast<OOModel::IfStatement>(original)) return;

	if (!clang_->isMacroRange(range)) return;

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

bool LexicalHelper::contains(clang::SourceRange r, clang::SourceRange o)
{
	auto range = unexpandedSourceRange(r);
	auto other = unexpandedSourceRange(o);

	auto s = clang_->sourceManager()->getSpellingLoc(range.getBegin()).getPtrEncoding();
	auto e = clang_->sourceManager()->getSpellingLoc(range.getEnd()).getPtrEncoding();
	auto os = clang_->sourceManager()->getSpellingLoc(other.getBegin()).getPtrEncoding();

	return s <= os && os <= e;
}

void LexicalHelper::applyLexicalTransformations(Model::Node* node, NodeMapping* mapping, QVector<QString> formalArgs)
{
	if (transformations_.contains(mapping->original(node)))
	{
		auto transformed = transformations_.value(mapping->original(node));

		bool containsArg = false;
		for (auto arg : formalArgs)
			if (transformed.contains(arg))
			{
				containsArg = true;
				break;
			}

		if (containsArg)
		{
			if (auto ref = DCast<OOModel::ReferenceExpression>(node))
				ref->setName(transformed);
			else if (auto decl = DCast<OOModel::Class>(node))
				decl->setName(transformed);
			else if (auto decl = DCast<OOModel::Method>(node))
				decl->setName(transformed);
			else if (auto decl = DCast<OOModel::VariableDeclaration>(node))
				decl->setName(transformed);
			else if (auto strLit = DCast<OOModel::StringLiteral>(node))
				strLit->setValue(transformed);
			else if (auto formalResult = DCast<OOModel::FormalResult>(node))
				formalResult->setTypeExpression(StaticStuff::createNameExpressionFromString(transformed));
			else if (auto boolLit = DCast<OOModel::BooleanLiteral>(node))
				replaceWithReference(boolLit, transformed, mapping);
			else if (auto castExpr = DCast<OOModel::CastExpression>(node))
				castExpr->setType(new OOModel::ReferenceExpression(transformed));
			else if (auto castExpr = DCast<OOModel::PointerTypeExpression>(node))
				replaceWithReference(castExpr, transformed, mapping);
			else if (auto methodCall = DCast<OOModel::MethodCallExpression>(node))
			{
				if (auto ref = DCast<OOModel::ReferenceExpression>(methodCall->callee()))
				{
					if (transformed.startsWith("#"))
						replaceWithReference(methodCall, transformed, mapping);
					else
						replaceWithReference(ref, transformed, mapping);
				}
				else
					qDebug() << "Unhandled transformed node type" << node->typeName() << "transformed" << transformed;
			}
			else if (auto templateInst = DCast<OOModel::ExplicitTemplateInstantiation>(node))
			{
				QRegularExpression regEx("((\\w+::)?\\w+<(\\w+::)?\\w+>)$");

				auto m = regEx.match(transformed);
				if (m.hasMatch())
					replaceWithReference(templateInst->instantiatedClass(), m.captured(1), mapping);
				else
					qDebug() << "could not correct explicit template instantiation: " << transformed;
			}
			else
				qDebug() << "Unhandled transformed node type" << node->typeName() << "transformed" << transformed;
		}
	}

	for (auto child : node->children())
		applyLexicalTransformations(child, mapping, formalArgs);
}

void LexicalHelper::replaceWithReference(Model::Node* current, QString replacement, NodeMapping* mapping)
{
	auto newValue = StaticStuff::createNameExpressionFromString(replacement);
	current->parent()->replaceChild(current, newValue);
	mapping->replaceClone(current, newValue);
}

}
