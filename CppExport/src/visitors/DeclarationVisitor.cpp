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
#include "../Config.h"

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
	if (!headerVisitor()) return visit(classs);

	auto fragment = new CompositeFragment{classs, "spacedSections"};
	*fragment << visit(classs);

	auto filter = [](Method* method) { return !method->typeArguments()->isEmpty() ||
															(method->modifiers()->isSet(OOModel::Modifier::Inline) &&
															 !method->modifiers()->isSet(OOModel::Modifier::Default) &&
															 !method->modifiers()->isSet(OOModel::Modifier::Deleted)); };
	QList<Class*> classes{classs};
	while (!classes.empty())
	{
		auto currentClass = classes.takeLast();
		*fragment << list(currentClass->methods(), DeclarationVisitor(SOURCE_VISITOR, data()), "spacedSections", filter);
		*fragment << list(currentClass->fields(), DeclarationVisitor(SOURCE_VISITOR, data()), "spacedSections",
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
	 if (ooReference->name() == "QT_Flags") return false;
	 return (Config::instance().metaCallLocationMap().value(ooReference->name()) == "cpp") == equal;
}

bool DeclarationVisitor::isSignalingDeclaration(Declaration* declaration)
{
	for (auto expression : *declaration->metaCalls())
		if (auto metaCall = DCast<MetaCallExpression>(expression))
			if (auto reference = DCast<ReferenceExpression>(metaCall->callee()))
				if (reference->name() == "PREDEF_SIGNAL")
					return true;
	return false;
}

SourceFragment* DeclarationVisitor::visit(Class* classs)
{
	auto fragment = new CompositeFragment{classs};

	if (sourceVisitor())
	{
		auto sections = fragment->append( new CompositeFragment{classs, "sections"});
		*sections << list(classs->metaCalls(), ExpressionVisitor(data()), "spacedSections",
								[](Expression* expression) { return metaCallFilter(expression, true); });
		*sections << list(classs->classes(), this, "sections");
		*sections << list(classs->methods(), this, "spacedSections", [](Method* method)
		{
			return method->typeArguments()->isEmpty() &&
						 !method->modifiers()->isSet(OOModel::Modifier::Inline) &&
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
	else
	{
		bool friendClass = false;
		if (auto parentClass = classs->firstAncestorOfType<Class>())
			friendClass = parentClass->friends()->isAncestorOf(classs);

		if (!friendClass)
		{
			*fragment << compositeNodeComments(classs, "declarationComment");

			if (!classs->typeArguments()->isEmpty())
				*fragment << list(classs->typeArguments(), ElementVisitor(data()), "templateArgsList");

			*fragment << printAnnotationsAndModifiers(classs);
		}
		else
			*fragment << "friend ";

		if (Class::ConstructKind::Class == classs->constructKind()) *fragment << "class ";
		else if (Class::ConstructKind::Struct == classs->constructKind()) *fragment << "struct ";
		else if (Class::ConstructKind::Enum == classs->constructKind()) *fragment << "enum ";
		else notAllowed(classs);

		auto potentialNamespace = classs->firstAncestorOfType<Module>();
		if (!friendClass && classs->firstAncestorOfType<Declaration>() == potentialNamespace &&
			 classs->typeArguments()->isEmpty())
			*fragment << pluginName(potentialNamespace, classs).toUpper() << "_API ";

		*fragment << classs->nameNode();

		if (!friendClass)
		{
			if (classs->modifiers()->isSet(Modifier::Final))
				*fragment << " " << new TextFragment{classs->modifiers(), "final"};
			if (!classs->baseClasses()->isEmpty())
			{
				// TODO: inheritance modifiers like private, virtual... (not only public)
				auto baseClassesFragment = new CompositeFragment{classs->baseClasses(), "comma"};
				for (auto baseClass : *classs->baseClasses())
				{
					auto baseClassFragment = new CompositeFragment{baseClass};
					*baseClassFragment << "public " << expression(baseClass);
					*baseClassesFragment << baseClassFragment;
				}
				*fragment << " : " << baseClassesFragment;
			}

			auto sections = fragment->append( new CompositeFragment{classs, "bodySections"});
			*sections << list(classs->metaCalls(), ExpressionVisitor(data()), "sections",
									[](Expression* expression) { return metaCallFilter(expression, false); });
			*sections << list(classs->enumerators(), ElementVisitor(data()), "sections");
			*sections << list(classs->friends(), this, "sections");

			auto publicSection = new CompositeFragment{classs, "accessorSections"};
			bool hasPublicSection = addMemberDeclarations(classs, publicSection, [](Declaration* declaration)
			{
					if (isSignalingDeclaration(declaration)) return false;
					return declaration->modifiers()->isSet(Modifier::Public);
			});
			auto signalingSection = new CompositeFragment{classs, "accessorSections"};
			bool hasSignalingSection = addMemberDeclarations(classs, signalingSection, [](Declaration* declaration)
			{
					return isSignalingDeclaration(declaration);
			});
			auto protectedSection = new CompositeFragment{classs, "accessorSections"};
			bool hasProtectedSection = addMemberDeclarations(classs, protectedSection,  [](Declaration* declaration)
			{
					if (isSignalingDeclaration(declaration)) return false;
					return declaration->modifiers()->isSet(Modifier::Protected);
			});
			auto privateSection = new CompositeFragment{classs, "accessorSections"};
			bool hasPrivateSection = addMemberDeclarations(classs, privateSection,  [](Declaration* declaration)
			{
					if (DCast<OOModel::ExplicitTemplateInstantiation>(declaration)) return false;
					if (isSignalingDeclaration(declaration)) return false;
					return !declaration->modifiers()->isSet(Modifier::Public) &&
					!declaration->modifiers()->isSet(Modifier::Protected);
			});

			if (hasPublicSection)
			{
				if (hasSignalingSection || hasProtectedSection || hasPrivateSection ||
					 classs->constructKind() != Class::ConstructKind::Struct)
					*sections << "public:";
				sections->append(publicSection);
			}
			if (hasSignalingSection)
			{
				if (hasPublicSection) *sections << "\n"; // add newline between two accessor sections

				*sections << "Q_SIGNALS:";
				sections->append(signalingSection);
			}
			if (hasProtectedSection)
			{
				if (hasSignalingSection || hasSignalingSection) *sections << "\n"; // add newline between two accessor sections

				*sections << "protected:";
				sections->append(protectedSection);
			}
			if (hasPrivateSection)
			{
				if (hasPublicSection || hasSignalingSection || hasProtectedSection)
					*sections << "\n"; // add newline between two accessor sections

				*sections << "private:";
				sections->append(privateSection);
			}
		}

		*fragment << ";";

		auto qtFlagsComposite = new CompositeFragment{classs, "sections"};
		*qtFlagsComposite << fragment;
		if (isEnumWithQtFlags(classs))
		{
			auto qDeclareFlagsFragment = new CompositeFragment{classs};
			*qDeclareFlagsFragment << "Q_DECLARE_FLAGS(" << classs->name() << "s, " << classs->name() << ")";
			*qtFlagsComposite << qDeclareFlagsFragment;
		}
		else
			for (auto potentialEnumWithQtFlags : Model::Node::childrenOfType<OOModel::Class>(classs))
				if (potentialEnumWithQtFlags != classs && isEnumWithQtFlags(potentialEnumWithQtFlags))
				{
					auto qDeclareOperatorsForFlagsFragment = new CompositeFragment{classs};
					*qDeclareOperatorsForFlagsFragment << "Q_DECLARE_OPERATORS_FOR_FLAGS("
												  << classs->name() << "::" << potentialEnumWithQtFlags->name() << "s)";
					*qtFlagsComposite << qDeclareOperatorsForFlagsFragment;
				}
		return qtFlagsComposite;
	}

	return fragment;
}

bool DeclarationVisitor::isEnumWithQtFlags(OOModel::Class* candidate)
{
	if (candidate->constructKind() == Class::ConstructKind::Enum)
		for (auto entry : *candidate->metaCalls())
			if (auto metaCall = DCast<OOModel::MetaCallExpression>(entry))
				if (auto reference = DCast<OOModel::ReferenceExpression>(metaCall->callee()))
					if (reference->name() == "QT_Flags")
						return true;
	return false;
}

template<typename Predicate>
bool DeclarationVisitor::addMemberDeclarations(Class* classs, CompositeFragment* section, Predicate filter)
{
	auto subDeclarations = list(classs->subDeclarations(), this, "sections", filter);
	auto fields = list(classs->fields(), this, "vertical", filter);
	auto classes = list(classs->classes(), this, "sections", filter);
	auto methods = list(classs->methods(), this, "sections", filter);

	*section << subDeclarations << fields << classes << methods;
	return !subDeclarations->fragments().empty() ||
			 !fields->fragments().empty() ||
			 !classes->fragments().empty() ||
			 !methods->fragments().empty();
}

bool DeclarationVisitor::shouldExportMethod(Method* method)
{
	if (headerVisitor())
	{
		auto parentDeclaration = method->firstAncestorOfType<Declaration>();
		Q_ASSERT(parentDeclaration);

		for (auto expression : *(parentDeclaration->metaCalls()))
			if (auto metaCall = DCast<OOModel::MetaCallExpression>(expression))
				for (auto generatedMethod : Model::Node::childrenOfType<Method>(metaCall->generatedTree()))
					if (methodSignaturesMatch(method, generatedMethod))
						return false;
	}
	if (sourceVisitor())
		return !isSignalingDeclaration(method);
	return true;
}

bool DeclarationVisitor::methodSignaturesMatch(Method* method, Method* other)
{
	// TODO: this method can be made more specific and relocated to generic method overload resolution
	if (method->arguments()->size() != other->arguments()->size() || method->symbolName() != other->symbolName())
		return false;

	for (auto i = 0; i < method->arguments()->size(); i++)
		if (method->arguments()->at(i)->name() != other->arguments()->at(i)->name())
			return false;

	return true;
}

QString DeclarationVisitor::pluginName(Module* namespaceModule, Declaration* declaration)
{
	auto value = Config::instance().exportFlagMap().value(namespaceModule->name() + "/" + declaration->name());
	if (!value.isEmpty()) return value;

	if (namespaceModule->name() == "Model")
		return "ModelBase";
	else if (namespaceModule->name() == "Visualization")
		return "VisualizationBase";
	else if (namespaceModule->name() == "Interaction")
		return "InteractionBase";
	return namespaceModule->name();
}

SourceFragment* DeclarationVisitor::visit(MetaDefinition* metaDefinition)
{
	auto fragment = new CompositeFragment{metaDefinition, "emptyLineAtEnd"};
	auto macro = new CompositeFragment{metaDefinition, "macro"};
	*macro << "#define " << metaDefinition->nameNode();
	*macro << list(metaDefinition->arguments(), ElementVisitor(MACRO_VISITOR, data()), "argsList");
	auto body = new CompositeFragment{metaDefinition->context(), "macroBody"};
	if (auto context = DCast<Module>(metaDefinition->context()))
		*body << list(context->classes(), DeclarationVisitor(MACRO_VISITOR, data()), "spacedSections");
	else if (auto context = DCast<Class>(metaDefinition->context()))
	{
		*body << list(context->metaCalls(), ExpressionVisitor(data()), "sections");
		*body << list(context->methods(), DeclarationVisitor(MACRO_VISITOR, data()), "spacedSections");
	}
	*macro << body;
	*fragment << macro;
	return fragment;
}

SourceFragment* DeclarationVisitor::visit(Method* method)
{
	// assertions
	if (method->results()->size() > 1)
		error(method->results(), "Cannot have more than one return value in C++");

	if (!shouldExportMethod(method)) return nullptr;

	QList<Class*> parentClasses;
	auto parentClass = method->firstAncestorOfType<Class>();
	while (parentClass)
	{
		parentClasses.prepend(parentClass);
		parentClass = parentClass->firstAncestorOfType<Class>();
	}

	auto fragment = new CompositeFragment{method};

	// comments
	if (!sourceVisitor())
		*fragment << compositeNodeComments(method, "declarationComment");

	// template<typename ...>
	if (!headerVisitor())
		if (method->modifiers()->isSet(Modifier::Inline))
			for (auto i = 0; i < parentClasses.size(); i++)
				if (!parentClasses.at(i)->typeArguments()->isEmpty())
					*fragment << list(parentClasses.at(i)->typeArguments(), ElementVisitor(data()), "templateArgsList");
	if (!method->typeArguments()->isEmpty())
		*fragment << list(method->typeArguments(), ElementVisitor(data()), "templateArgsList");

	// friend keyword
	if (!sourceVisitor())
		if (!parentClasses.empty())
			if (parentClasses.last()->friends()->isAncestorOf(method))
				*fragment << "friend ";

	// private, public, ...
	if (!sourceVisitor())
		*fragment << printAnnotationsAndModifiers(method);

	// inline
	if (!headerVisitor())
		if (method->modifiers()->isSet(Modifier::Inline))
			*fragment << new TextFragment{method->modifiers(), "inline"} << " ";

	// operator
	if (method->methodKind() == Method::MethodKind::Conversion)
	{
		if (sourceVisitor())
			if (!parentClasses.empty())
				*fragment << parentClasses.last()->name() << "::";
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
	auto potentialNamespace = method->firstAncestorOfType<Module>();
	if (headerVisitor() && method->firstAncestorOfType<Declaration>() == potentialNamespace &&
		 method->typeArguments()->isEmpty())
		*fragment << pluginName(potentialNamespace, method).toUpper() << "_API ";

	// method name qualifier
	if (sourceVisitor() && method->methodKind() != Method::MethodKind::Conversion)
		for (auto i = 0; i < parentClasses.size(); i++)
		{
			*fragment << parentClasses.at(i)->name();
			if (!parentClasses.at(i)->typeArguments()->isEmpty())
			{
				auto typeArgumentComposite = new CompositeFragment{parentClasses.at(i)->typeArguments(), "typeArgsList"};
				for (auto typeArgument : *parentClasses.at(i)->typeArguments())
					*typeArgumentComposite << typeArgument->nameNode();
				*fragment << typeArgumentComposite;
			}
			*fragment << "::";
		}

	if (method->methodKind() == Method::MethodKind::Destructor && !method->name().startsWith("~")) *fragment << "~";
	if (method->methodKind() == Method::MethodKind::OperatorOverload) *fragment << "operator";
	*fragment << method->nameNode();

	*fragment << list(method->arguments(), ElementVisitor(data()), "argsList");
	if (method->modifiers()->isSet(Modifier::Const))
		*fragment << " " << new TextFragment{method->modifiers(), "const"};

	if (!headerVisitor())
		if (!method->memberInitializers()->isEmpty())
			*fragment << " : " << list(method->memberInitializers(), ElementVisitor(data()), "comma");

	if (!method->throws()->isEmpty())
	{
		*fragment << " throw (";
		*fragment << list(method->throws(), ExpressionVisitor(data()), "comma");
		*fragment << ")";
	}

	if (!sourceVisitor())
	{
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

	if (!headerVisitor())
		*fragment << list(method->items(), StatementVisitor(data()), "body");

	notAllowed(method->subDeclarations());
	notAllowed(method->memberInitializers());

	return fragment;
}

SourceFragment* DeclarationVisitor::compositeNodeComments(Model::CompositeNode* compositeNode, const QString& style)
{
	if (auto commentNode = DCast<Comments::CommentNode>(compositeNode->comment()))
	{
		auto commentFragment = new CompositeFragment{commentNode->lines(), style};
		for (auto line : *(commentNode->lines()))
			*commentFragment << line;
		return commentFragment;
	}

	return nullptr;
}

SourceFragment* DeclarationVisitor::visit(VariableDeclaration* variableDeclaration)
{
	auto fragment = new CompositeFragment{variableDeclaration};
	if (!sourceVisitor())
	{
		*fragment << compositeNodeComments(variableDeclaration, "declarationComment");
		*fragment << printAnnotationsAndModifiers(variableDeclaration);
		*fragment << expression(variableDeclaration->typeExpression()) << " " << variableDeclaration->nameNode();
		if (variableDeclaration->initialValue() && !variableDeclaration->modifiers()->isSet(Modifier::Static))
		{
			//TODO: Use {} instead of =
			if (!DCast<ArrayInitializer>(variableDeclaration->initialValue())) *fragment << " = ";
			*fragment << expression(variableDeclaration->initialValue());
		}
		if (!DCast<Expression>(variableDeclaration->parent())) *fragment << ";";
	}
	else if (sourceVisitor() && (!DCast<Field>(variableDeclaration) ||
										  variableDeclaration->modifiers()->isSet(Modifier::Static) ||
										  variableDeclaration->modifiers()->isSet(Modifier::ConstExpr)))
	{
		if (DCast<Field>(variableDeclaration))
			if (auto parentClass = variableDeclaration->firstAncestorOfType<Class>())
				if (!parentClass->typeArguments()->isEmpty())
					*fragment << list(parentClass->typeArguments(), ElementVisitor(data()), "templateArgsList");

		if (!DCast<Field>(variableDeclaration) || variableDeclaration->modifiers()->isSet(Modifier::ConstExpr))
			*fragment << printAnnotationsAndModifiers(variableDeclaration);

		*fragment << expression(variableDeclaration->typeExpression()) << " ";

		if (DCast<Field>(variableDeclaration))
			if (auto parentClass = variableDeclaration->firstAncestorOfType<Class>())
			{
				*fragment << parentClass->name();
				if (!parentClass->typeArguments()->isEmpty())
				{
					auto typeArgumentComposite = new CompositeFragment{parentClass->typeArguments(), "typeArgsList"};
					for (auto typeArgument : *parentClass->typeArguments())
						*typeArgumentComposite << typeArgument->nameNode();
					*fragment << typeArgumentComposite;
				}
				*fragment << "::";
			}

		*fragment << variableDeclaration->nameNode();
		if (variableDeclaration->initialValue())
		{
			if (!DCast<ArrayInitializer>(variableDeclaration->initialValue()) ||
				 DCast<AutoTypeExpression>(variableDeclaration->typeExpression())) *fragment << " = ";
			*fragment << expression(variableDeclaration->initialValue());
		}

		if (!DCast<Expression>(variableDeclaration->parent())) *fragment << ";";
	}
	return fragment;
}

SourceFragment* DeclarationVisitor::printAnnotationsAndModifiers(Declaration* declaration)
{
	auto fragment = new CompositeFragment{declaration, "vertical"};
	if (!declaration->annotations()->isEmpty()) // avoid an extra new line if there are no annotations
		*fragment << list(declaration->annotations(), StatementVisitor(data()), "vertical");
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

	if (headerVisitor())
		*fragment << "extern template class "
					 << explicitTemplateInstantiation->firstAncestorOfType<OOModel::Project>()->name().toUpper() << "_API ";
	else
		*fragment << "template class ";

	*fragment << ExpressionVisitor(data()).visit(explicitTemplateInstantiation->instantiatedClass()) << ";";
	return fragment;
}

SourceFragment* DeclarationVisitor::visit(TypeAlias* typeAlias)
{
	auto fragment = new CompositeFragment{typeAlias};
	*fragment << compositeNodeComments(typeAlias, "declarationComment");

	if (!typeAlias->typeArguments()->isEmpty())
		*fragment << list(typeAlias->typeArguments(), ElementVisitor(data()), "templateArgsList");

	*fragment << "using " << typeAlias->nameNode() << " = " << expression(typeAlias->typeExpression()) << ";";
	return fragment;
}
}
