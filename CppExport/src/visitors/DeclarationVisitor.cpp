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

#include "DeclarationVisitor.h"
#include "ExpressionVisitor.h"
#include "StatementVisitor.h"
#include "ElementVisitor.h"
#include "CppPrintContext.h"
#include "../Config.h"
#include "../ExportHelpers.h"
#include "../SpecialCases.h"

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Declaration.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/NameImport.h"
#include "OOModel/src/declarations/VariableDeclaration.h"
#include "OOModel/src/declarations/ExplicitTemplateInstantiation.h"
#include "OOModel/src/declarations/TypeAlias.h"
#include "OOModel/src/expressions/MetaCallExpression.h"
#include "OOModel/src/declarations/MetaDefinition.h"
#include "OOModel/src/expressions/ArrayInitializer.h"
#include "OOModel/src/expressions/types/AutoTypeExpression.h"

#include "Export/src/tree/SourceDir.h"
#include "Export/src/tree/SourceFile.h"
#include "Export/src/tree/CompositeFragment.h"
#include "Export/src/tree/TextFragment.h"

#include "Comments/src/nodes/CommentNode.h"

using namespace Export;
using namespace OOModel;

namespace CppExport {

SourceFragment* DeclarationVisitor::visit(Declaration* declaration)
{
	if (auto castDeclaration = DCast<Method>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<MetaDefinition>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<Class>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<VariableDeclaration>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<TypeAlias>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<NameImport>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<ExplicitTemplateInstantiation>(declaration)) return visit(castDeclaration);

	notAllowed(declaration);

	auto fragment = new CompositeFragment{declaration};
	*fragment << "Invalid Declaration";
	return fragment;
}

SourceFragment* DeclarationVisitor::visitTopLevelClass(Class* classs)
{
	if (!printContext().isClass()) return visit(classs);

	auto fragment = new CompositeFragment{classs, "spacedSections"};
	*fragment << visit(classs);

	auto filter = [this](Method* method) { return !method->typeArguments()->isEmpty() ||
													(ExportHelpers::isInlineNonPrivateOrNonTemplateClassMethod(method, printContext()) &&
													 !method->modifiers()->isSet(OOModel::Modifier::Default) &&
													 !method->modifiers()->isSet(OOModel::Modifier::Deleted)); };
	QList<Class*> classes{classs};
	while (!classes.empty())
	{
		auto currentClass = classes.takeLast();
		if (!printContext().hasOption(CppPrintContext::PrintMethodBodyInline))
		{
			CppPrintContext methodsPrintContext
			{
				ExportHelpers::parentNamespaceModule(classs),
				printContext().options() | CppPrintContext::PrintMethodBody
			};
			*fragment << list(currentClass->methods(), DeclarationVisitor{methodsPrintContext, data()}, "spacedSections",
									filter);
		}
		CppPrintContext fieldsPrintContext
		{
			ExportHelpers::parentNamespaceModule(classs),
			printContext().options()
		};
		*fragment << list(currentClass->fields(),
								DeclarationVisitor{fieldsPrintContext, data()},
								"spacedSections",
								[](Field* field)
								{
									if (auto parentClass = field->firstAncestorOfType<OOModel::Class>())
										return !parentClass->typeArguments()->isEmpty();
									return false;
								});

		for (auto innerClass : *currentClass->classes())
			classes.append(innerClass);
	}

	return fragment;
}

bool DeclarationVisitor::metaCallFilter(Expression* expression, bool equal)
{
	 auto metaCall = DCast<MetaCallExpression>(expression);
	 auto ooReference = DCast<ReferenceExpression>(metaCall->callee());
	 return ooReference->name().startsWith("DEFINE_") == equal;
}

SourceFragment* DeclarationVisitor::visit(Class* classs)
{
	auto fragment = new CompositeFragment{classs, "sections"};

	if (printContext().isClass())
	{
		auto classFragment = fragment->append(new CompositeFragment{classs});

		*classFragment << compositeNodeComments(classs, "declarationComment");

		if (!classs->typeArguments()->isEmpty())
			*classFragment << list(classs->typeArguments(), ElementVisitor{data()}, "templateArgsList");

		*classFragment << printAnnotationsAndModifiers(classs);

		if (Class::ConstructKind::Class == classs->constructKind()) *classFragment << "class ";
		else if (Class::ConstructKind::Struct == classs->constructKind()) *classFragment << "struct ";
		else if (Class::ConstructKind::Enum == classs->constructKind()) *classFragment << "enum ";
		else notAllowed(classs);

		if (DCast<Module>(classs->firstAncestorOfType<Declaration>()) && classs->typeArguments()->isEmpty())
			*classFragment << ExportHelpers::exportFlag(classs);

		*classFragment << classs->nameNode();

		if (classs->modifiers()->isSet(Modifier::Final))
			*classFragment << " " << new TextFragment{classs->modifiers(), "final"};
		if (!classs->baseClasses()->isEmpty())
		{
			// TODO: inheritance modifiers like private, virtual... (not only public)
			*classFragment << " : ";
			auto baseClassesFragment = classFragment->append(new CompositeFragment{classs->baseClasses(), "comma"});
			for (auto baseClass : *classs->baseClasses())
			{
				auto baseClassFragment = baseClassesFragment->append(new CompositeFragment{baseClass});
				*baseClassFragment << "public " << expression(baseClass);
			}
		}

		auto sections = classFragment->append(new CompositeFragment{classs, "bodySections"});
		*sections << list(classs->metaCalls(), ExpressionVisitor{data()}, "sections",
								[](Expression* expression) { return metaCallFilter(expression, false); });
		*sections << list(classs->enumerators(), ElementVisitor{data()}, "sections");

		*sections << printFriends(classs);

		auto publicSection = addMemberDeclarations(classs, [](Declaration* declaration)
		{
				if (ExportHelpers::isSignalingDeclaration(declaration)) return false;
				return declaration->modifiers()->isSet(Modifier::Public);
		});
		auto signalingSection = addMemberDeclarations(classs, [](Declaration* declaration)
		{
				return ExportHelpers::isSignalingDeclaration(declaration);
		});
		auto protectedSection = addMemberDeclarations(classs, [](Declaration* declaration)
		{
				if (ExportHelpers::isSignalingDeclaration(declaration)) return false;
				return declaration->modifiers()->isSet(Modifier::Protected);
		});
		auto privateSection = addMemberDeclarations(classs, [](Declaration* declaration)
		{
				if (DCast<OOModel::ExplicitTemplateInstantiation>(declaration)) return false;
				if (ExportHelpers::isSignalingDeclaration(declaration)) return false;
				return declaration->modifiers()->isSet(Modifier::Private);
		});

		if (!publicSection->fragments().empty())
		{
			if (!signalingSection->fragments().empty() ||
				 !protectedSection->fragments().empty() ||
				 !privateSection->fragments().empty() ||
				 classs->constructKind() != Class::ConstructKind::Struct)
				*sections << "public:";
			*sections << publicSection;
		}
		if (!signalingSection->fragments().empty()) *sections << "Q_SIGNALS:" << signalingSection;
		if (!protectedSection->fragments().empty()) *sections << "protected:" << protectedSection;
		if (!privateSection->fragments().empty()) *sections << "private:" << privateSection;

		*classFragment << ";";

		SpecialCases::handleQT_Flags(classs, fragment);
	}
	else
	{
		auto sections = fragment->append( new CompositeFragment{classs, "sections"});
		*sections << list(classs->metaCalls(), ExpressionVisitor{data()}, "spacedSections",
								[](Expression* expression) { return metaCallFilter(expression, true); });
		*sections << list(classs->classes(), this, "sections");

		*sections << list(classs->methods(), this, "spacedSections", [this](Method* method)
		{
			return method->typeArguments()->isEmpty() &&
						 !ExportHelpers::isInlineNonPrivateOrNonTemplateClassMethod(method, printContext()) &&
						 !method->modifiers()->isSet(OOModel::Modifier::Abstract) &&
						 !method->modifiers()->isSet(OOModel::Modifier::Deleted) &&
						 !method->modifiers()->isSet(OOModel::Modifier::Default);
		});
		*sections << list(classs->fields(), this, "vertical", [](Field* field)
		{
			if (auto parentClass = field->firstAncestorOfType<OOModel::Class>())
				return parentClass->typeArguments()->isEmpty();
			return true;
		});
	}
	return fragment;
}

template<typename Predicate>
CompositeFragment* DeclarationVisitor::addMemberDeclarations(Class* classs, Predicate filter)
{
	QList<Declaration*> declarations;
	for (auto declaration : *classs->subDeclarations()) if (filter(declaration)) declarations.append(declaration);
	for (auto declaration : *classs->classes()) if (filter(declaration)) declarations.append(declaration);
	for (auto declaration : *classs->methods()) if (filter(declaration)) declarations.append(declaration);

	QHash<Declaration*, QSet<Declaration*>> declarationDependencies;
	for (auto declaration : declarations)
	{
		QSet<Declaration*> dependencies;
		QList<Model::Node*> workList{declaration};
		while (!workList.empty())
		{
			auto currentNode = workList.takeLast();
			if (auto reference = DCast<ReferenceExpression>(currentNode))
				if (auto target = reference->target())
					for (auto other : declarations)
						if (declaration != other)
							if (other->isAncestorOf(target) || other == target)
								dependencies.insert(other);

			if (!DCast<StatementItemList>(currentNode))
				workList << currentNode->children();
		}
		declarationDependencies.insert(declaration, dependencies);
	}

	auto fragment = new CompositeFragment{classs, "accessorSections"};
	CppPrintContext localPrintContext{classs, printContext().options() |
														(printContext().hasOption(CppPrintContext::PrintMethodBodyInline) ?
															CppPrintContext::PrintMethodBody :
															CppPrintContext::None)};
	DeclarationVisitor visitor{localPrintContext, data()};
	for (auto node : ExportHelpers::topologicalSort(declarationDependencies))
		*fragment << visitor.visit(node);
	auto fields = list(classs->fields(), this, "sections", filter);
	if (!fields->fragments().empty()) *fragment << fields;
	return fragment;
}

SourceFragment* DeclarationVisitor::visit(MetaDefinition* metaDefinition)
{

	CppPrintContext::Options printContextOptions = printContext().options();
	std::unique_ptr<Model::Node> dummyNode{};

	/*
	 * In case where the print context node is going to be an actual meta definition context we have to store it
	 * separately from dummyNode to prevent deletion of the meta definition context after dummyNode goes out of scope.
	 */
	Model::Node* printContextNode{};
	if (metaDefinition->name().startsWith("DEFINE_"))
	{
		auto context = DCast<OOModel::Module>(metaDefinition->context());
		if (context && context->name() != "Context")
			printContextNode = context;
		else
		{
			dummyNode.reset(new OOModel::Module());
			printContextNode = dummyNode.get();
		}
		printContextOptions = CppPrintContext::PrintMethodBody;

		if (SpecialCases::hasTemplatePrefixArgument(metaDefinition))
			printContextOptions |= CppPrintContext::PrintTemplatePrefix;
	}
	else
	{
		auto context = DCast<OOModel::Class>(metaDefinition->context());
		if (context && context->name() != "Context")
			printContextNode = context;
		else
		{
			dummyNode.reset(new OOModel::Class());
			printContextNode = dummyNode.get();
		}
		printContextOptions = CppPrintContext::PrintMethodBodyIfNotEmpty;
	}

	CppPrintContext metaDefinitionPrintContext{printContextNode, printContextOptions};
	auto fragment = new CompositeFragment{metaDefinition, "emptyLineAtEnd"};
	*fragment << compositeNodeComments(metaDefinition, "declarationComment");
	auto macro = fragment->append(new CompositeFragment{metaDefinition, "macro"});
	*macro << "#define " << metaDefinition->nameNode();
	if (!metaDefinition->arguments()->isEmpty())
		*macro << list(metaDefinition->arguments(), ElementVisitor{data()}, "argsList");
	auto body = new CompositeFragment{metaDefinition->context(), "macroBody"};
	if (auto context = DCast<Module>(metaDefinition->context()))
		*body << list(context->classes(), DeclarationVisitor{metaDefinitionPrintContext, data()}, "spacedSections");
	else if (auto context = DCast<Method>(metaDefinition->context()))
		*body << list(context->items(), StatementVisitor{metaDefinitionPrintContext, data()}, "sections");
	else if (auto context = DCast<Class>(metaDefinition->context()))
	{
		*body << list(context->metaCalls(), ExpressionVisitor{data()}, "sections");

		DeclarationVisitor declarationVisitor{metaDefinitionPrintContext, data()};
		if (!metaDefinitionPrintContext.isClass())
		{
			*body << list(context->methods(), declarationVisitor, "spacedSections");
			*body << list(context->fields(), declarationVisitor, "spacedSections");
		}
		else
		{
			QList<OOModel::Declaration*> declarations;
			for (auto declaration : *context->methods()) declarations.append(declaration);
			for (auto declaration : *context->fields()) declarations.append(declaration);

			auto printAccessorFragment = [&] (OOModel::Modifier::ModifierFlag modifier, QString accessorLabel)
			{
				auto accessorSection = new CompositeFragment{context, "accessorSections"};
				for (auto declaration : declarations)
					if (declaration->modifiers()->isSet(modifier))
						*accessorSection << declarationVisitor.visit(declaration);
				if (!accessorSection->fragments().empty())
					*body << accessorLabel << accessorSection;
			};

			printAccessorFragment(Modifier::Public, "public:");
			printAccessorFragment(Modifier::Protected, "protected:");
			printAccessorFragment(Modifier::Private, "private:");
		}
	}
	*macro << body;

	return fragment;
}

SourceFragment* DeclarationVisitor::printFriends(Class* classs)
{

	auto fragment = new CompositeFragment{classs->friends(), "sections"};
	for (auto friendDeclaration : *classs->friends())
	{
		if (auto friendClass = DCast<Class>(friendDeclaration))
		{
			// TODO: support type arguments (requires model support)
			auto friendClassFragment = fragment->append(new CompositeFragment{friendClass});
			*friendClassFragment << "friend class " << friendClass->nameNode() << ";";
		}
		else if (auto friendMethod = DCast<Method>(friendDeclaration))
		{
			auto friendMethodFragment = fragment->append(new CompositeFragment{friendMethod});
			*friendMethodFragment << "friend " << visit(friendMethod);
		}
		else
			Q_ASSERT(false);
	}

	return fragment;
}

SourceFragment* DeclarationVisitor::visit(Method* method)
{
	// assertions
	if (method->results()->size() > 1)
		error(method->results(), "Cannot have more than one return value in C++");

	if (!ExportHelpers::shouldExportMethod(method, printContext().isClass(), !printContext().isClass())) return nullptr;

	auto fragment = new CompositeFragment{method};

	// comments
	if (printContext().hasOption(CppPrintContext::PrintMethodBody) ==
		 printContext().hasOption(CppPrintContext::PrintDeclarationCommentWithMethodBody))
		*fragment << compositeNodeComments(method, "declarationComment");

	// template<typename ...>
	if (!printContext().isClass())
	{
		if (printContext().hasOption(CppPrintContext::PrintTemplatePrefix))
			*fragment << "templatePrefix ";

		QList<Class*> parentClasses;
		auto parentClass = method->firstAncestorOfType<Class>();
		while (parentClass)
		{
			parentClasses.prepend(parentClass);
			parentClass = parentClass->firstAncestorOfType<Class>();
		}

		for (auto i = 0; i < parentClasses.size(); i++)
			if (!parentClasses.at(i)->typeArguments()->isEmpty())
				*fragment << list(parentClasses.at(i)->typeArguments(), ElementVisitor{method, data()},
										"templateArgsList");
	}
	if (!method->typeArguments()->isEmpty())
		*fragment << list(method->typeArguments(), ElementVisitor{printContext().node(), data()}, "templateArgsList");

	// private, public, ...
	if (printContext().isClass())
		*fragment << printAnnotationsAndModifiers(method);

	// inline
	if (!printContext().isClass())
		if (method->modifiers()->isSet(Modifier::Inline))
			*fragment << new TextFragment{method->modifiers(), "inline"} << " ";

	// operator
	if (method->methodKind() == Method::MethodKind::Conversion)
	{
		ExportHelpers::printDeclarationQualifier(fragment, printContext().declaration(), method);
		*fragment << "operator ";
	}
	// return type
	if (method->methodKind() != Method::MethodKind::Constructor &&
		 method->methodKind() != Method::MethodKind::Destructor)
	{
		if (!method->results()->isEmpty())
			*fragment << expression(method->results()->at(0)->typeExpression()) << " ";
		else
			*fragment << "void ";
	}

	// export flag
	if (printContext().hasOption(CppPrintContext::IsHeaderPart) && !method->firstAncestorOfType<Class>()
		 && method->typeArguments()->isEmpty())
		*fragment << ExportHelpers::exportFlag(method);

	// method name qualifier
	if (method->methodKind() != Method::MethodKind::Conversion)
		ExportHelpers::printDeclarationQualifier(fragment, printContext().declaration(), method);

	if (method->methodKind() == Method::MethodKind::Destructor && !method->name().startsWith("~")) *fragment << "~";
	if (method->methodKind() == Method::MethodKind::OperatorOverload) *fragment << "operator";
	*fragment << method->nameNode();

	CppPrintContext argumentsPrintContext{printContext().node(), printContext().options() |
																						(printContext().isClass() ?
																							CppPrintContext::PrintDefaultArgumentValues :
																							CppPrintContext::None)};
	*fragment << list(method->arguments(), ElementVisitor{argumentsPrintContext, data()}, "argsList");
	if (method->modifiers()->isSet(Modifier::Const))
		*fragment << " " << new TextFragment{method->modifiers(), "const"};

	if (!printContext().isClass())
		if (!method->memberInitializers()->isEmpty())
			*fragment << " : " << list(method->memberInitializers(), ElementVisitor{method, data()},
												"comma");

	if (!method->throws()->isEmpty())
		*fragment << " throw (" << list(method->throws(), ExpressionVisitor{method, data()},
												  "comma") << ")";

	if (printContext().hasOption(CppPrintContext::PrintMethodBody) ||
		 (printContext().hasOption(CppPrintContext::PrintMethodBodyIfNotEmpty) && !method->items()->isEmpty()))
		*fragment << list(method->items(), StatementVisitor{method, data()}, "body");
	else
	{
		SpecialCases::overrideFlag(method, fragment);

		if (method->modifiers()->isSet(Modifier::Override))
			*fragment << new TextFragment{method->modifiers(), " override"};
		if (method->modifiers()->isSet(Modifier::Default))
			*fragment << new TextFragment{method->modifiers(), " = default"};
		if (method->modifiers()->isSet(Modifier::Deleted))
			*fragment << new TextFragment{method->modifiers(), " = delete"};
		if (method->modifiers()->isSet(Modifier::Abstract))
			*fragment << new TextFragment{method->modifiers(), " = 0"};
		*fragment << ";";
	}

	notAllowed(method->subDeclarations());

	return fragment;
}

SourceFragment* DeclarationVisitor::compositeNodeComments(Model::CompositeNode* compositeNode, const QString& style)
{
	if (auto commentNode = DCast<Comments::CommentNode>(compositeNode->comment()))
	{
		auto commentFragment = new CompositeFragment{commentNode->lines(), style};
		*commentFragment << "/**";
		for (auto line : *(commentNode->lines()))
			*commentFragment << " * " + line->get();
		*commentFragment << " */";
		return commentFragment;
	}

	return nullptr;
}

SourceFragment* DeclarationVisitor::visit(VariableDeclaration* variableDeclaration)
{
	auto field = DCast<Field>(variableDeclaration);

	// A filed might also belong to a Project/Module, representing a global variable.
	bool isClassField = field && field->firstAncestorOfType<Class>();

	// non-static and not constexpr fields are not printed outside of a class
	if (isClassField && !printContext().isClass() &&
		 !field->modifiers()->isSet(Modifier::Static) &&
		 !field->modifiers()->isSet(Modifier::ConstExpr)) return {};

	bool isStaticGlobal = !isClassField && field && field->modifiers()->isSet(Modifier::Static);
	// static global variables are not printed inside the .h file
	if (isStaticGlobal && printContext().hasOption(CppPrintContext::IsHeaderPart) )
		return {};

	auto fragment = new CompositeFragment{variableDeclaration};
	if (printContext().isClass() || isStaticGlobal)
		*fragment << compositeNodeComments(variableDeclaration, "declarationComment");

	if (isClassField && !printContext().isClass())
	{
		// template<typename T...>
		if (printContext().hasOption(CppPrintContext::PrintTemplatePrefix))
			*fragment << "templatePrefix ";
		if (auto parentClass = field->firstAncestorOfType<Class>())
			if (!parentClass->typeArguments()->isEmpty())
				*fragment << list(parentClass->typeArguments(), ElementVisitor{data()}, "templateArgsList");

		if (field->modifiers()->isSet(Modifier::ConstExpr))
			*fragment << printAnnotationsAndModifiers(field);

		// field type
		*fragment << expression(field->typeExpression()) << " ";

		// parent class name qualifier
		ExportHelpers::printDeclarationQualifier(fragment, printContext().declaration(), field);
	}
	else
		*fragment << printAnnotationsAndModifiers(variableDeclaration)
					 << expression(variableDeclaration->typeExpression()) << " ";

	// name
	*fragment << variableDeclaration->nameNode();

	// initial value
	if (variableDeclaration->initialValue() &&
		 (   (isClassField && (!variableDeclaration->modifiers()->isSet(Modifier::Static) || !printContext().isClass()))
		  || isStaticGlobal
		  || (!isClassField && !isStaticGlobal && !printContext().hasOption(CppPrintContext::IsHeaderPart))
		  )
		 )
	{
		// if auto type then print equals ("=")
		if (!DCast<ArrayInitializer>(variableDeclaration->initialValue()) ||
			 DCast<AutoTypeExpression>(variableDeclaration->typeExpression())) *fragment << " = ";
		*fragment << expression(variableDeclaration->initialValue());
	}

	if (!DCast<Expression>(variableDeclaration->parent())) *fragment << ";";
	return fragment;
}

SourceFragment* DeclarationVisitor::printAnnotationsAndModifiers(Declaration* declaration)
{
	auto fragment = new CompositeFragment{declaration, "vertical"};
	if (!declaration->annotations()->isEmpty()) // avoid an extra new line if there are no annotations
		*fragment << list(declaration->annotations(), StatementVisitor{data()}, "vertical");
	auto header = fragment->append(new CompositeFragment{declaration, "space"});

	if (declaration->modifiers()->isSet(Modifier::ConstExpr))
		*header << new TextFragment{declaration->modifiers(), "constexpr"};
	if (declaration->modifiers()->isSet(Modifier::Static))
		*header << new TextFragment{declaration->modifiers(), "static"};
	if (declaration->modifiers()->isSet(Modifier::Final) && !DCast<OOModel::Class>(declaration))
		*header << new TextFragment{declaration->modifiers(), "final"};
	if (declaration->modifiers()->isSet(Modifier::Virtual))
		*header << new TextFragment{declaration->modifiers(), "virtual"};
	if (declaration->modifiers()->isSet(Modifier::Explicit))
		*header << new TextFragment{declaration->modifiers(), "explicit"};

	return fragment;
}

SourceFragment* DeclarationVisitor::visit(NameImport* nameImport)
{
	auto fragment = new CompositeFragment{nameImport};
	notAllowed(nameImport->annotations());

	*fragment << "using ";
	if (nameImport->importAll()) *fragment << "namespace ";
	*fragment << expression(nameImport->importedName()) << ";";
	return fragment;
}

SourceFragment* DeclarationVisitor::visit(ExplicitTemplateInstantiation* explicitTemplateInstantiation)
{
	auto fragment = new CompositeFragment{explicitTemplateInstantiation};

	if (printContext().hasOption(CppPrintContext::PrintExternKeyword))
		*fragment << "extern template class "
					 << ExportHelpers::exportFlag(explicitTemplateInstantiation);
	else
		*fragment << "template class ";

	// reconstruct potentially eliminated prefix for template instantiation special cases
	if (explicitTemplateInstantiation->firstAncestorOfType<OOModel::Class>() &&
		 !explicitTemplateInstantiation->instantiatedClass()->prefix())
		if (auto namespaceModule = ExportHelpers::parentNamespaceModule(explicitTemplateInstantiation))
			*fragment << namespaceModule->name() << "::";

	*fragment << ExpressionVisitor{data()}.visit(explicitTemplateInstantiation->instantiatedClass()) << ";";
	return fragment;
}

SourceFragment* DeclarationVisitor::visit(TypeAlias* typeAlias)
{
	auto fragment = new CompositeFragment{typeAlias};
	*fragment << compositeNodeComments(typeAlias, "declarationComment");

	if (!typeAlias->typeArguments()->isEmpty())
		*fragment << list(typeAlias->typeArguments(), ElementVisitor{data()}, "templateArgsList");

	*fragment << "using " << typeAlias->nameNode() << " = " << expression(typeAlias->typeExpression()) << ";";
	return fragment;
}


}
