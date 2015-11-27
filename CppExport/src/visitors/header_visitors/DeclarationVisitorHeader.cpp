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

#include "DeclarationVisitorHeader.h"
#include "ExpressionVisitorHeader.h"
#include "StatementVisitorHeader.h"
#include "ElementVisitorHeader.h"
#include "../source_visitors/DeclarationVisitorSource.h"

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

SourceFragment* DeclarationVisitorHeader::visit(Declaration* declaration)
{
	if (auto castDeclaration = DCast<Method>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<Class>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<VariableDeclaration>(declaration)) return visit(castDeclaration);
	if (auto castDeclaration = DCast<TypeAlias>(declaration)) return visit(castDeclaration);

	notAllowed(declaration);

	// TODO: handle comments
	auto fragment = new CompositeFragment(declaration);
	*fragment << "Invalid Declaration";
	return fragment;
}

SourceDir* DeclarationVisitorHeader::visitProject(Project* project, SourceDir* parent)
{
	auto projectDir = parent ? &parent->subDir(project->name()) : new SourceDir(nullptr, "src");

	for (auto node : *project->projects()) visitProject(node, projectDir);
	for (auto node : *project->modules()) visitModule(node, projectDir);
	for (auto node : *project->classes()) visitTopLevelClass(node, projectDir);

	notAllowed(project->methods());
	notAllowed(project->fields());

	return projectDir;
}

SourceDir* DeclarationVisitorHeader::visitModule(Module* module, SourceDir* parent)
{
	Q_ASSERT(parent);
	auto moduleDir = &parent->subDir(module->name());

	for (auto node : *module->modules()) visitModule(node, moduleDir);
	for (auto node : *module->classes()) visitTopLevelClass(node, moduleDir);

	notAllowed(module->methods());
	notAllowed(module->fields());

	return moduleDir;
}

SourceFile* DeclarationVisitorHeader::visitTopLevelClass(Class* classs, SourceDir* parent)
{
	Q_ASSERT(parent);
	auto classFile = &parent->file(classs->name() + ".h");

	CompositeFragment* fragment = classFile->append(new CompositeFragment(classs, "sections"));

	auto imports = fragment->append(new CompositeFragment(classs, "vertical"));
	for (auto node : *classs->subDeclarations())
	{
		if (auto ni = DCast<NameImport>(node)) *imports << visit(ni);
		else notAllowed(node);
	}

	*fragment << visit(classs);

	return classFile;
}

SourceFragment* DeclarationVisitorHeader::visitTopLevelClass(Class* classs)
{
	auto fragment = new CompositeFragment(classs, "spacedSections");
	*fragment << visit(classs);

	auto filter = [](Method* method) { return !method->typeArguments()->isEmpty(); };
	*fragment << list(classs->methods(), DeclarationVisitorSource(data()), "spacedSections", filter);
	return fragment;
}

SourceFragment* DeclarationVisitorHeader::visit(Class* classs)
{
	auto fragment = new CompositeFragment(classs);
	if (Class::ConstructKind::Class == classs->constructKind())
		*fragment << printAnnotationsAndModifiers(classs) << "class " << classs->nameNode();
	else if (Class::ConstructKind::Struct == classs->constructKind())
		*fragment << printAnnotationsAndModifiers(classs) << "struct " << classs->nameNode();
	else if (Class::ConstructKind::Enum == classs->constructKind())
		*fragment << printAnnotationsAndModifiers(classs) << "enum " << classs->nameNode();
	else
		notAllowed(classs);

	if (!classs->typeArguments()->isEmpty())
		*fragment << list(classs->typeArguments(), ElementVisitorHeader(data()), "typeArgsList");

	if (!classs->baseClasses()->isEmpty())
		// TODO: inheritance modifiers like private, virtual... (not only public)
		*fragment << list(classs->baseClasses(), ExpressionVisitorHeader(data()), "baseClasses");

	notAllowed(classs->friends());

	auto sections = fragment->append( new CompositeFragment(classs, "bodySections"));

	if (classs->enumerators()->size() > 0)
		error(classs->enumerators(), "Enum unhandled"); // TODO

	auto publicSection = new CompositeFragment(classs, "accessorSections");
	auto publicFilter = [](Declaration* declaration) { return declaration->modifiers()->isSet(Modifier::Public); };
	bool hasPublicSection = addMemberDeclarations(classs, publicSection, publicFilter);
	if (hasPublicSection)
	{
		*sections << "public:";
		sections->append(publicSection);
	}

	auto protectedSection = new CompositeFragment(classs, "accessorSections");
	auto protectedFilter = [](Declaration* declaration) { return declaration->modifiers()->isSet(Modifier::Protected); };
	bool hasProtectedSection = addMemberDeclarations(classs, protectedSection, protectedFilter);
	if (hasProtectedSection)
	{
		if (hasPublicSection) *sections << "\n"; // add newline between two accessor sections

		*sections << "protected:";
		sections->append(protectedSection);
	}

	auto privateSection = new CompositeFragment(classs, "accessorSections");
	auto privateFilter = [](Declaration* declaration) { return !declaration->modifiers()->isSet(Modifier::Public) &&
																				  !declaration->modifiers()->isSet(Modifier::Protected); };
	bool hasPrivateSection = addMemberDeclarations(classs, privateSection, privateFilter);
	if (hasPrivateSection)
	{
		if (hasPublicSection || hasProtectedSection) *sections << "\n"; // add newline between two accessor sections

		*sections << "private:";
		sections->append(privateSection);
	}

	return fragment;
}

