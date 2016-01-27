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
#include "AlloyExporter.h"
#include "../visitors/AlloyVisitor.h"

#include "Export/src/writer/Exporter.h"
#include "Export/src/writer/FragmentLayouter.h"
#include "Export/src/tree/SourceDir.h"
#include "Export/src/tree/SourceFragment.h"

namespace Alloy {

void AlloyExporter::exportTree(Model::Node* aNode, const QString& path)
{
	auto dir = new Export::SourceDir{nullptr, path};
	dir->subDir("output");

	auto file = &dir->file("model.als");
	auto anAlloyVisitor = new AlloyVisitor{};
	file->append(anAlloyVisitor->visit(aNode));
	delete anAlloyVisitor;

	file->append(aNode, "pred show() {}\n");
	file->append(aNode, "run show for 3");

	auto layouter = Export::FragmentLayouter{"\t"};
	layouter.addRule("fieldList", Export::FragmentLayouter::SpaceAfterSeparator |
						  Export::FragmentLayouter::NewLineAfterPostfix, "{", ",", "}");
	layouter.addRule("argsDefinitionList", Export::FragmentLayouter::SpaceAfterSeparator |
						  Export::FragmentLayouter::NewLineAfterPostfix, "(", ",", ")");
	layouter.addRule("argsCallList", Export::FragmentLayouter::SpaceAfterSeparator, "[", ",", "]");
	layouter.addRule("methodBody", Export::FragmentLayouter::NewLineAfterPrefix |
						  Export::FragmentLayouter::NewLineBefore |
						  Export::FragmentLayouter::NewLineAfterPostfix |
						  Export::FragmentLayouter::NewLineBeforePostfix, "{", "\n", "}");

	Export::Exporter::exportToFileSystem(path, dir, &layouter, Export::Exporter::ExportSpan::AllFiles);
}

}
