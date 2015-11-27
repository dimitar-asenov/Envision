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

#include "DeclarationVisitorSource.h"
#include "ExpressionVisitorSource.h"
#include "StatementVisitorSource.h"
#include "ElementVisitorSource.h"

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

namespace CppExport {

SourceFragment* DeclarationVisitorSource::visit(Declaration* declaration)
{
	if (auto castDeclaration = DCast<Method>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<Class>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<VariableDeclaration>(declaration)) return visit(castDeclaration);

	notAllowed(declaration);

	// TODO: handle comments
	auto fragment = new CompositeFragment(declaration);
	*fragment << "Invalid Declaration";
	return fragment;
}

SourceDir* DeclarationVisitorSource::visitProject(Project* project, SourceDir* parent)
{
	auto projectDir = parent ? &parent->subDir(project->name()) : new SourceDir(nullptr, "src");

	for (auto node : *project->projects()) visitProject(node, projectDir);
	for (auto node : *project->modules()) visitModule(node, projectDir);
	for (auto node : *project->classes()) visitTopLevelClass(node, projectDir);

	notAllowed(project->methods());
	notAllowed(project->fields());

	return projectDir;
}

SourceDir* DeclarationVisitorSource::visitModule(Module* module, SourceDir* parent)
{
	Q_ASSERT(parent);
	auto moduleDir = &parent->subDir(module->name());

	for (auto node : *module->modules()) visitModule(node, moduleDir);
	for (auto node : *module->classes()) visitTopLevelClass(node, moduleDir);

	notAllowed(module->methods());
	notAllowed(module->fields());

	return moduleDir;
}

SourceFile* DeclarationVisitorSource::visitTopLevelClass(Class* classs, SourceDir* parent)
{
	Q_ASSERT(parent);
	auto classFile = &parent->file(classs->name() + ".cpp");

	auto fragment = classFile->append(new CompositeFragment(classs, "sections"));

	auto imports = fragment->append(new CompositeFragment(classs, "vertical"));
	for (auto node : *classs->subDeclarations())
	{
		if (auto ni = DCast<NameImport>(node)) *imports << visit(ni);
		else notAllowed(node);
	}

	*fragment << visit(classs);

	return classFile;
}

SourceFragment* DeclarationVisitorSource::visitTopLevelClass(Class* classs)
{
	return visit(classs);
}

SourceFragment* DeclarationVisitorSource::visit(Class* classs)
{
	auto fragment = new CompositeFragment(classs);

	//TODO
	auto sections = fragment->append( new CompositeFragment(classs, "sections"));
	*sections << list(classs->enumerators(), ElementVisitorSource(data()), "enumerators");
	*sections << list(classs->classes(), this, "sections");
	*sections << list(classs->methods(), this, "spacedSections");
	*sections << list(classs->fields(), this, "vertical");

	return fragment;
}

SourceFragment* DeclarationVisitorSource::visit(Method* method)
{
	auto fragment = new CompositeFragment(method);

	if (!method->typeArguments()->isEmpty())
		*fragment << list(method->typeArguments(), ElementVisitorSource(data()), "templateArgsList");

	if (method->results()->size() > 1)
		error(method->results(), "Cannot have more than one return value in C++");

	if (method->methodKind() != Method::MethodKind::Constructor &&
		 method->methodKind() != Method::MethodKind::Destructor)
	{
		if (!method->results()->isEmpty())
			*fragment << expression(method->results()->at(0)->typeExpression()) << " ";
		else
			*fragment << "void ";
	}

	if (auto parentClass = method->firstAncestorOfType<Class>())
		*fragment << parentClass->name() << "::";

	if (method->methodKind() == Method::MethodKind::Destructor && !method->name().startsWith("~")) *fragment << "~";
	*fragment << method->nameNode();

	if (!method->typeArguments()->isEmpty())
		*fragment << list(method->typeArguments(), ElementVisitorSource(data()), "typeArgsList");

	*fragment << list(method->arguments(), ElementVisitorSource(data()), "argsList");

	if (!method->memberInitializers()->isEmpty())
		*fragment << " : " << list(method->memberInitializers(), ElementVisitorSource(data()));

	if (!method->throws()->isEmpty())
	{
		*fragment << " throw (";
		*fragment << list(method->throws(), ExpressionVisitorSource(data()), "comma");
		*fragment << ")";
	}

	*fragment << list(method->items(), StatementVisitorSource(data()), "body");

	notAllowed(method->subDeclarations());
	notAllowed(method->memberInitializers());

	return fragment;
}

SourceFragment* DeclarationVisitorSource::visit(VariableDeclaration* variableDeclaration)
{
	auto fragment = new CompositeFragment(variableDeclaration);
	bool isField = DCast<Field>(variableDeclaration);

	if (!isField || variableDeclaration->modifiers()->isSet(Modifier::Static))
	{
		*fragment << expression(variableDeclaration->typeExpression()) << " ";

		if (isField)
			if (auto parentClass = variableDeclaration->firstAncestorOfType<Class>())
				*fragment << parentClass->name() << "::";

		*fragment << variableDeclaration->nameNode();
		if (variableDeclaration->initialValue())
			*fragment << " = " << expression(variableDeclaration->initialValue());

		if (!DCast<Expression>(variableDeclaration->parent())) *fragment << ";";
	}
	return fragment;
}

SourceFragment* DeclarationVisitorSource::visit(NameImport* nameImport)
{
	auto fragment = new CompositeFragment(nameImport);
	notAllowed(nameImport->annotations());

	*fragment << "import " << expression(nameImport->importedName());
	if (nameImport->importAll()) *fragment << ".*";
	*fragment << ";";

	return fragment;
}

SourceFragment* DeclarationVisitorSource::visit(ExplicitTemplateInstantiation* eti)
{
	notAllowed(eti);
	return new TextFragment(eti);
}

SourceFragment* DeclarationVisitorSource::visit(TypeAlias* typeAlias)
{
	auto fragment = new CompositeFragment(typeAlias);
	*fragment << "using " << typeAlias->nameNode() << " = " << expression(typeAlias->typeExpression()) << ";";
	return fragment;
}
}
