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

#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/NameImport.h"

#include "Export/src/tree/SourceDir.h"
#include "Export/src/tree/SourceFile.h"
#include "Export/src/tree/CompositeFragment.h"
#include "Export/src/tree/TextFragment.h"

using namespace Export;
using namespace OOModel;

namespace JavaExport {

SourceDir* DeclarationVisitor::visitProject(Project* project, SourceDir* parent)
{
	auto projectDir = parent ? &parent->subDir(project->name()) : new SourceDir(nullptr, "src");

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

	auto fragment = classFile->append(new CompositeFragment(classs, "vertical"));

	auto header = fragment->append(new CompositeFragment(classs));
	if (!packageStack().isEmpty())
		*header << "package " << packagesSoFar() << ";";

	for (auto node : *classs->subDeclarations())
	{
		if (auto ni = DCast<NameImport>(node)) *fragment << visit(ni);
		else notAllowed(node);
	}

	*fragment << visit(classs);

	return classFile;
}

SourceFragment* DeclarationVisitor::visit(Class* classs)
{
	auto fragment = new CompositeFragment(classs);
	*fragment << visitDeclaration(classs) << "class " << new TextFragment(classs->nameNode(), classs->name());

	if (!classs->baseClasses()->isEmpty())
	{
		*fragment << " extends ";
		*fragment << list(classs->baseClasses(), this, "comma");
	}

	notAllowed(classs->friends());

	//TODO
	*fragment << list(classs->methods(), this, "body");

	return fragment;
}

SourceFragment* DeclarationVisitor::visit(Method* method)
{
	auto fragment = new CompositeFragment(method);
	*fragment << visitDeclaration(method) << new TextFragment(method->nameNode(), method->name());

	//TODO
	*fragment << list(method->items(), this, "body");

	return fragment;
}

SourceFragment* DeclarationVisitor::visit(Field* field)
{
	auto fragment = new CompositeFragment(field);
	*fragment << visitDeclaration(field);
	return fragment;
}


SourceFragment* DeclarationVisitor::visit(NameImport* nameImport)
{
	auto fragment = new CompositeFragment(nameImport);
	*fragment << visitDeclaration(nameImport);
	return fragment;
}

SourceFragment* DeclarationVisitor::visitDeclaration(Declaration* declaration)
{
	auto fragment = new CompositeFragment(declaration, "vertical");
	*fragment << list(declaration->annotations(), this, "vertical");
	auto header = fragment->append(new CompositeFragment(declaration, "space"));

	if (declaration->modifiers()->isSet(Modifier::Public))
		*header << new TextFragment(declaration->modifiers(), "public");
	if (declaration->modifiers()->isSet(Modifier::Private))
		*header << new TextFragment(declaration->modifiers(), "private");
	if (declaration->modifiers()->isSet(Modifier::Protected))
		*header << new TextFragment(declaration->modifiers(), "protected");

	if (declaration->modifiers()->isSet(Modifier::Static))
		*header << new TextFragment(declaration->modifiers(), "static");

	if (declaration->modifiers()->isSet(Modifier::Final))
		*header << new TextFragment(declaration->modifiers(), "final");
	if (declaration->modifiers()->isSet(Modifier::Abstract))
		*header << new TextFragment(declaration->modifiers(), "abstract");

	if (declaration->modifiers()->isSet(Modifier::Virtual))
		error(declaration->modifiers(), "Virtual modifier is invalid in Java");
	if (declaration->modifiers()->isSet(Modifier::Override))
		error(declaration->modifiers(), "Override modifier is invalid in Java");
	if (declaration->modifiers()->isSet(Modifier::Inline))
		error(declaration->modifiers(), "Inline modifier is invalid in Java");

	return fragment;
}


SourceFragment* DeclarationVisitor::visit(StatementItem* statementItem)
{
	auto fragment = new CompositeFragment(statementItem);
	return fragment;
}


SourceFragment* DeclarationVisitor::visit(Expression* expression)
{
	auto fragment = new CompositeFragment(expression);
	return fragment;
}

} /* namespace JavaExport */
