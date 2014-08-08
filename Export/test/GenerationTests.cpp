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

#include "ExportPlugin.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "../src/writer/Exporter.h"
#include "../src/writer/FragmentLayouter.h"
#include "../src/writer/TextToNodeMap.h"
#include "../src/tree/SourceDir.h"
#include "../src/tree/SourceFile.h"
#include "../src/tree/TextFragment.h"
#include "../src/tree/CompositeFragment.h"

using namespace Model;

namespace Export {

TEST(ExportPlugin, ExportJustText)
{
	QString testDir = QDir::tempPath() + "/Envision/Export/tests";
	FragmentLayouter layouter{"\t"};

	SourceDir root{nullptr, "text"};
	auto& file = root.file("text");
	file.append( reinterpret_cast<Node*>(1), "Test text" ); // The pointer is irrelevant for now

	auto map = Exporter::exportToFileSystem(testDir, &root, &layouter);
	SAFE_DELETE(map);

	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text", testDir +"/text/text");
}

TEST(ExportPlugin, ExportAndModify)
{
	QString testDir = QDir::tempPath() + "/Envision/Export/tests";
	FragmentLayouter layouter{"\t"};

	SourceDir root{nullptr, "text"};
	auto& file = root.file("text");
	file.append( reinterpret_cast<Node*>(1), "Test text" ); // The pointer is irrelevant for now

	auto map = Exporter::exportToFileSystem(testDir, &root, &layouter);
	SAFE_DELETE(map);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text", testDir +"/text/text");

	file.append( reinterpret_cast<Node*>(2), ". Now modified" ); // The pointer is irrelevant for now

	map = Exporter::exportToFileSystem(testDir, &root, &layouter);
	SAFE_DELETE(map);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text_modified", testDir +"/text/text");
}

}
