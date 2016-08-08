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

#include "DeclarationVisitor.h"
#include "ExpressionVisitor.h"
#include "StatementVisitor.h"
#include "ElementVisitor.h"

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Declaration.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/NameImport.h"
#include "OOModel/src/declarations/VariableDeclaration.h"
#include "OOModel/src/declarations/ExplicitTemplateInstantiation.h"
#include "OOModel/src/declarations/TypeAlias.h"

#include "Export/src/tree/SourceDir.h"
#include "Export/src/tree/SourceFile.h"
#include "Export/src/tree/CompositeFragment.h"
#include "Export/src/tree/TextFragment.h"

using namespace Export;
using namespace OOModel;

namespace JavaExport {

SourceFragment* DeclarationVisitor::visit(Declaration* declaration)
{
	if (auto castDeclaration = DCast<Method>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<Class>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<VariableDeclaration>(declaration)) return visit(castDeclaration);

	notAllowed(declaration);

	// TODO: handle comments
	auto fragment = new CompositeFragment{declaration};
	*fragment << "Invalid Declaration";
	return fragment;
}

SourceDir* DeclarationVisitor::visitProject(Project* project, SourceDir* parent)
{
	auto projectDir = parent ? &parent->subDir(project->name()) : new SourceDir{nullptr, "src"};

	if (parent) packageStack().append(project->name());

	for (auto node : *project->projects()) visitProject(node, projectDir);
	for (auto node : *project->modules()) visitModule(node, projectDir);
	for (auto node : *project->classes()) visitTopLevelClass(node, projectDir);

	if (parent) packageStack().removeLast();

	notAllowed(project->methods());
	notAllowed(project->fields());

	return projectDir;
}

SourceDir* DeclarationVisitor::visitModule(Module* module, SourceDir* parent)
{
	Q_ASSERT(parent);
	auto moduleDir = &parent->subDir(module->name());

	packageStack().append(module->name());

	for (auto node : *module->modules()) visitModule(node, moduleDir);
	for (auto node : *module->classes()) visitTopLevelClass(node, moduleDir);

	packageStack().removeLast();

	notAllowed(module->methods());
	notAllowed(module->fields());

	return moduleDir;
}

SourceFile* DeclarationVisitor::visitTopLevelClass(Class* classs, SourceDir* parent)
{
	Q_ASSERT(parent);
	auto classFile = &parent->file(classs->name() + ".java");

	auto fragment = classFile->append(new CompositeFragment{classs, "sections"});

	auto header = fragment->append(new CompositeFragment{classs});
	if (!packageStack().isEmpty())
		*header << "package " << packagesSoFar() << ";";

	auto imports = fragment->append(new CompositeFragment{classs, "vertical"});
	for (auto node : *classs->subDeclarations())
	{
		if (auto ni = DCast<NameImport>(node)) *imports << visit(ni);
		else notAllowed(node);
	}

	*fragment << visit(classs);

	return classFile;
}

SourceFragment* DeclarationVisitor::visit(Class* classs)
{
	auto fragment = new CompositeFragment{classs};
	if (Class::ConstructKind::Class == classs->constructKind())
		*fragment << printAnnotationsAndModifiers(classs) << "class " << classs->nameNode();
	else if (Class::ConstructKind::Interface == classs->constructKind())
		*fragment << printAnnotationsAndModifiers(classs) << "interface " << classs->nameNode();
	else if (Class::ConstructKind::Enum == classs->constructKind())
		*fragment << printAnnotationsAndModifiers(classs) << "enum " << classs->nameNode();
	else if (Class::ConstructKind::EnumClass == classs->constructKind())
		*fragment << printAnnotationsAndModifiers(classs) << "enum class " << classs->nameNode();
	else if (Class::ConstructKind::Annotation == classs->constructKind())
		*fragment << printAnnotationsAndModifiers(classs) << "@interface " << classs->nameNode();
	else
		notAllowed(classs);

	if (!classs->typeArguments()->isEmpty())
		*fragment << list(classs->typeArguments(), ElementVisitor{data()}, "typeArgsList");

	if (!classs->baseClasses()->isEmpty())
	{
		if (Class::ConstructKind::Interface == classs->constructKind() ||
			 Class::ConstructKind::Annotation == classs->constructKind())
		{
			*fragment << " extends ";
			*fragment << list(classs->baseClasses(), ExpressionVisitor{data()}, "comma");
		}
		else if (Class::ConstructKind::Enum == classs->constructKind() ||
					Class::ConstructKind::EnumClass == classs->constructKind())
		{
			*fragment << " implements ";
			*fragment << list(classs->baseClasses(), ExpressionVisitor{data()}, "comma");
		}
		else
		{
			// TODO: there might not be an extend and only implements.
			*fragment << " extends ";
			*fragment << expression(classs->baseClasses()->at(0));

			if (classs->baseClasses()->size() > 1)
			{
				*fragment << " implements ";
				int i = 1;
				for (; i < classs->baseClasses()->size() - 1; ++i)
					*fragment << expression(classs->baseClasses()->at(i)) << ", ";
				*fragment << expression(classs->baseClasses()->at(i));
			}
		}
	}

	notAllowed(classs->friends());

	//TODO: Add more stuff if needed.
	auto sections = fragment->append( new CompositeFragment{classs, "bodySections"});
	*sections << list(classs->enumerators(), ElementVisitor{data()}, "enumerators");
	*sections << list(classs->classes(), this, "declarations");
	*sections << list(classs->methods(), this, "sections");
	*sections << list(classs->fields(), this, "vertical");

	return fragment;
}

SourceFragment* DeclarationVisitor::visit(Method* method)
{
	auto fragment = new CompositeFragment{method};
	*fragment << printAnnotationsAndModifiers(method);

	if (method->results()->size() > 1)
		error(method->results(), "Cannot have more than one return value in Java");

	if (!method->results()->isEmpty())
		*fragment << expression(method->results()->at(0)->typeExpression()) << " ";
	else if (method->methodKind() != Method::MethodKind::Constructor)
		*fragment << "void ";

	if (method->methodKind() == Method::MethodKind::Destructor)
		error(method, "Can not have a method of type Destructor in Java");

	*fragment << method->nameNode();

	if (!method->typeArguments()->isEmpty())
		*fragment << list(method->typeArguments(), ElementVisitor{data()}, "typeArgsList");

	*fragment << list(method->arguments(), ElementVisitor{data()}, "argsList");

	if (!method->throws()->isEmpty())
	{
		*fragment << " throws ";
		*fragment << list(method->throws(), ExpressionVisitor{data()}, "comma");
	}

	*fragment << list(method->items(), StatementVisitor{data()}, "body");

	notAllowed(method->subDeclarations());
	notAllowed(method->memberInitializers());

	return fragment;
}

SourceFragment* DeclarationVisitor::visit(VariableDeclaration* vd)
{
	auto fragment = new CompositeFragment{vd};
	*fragment << printAnnotationsAndModifiers(vd);
	*fragment << expression(vd->typeExpression()) << " " << vd->nameNode();
	if (vd->initialValue())
		*fragment << " = " << expression(vd->initialValue());

	if (!DCast<Expression>(vd->parent())) *fragment << ";";
	return fragment;
}

SourceFragment* DeclarationVisitor::visit(NameImport* nameImport)
{
	auto fragment = new CompositeFragment{nameImport};
	*fragment << printAnnotationsAndModifiers(nameImport);

	notAllowed(nameImport->annotations());

	*fragment << "import " << expression(nameImport->importedName());
	if (nameImport->importAll()) *fragment << ".*";
	*fragment << ";";

	return fragment;
}

SourceFragment* DeclarationVisitor::printAnnotationsAndModifiers(Declaration* declaration)
{
	auto fragment = new CompositeFragment{declaration, "vertical"};
	if (!declaration->annotations()->isEmpty()) // avoid an extra new line if there are no annotations
		*fragment << list(declaration->annotations(), StatementVisitor{data()}, "vertical");
	auto header = fragment->append(new CompositeFragment{declaration, "space"});

	if (declaration->modifiers()->isSet(Modifier::Public))
		*header << new TextFragment{declaration->modifiers(), "public"};
	if (declaration->modifiers()->isSet(Modifier::Private))
		*header << new TextFragment{declaration->modifiers(), "private"};
	if (declaration->modifiers()->isSet(Modifier::Protected))
		*header << new TextFragment{declaration->modifiers(), "protected"};

	if (declaration->modifiers()->isSet(Modifier::Static))
		*header << new TextFragment{declaration->modifiers(), "static"};

	if (declaration->modifiers()->isSet(Modifier::Final))
		*header << new TextFragment{declaration->modifiers(), "final"};
	if (declaration->modifiers()->isSet(Modifier::Abstract))
		*header << new TextFragment{declaration->modifiers(), "abstract"};

	if (declaration->modifiers()->isSet(Modifier::Virtual))
		error(declaration->modifiers(), "Virtual modifier is invalid in Java");
	if (declaration->modifiers()->isSet(Modifier::Override))
		error(declaration->modifiers(), "Override modifier is invalid in Java");
	if (declaration->modifiers()->isSet(Modifier::Inline))
		error(declaration->modifiers(), "Inline modifier is invalid in Java");

	return fragment;
}

SourceFragment* DeclarationVisitor::visit(ExplicitTemplateInstantiation* eti)
{
	notAllowed(eti);
	return new TextFragment{eti};
}

SourceFragment* DeclarationVisitor::visit(TypeAlias* ta)
{
	notAllowed(ta);
	return new TextFragment{ta};
}

}