template<typename Predicate>
bool DeclarationVisitorHeader::addMemberDeclarations(Class* classs, CompositeFragment* section, Predicate filter)
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

SourceFragment* DeclarationVisitorHeader::visit(Method* method)
{
	auto fragment = new CompositeFragment(method);

	if (!method->typeArguments()->isEmpty())
		*fragment << list(method->typeArguments(), ElementVisitorHeader(data()), "templateArgsList");

	*fragment << printAnnotationsAndModifiers(method);

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

	if (method->methodKind() == Method::MethodKind::Destructor && !method->name().startsWith("~")) *fragment << "~";
	*fragment << method->nameNode();

	*fragment << list(method->arguments(), ElementVisitorHeader(data()), "argsList");

	if (!method->throws()->isEmpty())
	{
		*fragment << " throw (";
		*fragment << list(method->throws(), ExpressionVisitorHeader(data()), "comma");
		*fragment << ")";
	}

	notAllowed(method->subDeclarations());
	notAllowed(method->memberInitializers());

	if (method->modifiers()->isSet(Modifier::Override))
		*fragment << " " << new TextFragment(method->modifiers(), "override");

	*fragment << ";";

	return fragment;
}

SourceFragment* DeclarationVisitorHeader::visit(VariableDeclaration* vd)
{
	auto fragment = new CompositeFragment(vd);
	*fragment << printAnnotationsAndModifiers(vd);
	*fragment << expression(vd->typeExpression()) << " " << vd->nameNode();
	if (vd->initialValue())
		*fragment << " = " << expression(vd->initialValue());

	if (!DCast<Expression>(vd->parent())) *fragment << ";";
	return fragment;
}

SourceFragment* DeclarationVisitorHeader::visit(NameImport* nameImport)
{
	error(nameImport, "NameImport unhandled"); // TODO
	return new TextFragment(nameImport);
}

SourceFragment* DeclarationVisitorHeader::printAnnotationsAndModifiers(Declaration* declaration)
{
	auto fragment = new CompositeFragment(declaration, "vertical");
	if (!declaration->annotations()->isEmpty()) // avoid an extra new line if there are no annotations
		*fragment << list(declaration->annotations(), StatementVisitorHeader(data()), "vertical");
	auto header = fragment->append(new CompositeFragment(declaration, "space"));

	if (declaration->modifiers()->isSet(Modifier::Static))
		*header << new TextFragment(declaration->modifiers(), "static");

	if (declaration->modifiers()->isSet(Modifier::Final))
		*header << new TextFragment(declaration->modifiers(), "final");
	if (declaration->modifiers()->isSet(Modifier::Abstract))
		*header << new TextFragment(declaration->modifiers(), "abstract");

	if (declaration->modifiers()->isSet(Modifier::Virtual))
		*header << new TextFragment(declaration->modifiers(), "virtual");
	if (declaration->modifiers()->isSet(Modifier::Inline))
		*header << new TextFragment(declaration->modifiers(), "inline");

	return fragment;
}

SourceFragment* DeclarationVisitorHeader::visit(ExplicitTemplateInstantiation* eti)
{
	error(eti, "ExplicitTemplateInstantiation unhandled"); // TODO
	return new TextFragment(eti);
}

SourceFragment* DeclarationVisitorHeader::visit(TypeAlias* typeAlias)
{
	auto fragment = new CompositeFragment(typeAlias);
	*fragment << "using " << typeAlias->nameNode() << " = " << expression(typeAlias->typeExpression()) << ";";
	return fragment;
}

}
