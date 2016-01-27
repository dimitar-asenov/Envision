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
#include "JavaExporter.h"
#include "../visitors/DeclarationVisitor.h"

#include "OOModel/src/declarations/Project.h"

#include "Export/src/writer/Exporter.h"
#include "Export/src/writer/FragmentLayouter.h"
#include "ModelBase/src/model/TreeManager.h"

namespace JavaExport {

QList<Export::ExportError> JavaExporter::exportTree(Model::TreeManager* manager,
																	 const QString& pathToProjectContainerDirectory)
{
	auto project = DCast<OOModel::Project>(manager->root());
	Q_ASSERT(project);

	DeclarationVisitor visitor;
	auto dir = std::unique_ptr<Export::SourceDir>( visitor.visitProject(project) );

	auto layouter = Export::FragmentLayouter{"\t"};
	layouter.addRule("enumerators", Export::FragmentLayouter::SpaceAfterSeparator, "", ",", "");
	layouter.addRule("vertical", Export::FragmentLayouter::NoIndentation, "", "\n", "");
	layouter.addRule("sections", Export::FragmentLayouter::NoIndentation, "", "\n\n", "");
	layouter.addRule("bodySections", Export::FragmentLayouter::NewLineBefore
						  | Export::FragmentLayouter::IndentChildFragments | Export::FragmentLayouter::NewLineAfterPrefix
						  | Export::FragmentLayouter::NewLineBeforePostfix, "{", "\n\n", "}");
	layouter.addRule("space", Export::FragmentLayouter::SpaceAtEnd, "", " ", "");
	layouter.addRule("comma", Export::FragmentLayouter::SpaceAfterSeparator, "", ",", "");
	layouter.addRule("initializerList", Export::FragmentLayouter::SpaceAfterSeparator, "{", ",", "}");
	layouter.addRule("argsList", Export::FragmentLayouter::SpaceAfterSeparator, "(", ",", ")");
	layouter.addRule("typeArgsList", Export::FragmentLayouter::SpaceAfterSeparator, "<", ",", ">");

	layouter.addRule("body", Export::FragmentLayouter::NewLineBefore | Export::FragmentLayouter::IndentChildFragments
							| Export::FragmentLayouter::NewLineAfterPrefix | Export::FragmentLayouter::NewLineBeforePostfix,
							"{", "\n", "}");

	auto map = Export::Exporter::exportToFileSystem(pathToProjectContainerDirectory, dir.get(), &layouter,
																	Export::Exporter::ExportSpan::AllFiles);
	exportMaps().insert(project, map);

	return visitor.errors();
}

Export::ExportMapContainer& JavaExporter::exportMaps()
{
	static Export::ExportMapContainer* container = new Export::ExportMapContainer{};
	return *container;
}

}
