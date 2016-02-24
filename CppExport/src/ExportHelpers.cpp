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

#include "ExportHelpers.h"

#include "Config.h"
#include "CodeComposite.h"
#include "visitors/CppPrintContext.h"

#include "Export/src/tree/CompositeFragment.h"
#include "OOModel/src/declarations/Declaration.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/expressions/MetaCallExpression.h"
#include "OOModel/src/declarations/NameImport.h"

namespace CppExport {


bool ExportHelpers::shouldExportMethod(OOModel::Method* method, bool isHeaderVisitor, bool isSourceVisitor)
{
	if (isHeaderVisitor)
	{
		auto parentDeclaration = method->firstAncestorOfType<OOModel::Declaration>();
		Q_ASSERT(parentDeclaration);

		for (auto expression : *(parentDeclaration->metaCalls()))
			if (auto metaCall = DCast<OOModel::MetaCallExpression>(expression))
				for (auto generatedMethod : Model::Node::childrenOfType<OOModel::Method>(metaCall->generatedTree()))
					if (methodSignaturesMatch(method, generatedMethod))
						return false;
	}
	if (isSourceVisitor)
		return !isSignalingDeclaration(method);
	return true;
}

bool ExportHelpers::methodSignaturesMatch(OOModel::Method* method, OOModel::Method* other)
{
	// TODO: this method can be made more specific and relocated to generic method overload resolution
	if (method->arguments()->size() != other->arguments()->size() || method->symbolName() != other->symbolName())
		return false;

	for (auto i = 0; i < method->arguments()->size(); i++)
		if (method->arguments()->at(i)->name() != other->arguments()->at(i)->name())
			return false;

	return true;
}

QString ExportHelpers::pluginName(Model::Node* node)
{
	if (auto parentProject = node->firstAncestorOfType<OOModel::Project>())
		return parentProject->name();
	return {};
}

QString ExportHelpers::exportFlag(Model::Node* node)
{
	auto name = pluginName(node);
	if (!name.isEmpty()) return name.toUpper() + "_API ";
	return {};
}

CodeComposite* ExportHelpers::apiInclude(Model::Node* node)
{
	auto name = pluginName(node);
	if (name.isEmpty()) return nullptr;
	return new CodeComposite{name + "/src/" + name.toLower() + "_api"};
}

bool ExportHelpers::isSignalingDeclaration(OOModel::Declaration* declaration)
{
	for (auto expression : *declaration->metaCalls())
		if (auto metaCall = DCast<OOModel::MetaCallExpression>(expression))
			if (auto reference = DCast<OOModel::ReferenceExpression>(metaCall->callee()))
				if (reference->name() == "PREDEF_SIGNAL")
					return true;
	return false;
}

OOModel::Declaration* ExportHelpers::firstValidAncestorPrintContext(Model::Node* node)
{
	Q_ASSERT(node);

	auto parent = node->parent();
	while (parent)
	{
		if (CppPrintContext::isValidPrintContext(parent)) break;
		parent = parent->parent();
	}
	return DCast<OOModel::Declaration>(parent);
}

bool ExportHelpers::printDeclarationQualifier(QualificationType qualification, Export::CompositeFragment* fragment,
															 OOModel::Declaration* from, OOModel::Class* parentClass,
															 Model::Node* to, bool printTypename)
{
	Q_ASSERT(CppPrintContext::isValidPrintContext(from));

	QList<OOModel::Declaration*> printContexts;
	auto printContext = firstValidAncestorPrintContext(to);

	if (parentClass)
	{
		// Check if the parent class, or any of its subclasses is a parent of the target
		// Note that we do this check regardless of whether the qualification is ParentClass or Using so that
		// we can make sure that in the Using case it does not find false qualifiers
		QSet<OOModel::Class*> parentClasses = {parentClass};
		parentClasses.unite(parentClass->allBaseClasses());

		for (auto classs : parentClasses)
			if (classs->isAncestorOf(to))
			{
				printContexts.append(classs);
				break;
			}
	}

	bool hasParentQualifiers = !printContexts.isEmpty();

	if (!hasParentQualifiers)
	{
		while (!printContext->isAncestorOf(from) && printContext != from)
		{
			if (!DCast<OOModel::Method>(printContext)) printContexts.prepend(printContext);
			if (!printContext) break;
			printContext = firstValidAncestorPrintContext(printContext);
		}
	}

	bool isInMacro = from && from->name().isEmpty();

	// The empty from->name() case is to handle the case of methods printed inside macro bodies
	bool isValidParentClassQualification = qualification==QualificationType::ParentClass &&
			( hasParentQualifiers || printContext == from || isInMacro );

	bool isValidUsingQualification = qualification==QualificationType::Using &&
			(!hasParentQualifiers && printContext != from && !isInMacro );

	if (!printContexts.isEmpty() && !isValidParentClassQualification && !isValidUsingQualification)
		printContexts.clear();

	for (auto printContext : printContexts)
	{
		if (printContext)
		{
			auto classs = DCast<OOModel::Class>(printContext);
			if (classs && !classs->typeArguments()->isEmpty())
			{
				if (printTypename) *fragment << "typename ";
				*fragment << printContext->nameNode();

				auto typeArgumentComposite = fragment->append(new Export::CompositeFragment
																			 {
																				 classs->typeArguments(),
																				 "typeArgsList"
																			 });
				for (auto typeArgument : *classs->typeArguments())
					*typeArgumentComposite << typeArgument->nameNode();
			}
			else
				*fragment << printContext->nameNode();
		}
		*fragment << "::";
	}

	return !printContexts.isEmpty();
}

OOModel::Module* ExportHelpers::parentNamespaceModule(Model::Node* node)
{
	if (DCast<OOModel::NameImport>(node)) return nullptr;

	auto parentModule = node->firstAncestorOfType<OOModel::Module>();
	while (parentModule)
	{
		if (parentModule->kind() == OOModel::Module::ModuleKind::Standard) return parentModule;
		parentModule = parentModule->firstAncestorOfType<OOModel::Module>();
	}
	return nullptr;
}

bool ExportHelpers::isInHeader(OOModel::Method* method, CppPrintContext& printContext)
{
	bool isBodyTrivial = method->modifiers()->isSet(OOModel::Modifier::Default) ||
	method->modifiers()->isSet(OOModel::Modifier::Deleted) ||
	method->modifiers()->isSet(OOModel::Modifier::Abstract);

	if (isBodyTrivial) return true;

	bool isInline = method->modifiers()->isSet(OOModel::Modifier::Inline);
	bool isStatic = method->modifiers()->isSet(OOModel::Modifier::Static);
	bool isPrivate = method->modifiers()->isSet(OOModel::Modifier::Private);

	auto parentClass = method->firstAncestorOfType<OOModel::Class>();
	bool hasFriends = parentClass && !parentClass->friends()->isEmpty();

	bool isTemplateMethod = !method->typeArguments()->isEmpty();
	bool isTemplateClass = parentClass && !parentClass->typeArguments()->isEmpty();
	bool isSeparateTemplateImplementation =
			printContext.hasOption(CppPrintContext::IsTemplateImplementationSeparateFile);

	if (isInline && !isTemplateClass && !isTemplateMethod)
	return !isPrivate || hasFriends;

	if (isTemplateMethod && !parentClass) return !isStatic;
	if (isTemplateMethod && parentClass) return true;

	if (isTemplateClass && !isSeparateTemplateImplementation) return true;
	if (isTemplateClass && isSeparateTemplateImplementation)
	{
		if (isInline) return !isPrivate || hasFriends;
		else return false;
	}

	Q_ASSERT(!isInline && !isTemplateMethod && !isTemplateClass);
	return false;
}

QString ExportHelpers::strip_CPPFromName(Model::Node* node)
{
	QString name;
	if (auto metaDefinition = DCast<OOModel::MetaDefinition>(node))
		name = metaDefinition->name();
	else if (auto metaCall = DCast<OOModel::MetaCallExpression>(node))
	{
		auto reference = DCast<OOModel::ReferenceExpression>(metaCall->callee());
		Q_ASSERT(reference);
		name = reference->name();
	}
	else
		Q_ASSERT(false);

	if (name.endsWith("_CPP")) return name.left(name.size() - 4);
	return name;
}

}
