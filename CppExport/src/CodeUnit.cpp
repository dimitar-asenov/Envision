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

#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/MetaDefinition.h"
#include "OOModel/src/expressions/MetaCallExpression.h"
#include "OOModel/src/declarations/TypeAlias.h"
#include "OOModel/src/declarations/NameImport.h"
#include "OOModel/src/declarations/ExplicitTemplateInstantiation.h"

namespace CppExport {

CodeUnit::CodeUnit(QString name, Model::Node* node)
	: name_{name}, node_{node}, headerPart_{this}, sourcePart_{this}
{
	Q_ASSERT(!name.isEmpty());
}

void CodeUnit::calculateSourceFragments()
{
	if (auto classs = DCast<OOModel::Class>(node()))
	{
		headerPart()->setFragment(DeclarationVisitor(HEADER_VISITOR).visitTopLevelClass(classs));
		sourcePart()->setFragment(DeclarationVisitor(SOURCE_VISITOR).visitTopLevelClass(classs));
	}
	else if (auto module = DCast<OOModel::Module>(node()))
	{
		auto fragment = new Export::CompositeFragment{node()};
		for (auto metaDefinition : Model::Node::childrenOfType<OOModel::MetaDefinition>(module))
			*fragment << DeclarationVisitor(HEADER_VISITOR).visit(metaDefinition);
		headerPart()->setFragment(fragment);
	}
	else if (auto method = DCast<OOModel::Method>(node()))
	{
		if (method->typeArguments()->isEmpty() &&
			 !method->modifiers()->isSet(OOModel::Modifier::Inline))
		{
			headerPart()->setFragment(DeclarationVisitor(HEADER_VISITOR).visit(method));

			if (!method->modifiers()->isSet(OOModel::Modifier::Abstract) &&
				 !method->modifiers()->isSet(OOModel::Modifier::Deleted))
				sourcePart()->setFragment(DeclarationVisitor(SOURCE_VISITOR).visit(method));
		}
		else
		{
			headerPart()->setFragment(DeclarationVisitor(SOURCE_VISITOR).visit(method));
		}
	}
	else if (auto variableDeclaration = DCast<OOModel::VariableDeclaration>(node()))
	{
		if (variableDeclaration->firstAncestorOfType<OOModel::Class>())
		{
			headerPart()->setFragment(DeclarationVisitor(HEADER_VISITOR).visit(variableDeclaration));
			sourcePart()->setFragment(DeclarationVisitor(SOURCE_VISITOR).visit(variableDeclaration));
		}
		else
			sourcePart()->setFragment(DeclarationVisitor(HEADER_VISITOR).visit(variableDeclaration));
	}
	else if (auto typeAlias = DCast<OOModel::TypeAlias>(node()))
	{
		headerPart()->setFragment(DeclarationVisitor(HEADER_VISITOR).visit(typeAlias));
	}
	else if (auto metaCall = DCast<OOModel::MetaCallExpression>(node()))
	{
		auto ooReference = DCast<OOModel::ReferenceExpression>(metaCall->callee());
		if (Config::instance().metaCallLocationMap().value(ooReference->name()) != "cpp")
			headerPart()->setFragment(ExpressionVisitor(SOURCE_VISITOR).visit(metaCall));
		else
			sourcePart()->setFragment(ExpressionVisitor(SOURCE_VISITOR).visit(metaCall));
	}
	else if (auto metaDefinition = DCast<OOModel::MetaDefinition>(node()))
	{
		// TODO: add a similar map to the metaCallLocationMap (maybe even unify them?)
		headerPart()->setFragment(DeclarationVisitor(MACRO_VISITOR).visit(metaDefinition));
	}
	else if (auto nameImport = DCast<OOModel::NameImport>(node()))
	{
		headerPart()->setFragment(DeclarationVisitor(HEADER_VISITOR).visit(nameImport));
		sourcePart()->setFragment(DeclarationVisitor(SOURCE_VISITOR).visit(nameImport));
	}
	else if (auto explicitTemplateInstantiation = DCast<OOModel::ExplicitTemplateInstantiation>(node()))
	{
		headerPart()->setFragment(DeclarationVisitor(HEADER_VISITOR).visit(explicitTemplateInstantiation));
		sourcePart()->setFragment(DeclarationVisitor(SOURCE_VISITOR).visit(explicitTemplateInstantiation));
	}
	else
		Q_ASSERT(false);
}

void CodeUnit::calculateDependencies(QList<CodeUnit*>& allUnits)
{
	headerPart()->calculateDependencies(allUnits);
	sourcePart()->calculateDependencies(allUnits);
}

}
