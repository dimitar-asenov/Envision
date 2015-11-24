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

#include "CppExporter.h"
#include "../visitors/header_visitors/DeclarationVisitorHeader.h"
#include "../visitors/source_visitors/DeclarationVisitorSource.h"

#include "OOModel/src/declarations/Project.h"

#include "Export/src/writer/Exporter.h"
#include "Export/src/writer/FragmentLayouter.h"
#include "ModelBase/src/model/TreeManager.h"

#include "CodeUnit.h"
#include "CodeComposite.h"

namespace CppExport {

QList<ExportError> CppExporter::exportTree(Model::TreeManager* manager, const QString& pathToProjectContainerDirectory)
{
	auto project = DCast<OOModel::Project>(manager->root());
	Q_ASSERT(project);

	auto codeComposite = new CodeComposite("Composite");
	for (auto i = 0; i < project->classes()->size(); i++)
	{
		auto c = project->classes()->at(i);
		//if (c->name() == "HelloWorld" || c->name() == "Generic")
		{
			auto unit = new CodeUnit(c->name(), c);
			codeComposite->addUnit(unit);
		}
	}

	exportCodeComposite(codeComposite);

	return {};

	auto layout = layouter();

	DeclarationVisitorHeader visitor;
	auto dir = std::unique_ptr<Export::SourceDir>( visitor.visitProject(project) );
	auto map = Export::Exporter::exportToFileSystem(pathToProjectContainerDirectory + "/Headers", dir.get(), &layout);
	exportMaps().insert(project, map);

	DeclarationVisitorSource visitor2;
	auto dir2 = std::unique_ptr<Export::SourceDir>( visitor2.visitProject(project) );
	auto map2 = Export::Exporter::exportToFileSystem(pathToProjectContainerDirectory + "/Sources", dir2.get(), &layout);
	exportMaps().insert(project, map2);

	return visitor.errors();
}

void CppExporter::exportFragment(Export::SourceFragment* fragment)
{
	auto dir = new Export::SourceDir(nullptr, "src");
	auto file = &dir->file("exported.h");
	file->append(fragment);
	auto layout = layouter();
	Export::Exporter::exportToFileSystem("", dir, &layout);
}

void CppExporter::calculateSourceFragments(CodeComposite* codeComposite)
{
	DeclarationVisitorHeader visitor;
	for (auto unit : codeComposite->units())
		if (auto classs = DCast<OOModel::Class>(unit->node()))
			unit->setSourceFragment(visitor.visitTopLevelClass(classs));
		else
			Q_ASSERT(false);
}

void CppExporter::exportCodeComposite(CodeComposite* codeComposite)
{
	calculateSourceFragments(codeComposite);

	auto composite = new Export::CompositeFragment(new Model::Integer());
	for (auto unit : codeComposite->units())
		composite->append(unit->sourceFragment());

	exportFragment(composite);
}

Export::FragmentLayouter CppExporter::layouter()
{
	auto result = Export::FragmentLayouter{"\t"};
	result.addRule("enumerators", Export::FragmentLayouter::SpaceAfterSeparator, "", ",", "");
	result.addRule("vertical", Export::FragmentLayouter::NoIndentation, "", "\n", "");
	result.addRule("sections", Export::FragmentLayouter::NoIndentation, "", "\n", "");
	result.addRule("accessorSections", Export::FragmentLayouter::IndentChildFragments, "", "\n", "");
	result.addRule("bodySections", Export::FragmentLayouter::NewLineBefore
						  | Export::FragmentLayouter::IndentChildFragments | Export::FragmentLayouter::NewLineAfterPrefix
						  | Export::FragmentLayouter::NewLineBeforePostfix, "{", "\n", "}");
	result.addRule("space", Export::FragmentLayouter::SpaceAtEnd, "", " ", "");
	result.addRule("comma", Export::FragmentLayouter::SpaceAfterSeparator, "", ",", "");
	result.addRule("baseClasses", Export::FragmentLayouter::SpaceAfterSeparator, "public: ", ",", "");
	result.addRule("initializerList", Export::FragmentLayouter::SpaceAfterSeparator, "{", ",", "}");
	result.addRule("argsList", Export::FragmentLayouter::SpaceAfterSeparator, "(", ",", ")");
	result.addRule("typeArgsList", Export::FragmentLayouter::SpaceAfterSeparator, "<", ",", ">");

	result.addRule("body", Export::FragmentLayouter::NewLineBefore | Export::FragmentLayouter::IndentChildFragments
							| Export::FragmentLayouter::NewLineAfterPrefix | Export::FragmentLayouter::NewLineBeforePostfix,
							"{", "\n", "}");

	return result;
}

Export::ExportMapContainer& CppExporter::exportMaps()
{
	static Export::ExportMapContainer* container = new Export::ExportMapContainer();
	return *container;
}

}
