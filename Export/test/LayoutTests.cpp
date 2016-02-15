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

#include "../src/ExportPlugin.h"
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

class CompositeFragmentTest : public SelfTest::Test<ExportPlugin, CompositeFragmentTest> { public: void test()
{
	QString testDir = QDir::tempPath() + "/Envision/Export/tests";

	SourceDir root{nullptr, "composite"};
	auto& file = root.file("composite");

	auto header = file.append(new CompositeFragment{ reinterpret_cast<Node*>(1)});
	header->append( reinterpret_cast<Node*>(2), "type");
	header->append( reinterpret_cast<Node*>(1), " ");
	header->append( reinterpret_cast<Node*>(3), "name");

	auto argsList = header->append(new CompositeFragment{reinterpret_cast<Node*>(4), "arguments"});

	auto arg1 = argsList->append(new CompositeFragment{reinterpret_cast<Node*>(5)});
	arg1->append(reinterpret_cast<Node*>(6), "type1");
	arg1->append(reinterpret_cast<Node*>(5), " ");
	arg1->append(reinterpret_cast<Node*>(7), "arg1");

	auto arg2 = argsList->append(new CompositeFragment{reinterpret_cast<Node*>(8)});
	arg2->append(reinterpret_cast<Node*>(9), "type2");
	arg2->append(reinterpret_cast<Node*>(8), " ");
	arg2->append(reinterpret_cast<Node*>(10), "arg2");

	auto body = file.append(new CompositeFragment{ reinterpret_cast<Node*>(11), "body"});
	body->append(reinterpret_cast<Node*>(12), "statement1");
	body->append(reinterpret_cast<Node*>(13), "statement2");

	auto body2 = body->append(new CompositeFragment{ reinterpret_cast<Node*>(14), "body"});
	body2->append(reinterpret_cast<Node*>(15), "subStatement1");
	body2->append(reinterpret_cast<Node*>(16), "subStatement2");

	FragmentLayouter layouter{"\t"};
	layouter.addRule("arguments", FragmentLayouter::SpaceAfterSeparator, "(", ",", ")");
	layouter.addRule("body", FragmentLayouter::NewLineBefore | FragmentLayouter::IndentChildFragments
							| FragmentLayouter::NewLineAfterPrefix | FragmentLayouter::NewLineBeforePostfix,
							"{", "\n", "}");

	auto map = Exporter::exportToFileSystem(testDir, &root, &layouter, Export::Exporter::ExportSpan::AllFiles);

	//Test the map

	// LINE 0

	// type
	CHECK_CONDITION(map->node("composite/composite", 0, 0) == reinterpret_cast<Node*>(2));
	CHECK_CONDITION(map->node("composite/composite", 0, 1) == reinterpret_cast<Node*>(2));
	CHECK_CONDITION(map->node("composite/composite", 0, 2) == reinterpret_cast<Node*>(2));
	CHECK_CONDITION(map->node("composite/composite", 0, 3) == reinterpret_cast<Node*>(2));

	// empty space
	CHECK_CONDITION(map->node("composite/composite", 0, 4) == reinterpret_cast<Node*>(1));

	// name
	CHECK_CONDITION(map->node("composite/composite", 0, 5) == reinterpret_cast<Node*>(3));
	CHECK_CONDITION(map->node("composite/composite", 0, 6) == reinterpret_cast<Node*>(3));
	CHECK_CONDITION(map->node("composite/composite", 0, 7) == reinterpret_cast<Node*>(3));
	CHECK_CONDITION(map->node("composite/composite", 0, 8) == reinterpret_cast<Node*>(3));

	// (
	CHECK_CONDITION(map->node("composite/composite", 0, 9) == reinterpret_cast<Node*>(4));

	// type1
	CHECK_CONDITION(map->node("composite/composite", 0, 10) == reinterpret_cast<Node*>(6));
	CHECK_CONDITION(map->node("composite/composite", 0, 11) == reinterpret_cast<Node*>(6));
	CHECK_CONDITION(map->node("composite/composite", 0, 12) == reinterpret_cast<Node*>(6));
	CHECK_CONDITION(map->node("composite/composite", 0, 13) == reinterpret_cast<Node*>(6));
	CHECK_CONDITION(map->node("composite/composite", 0, 14) == reinterpret_cast<Node*>(6));

	// empty space
	CHECK_CONDITION(map->node("composite/composite", 0, 15) == reinterpret_cast<Node*>(5));

	// arg1
	CHECK_CONDITION(map->node("composite/composite", 0, 16) == reinterpret_cast<Node*>(7));
	CHECK_CONDITION(map->node("composite/composite", 0, 17) == reinterpret_cast<Node*>(7));
	CHECK_CONDITION(map->node("composite/composite", 0, 18) == reinterpret_cast<Node*>(7));
	CHECK_CONDITION(map->node("composite/composite", 0, 19) == reinterpret_cast<Node*>(7));

	// ", "
	CHECK_CONDITION(map->node("composite/composite", 0, 20) == reinterpret_cast<Node*>(4));
	CHECK_CONDITION(map->node("composite/composite", 0, 21) == reinterpret_cast<Node*>(4));

	// type2
	CHECK_CONDITION(map->node("composite/composite", 0, 22) == reinterpret_cast<Node*>(9));
	CHECK_CONDITION(map->node("composite/composite", 0, 23) == reinterpret_cast<Node*>(9));
	CHECK_CONDITION(map->node("composite/composite", 0, 24) == reinterpret_cast<Node*>(9));
	CHECK_CONDITION(map->node("composite/composite", 0, 25) == reinterpret_cast<Node*>(9));
	CHECK_CONDITION(map->node("composite/composite", 0, 26) == reinterpret_cast<Node*>(9));

	// empty space
	CHECK_CONDITION(map->node("composite/composite", 0, 27) == reinterpret_cast<Node*>(8));

	// arg2
	CHECK_CONDITION(map->node("composite/composite", 0, 28) == reinterpret_cast<Node*>(10));
	CHECK_CONDITION(map->node("composite/composite", 0, 29) == reinterpret_cast<Node*>(10));
	CHECK_CONDITION(map->node("composite/composite", 0, 30) == reinterpret_cast<Node*>(10));
	CHECK_CONDITION(map->node("composite/composite", 0, 31) == reinterpret_cast<Node*>(10));

	// )
	CHECK_CONDITION(map->node("composite/composite", 0, 32) == reinterpret_cast<Node*>(4));

	// newline
	CHECK_CONDITION(map->node("composite/composite", 0, 33) == reinterpret_cast<Node*>(11));

	// any other number on the same line
	CHECK_CONDITION(map->node("composite/composite", 0, 34) == nullptr);

	// LINE 1

	// {
	CHECK_CONDITION(map->node("composite/composite", 1, 0) == reinterpret_cast<Node*>(11));
	// newline
	CHECK_CONDITION(map->node("composite/composite", 1, 1) == reinterpret_cast<Node*>(11));
	// any other number on the same line
	CHECK_CONDITION(map->node("composite/composite", 1, 2) == nullptr);

	// LINE 2

	// \t
	CHECK_CONDITION(map->node("composite/composite", 2, 0) == reinterpret_cast<Node*>(12));
	// statemet1
	CHECK_CONDITION(map->node("composite/composite", 2, 1) == reinterpret_cast<Node*>(12));
	CHECK_CONDITION(map->node("composite/composite", 2, 2) == reinterpret_cast<Node*>(12));
	CHECK_CONDITION(map->node("composite/composite", 2, 3) == reinterpret_cast<Node*>(12));
	CHECK_CONDITION(map->node("composite/composite", 2, 4) == reinterpret_cast<Node*>(12));
	CHECK_CONDITION(map->node("composite/composite", 2, 5) == reinterpret_cast<Node*>(12));
	CHECK_CONDITION(map->node("composite/composite", 2, 6) == reinterpret_cast<Node*>(12));
	CHECK_CONDITION(map->node("composite/composite", 2, 7) == reinterpret_cast<Node*>(12));
	CHECK_CONDITION(map->node("composite/composite", 2, 8) == reinterpret_cast<Node*>(12));
	CHECK_CONDITION(map->node("composite/composite", 2, 9) == reinterpret_cast<Node*>(12));
	CHECK_CONDITION(map->node("composite/composite", 2, 10) == reinterpret_cast<Node*>(12));

	// new line
	CHECK_CONDITION(map->node("composite/composite", 2, 11) == reinterpret_cast<Node*>(11));
	// any other number on the same line
	CHECK_CONDITION(map->node("composite/composite", 2, 12) == nullptr);

	// LINE 3

	// \t
	CHECK_CONDITION(map->node("composite/composite", 3, 0) == reinterpret_cast<Node*>(13));
	// statemet1
	CHECK_CONDITION(map->node("composite/composite", 3, 1) == reinterpret_cast<Node*>(13));
	CHECK_CONDITION(map->node("composite/composite", 3, 2) == reinterpret_cast<Node*>(13));
	CHECK_CONDITION(map->node("composite/composite", 3, 3) == reinterpret_cast<Node*>(13));
	CHECK_CONDITION(map->node("composite/composite", 3, 4) == reinterpret_cast<Node*>(13));
	CHECK_CONDITION(map->node("composite/composite", 3, 5) == reinterpret_cast<Node*>(13));
	CHECK_CONDITION(map->node("composite/composite", 3, 6) == reinterpret_cast<Node*>(13));
	CHECK_CONDITION(map->node("composite/composite", 3, 7) == reinterpret_cast<Node*>(13));
	CHECK_CONDITION(map->node("composite/composite", 3, 8) == reinterpret_cast<Node*>(13));
	CHECK_CONDITION(map->node("composite/composite", 3, 9) == reinterpret_cast<Node*>(13));
	CHECK_CONDITION(map->node("composite/composite", 3, 10) == reinterpret_cast<Node*>(13));

	// new line
	CHECK_CONDITION(map->node("composite/composite", 3, 11) == reinterpret_cast<Node*>(11));

	// any other number on the same line
	CHECK_CONDITION(map->node("composite/composite", 3, 12) == nullptr);

	// LINE 4

	// \t
	CHECK_CONDITION(map->node("composite/composite", 4, 0) == reinterpret_cast<Node*>(14));
	// {
	CHECK_CONDITION(map->node("composite/composite", 4, 1) == reinterpret_cast<Node*>(14));
	// newline
	CHECK_CONDITION(map->node("composite/composite", 4, 2) == reinterpret_cast<Node*>(14));
	// any other number on the same line
	CHECK_CONDITION(map->node("composite/composite", 4, 3) == nullptr);

	// LINE 5

	// \t\t
	CHECK_CONDITION(map->node("composite/composite", 5, 0) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 1) == reinterpret_cast<Node*>(15));
	// subStatemet1
	CHECK_CONDITION(map->node("composite/composite", 5, 2) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 3) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 4) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 5) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 6) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 7) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 8) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 9) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 10) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 11) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 12) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 13) == reinterpret_cast<Node*>(15));
	CHECK_CONDITION(map->node("composite/composite", 5, 14) == reinterpret_cast<Node*>(15));

	// new line
	CHECK_CONDITION(map->node("composite/composite", 5, 15) == reinterpret_cast<Node*>(14));
	// any other number on the same line
	CHECK_CONDITION(map->node("composite/composite", 5, 16) == nullptr);

	// LINE 6

	// \t\t
	CHECK_CONDITION(map->node("composite/composite", 6, 0) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 1) == reinterpret_cast<Node*>(16));
	// subStatemet1
	CHECK_CONDITION(map->node("composite/composite", 6, 2) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 3) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 4) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 5) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 6) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 7) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 8) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 9) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 10) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 11) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 12) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 13) == reinterpret_cast<Node*>(16));
	CHECK_CONDITION(map->node("composite/composite", 6, 14) == reinterpret_cast<Node*>(16));

	// new line
	CHECK_CONDITION(map->node("composite/composite", 6, 15) == reinterpret_cast<Node*>(14));
	// any other number on the same line
	CHECK_CONDITION(map->node("composite/composite", 6, 16) ==  nullptr);

	// LINE 7

	// \t
	CHECK_CONDITION(map->node("composite/composite", 7, 0) == reinterpret_cast<Node*>(14));
	// }
	CHECK_CONDITION(map->node("composite/composite", 7, 1) == reinterpret_cast<Node*>(14));
	// new line
	CHECK_CONDITION(map->node("composite/composite", 7, 2) == reinterpret_cast<Node*>(11));
	// any other number on the same line
	CHECK_CONDITION(map->node("composite/composite", 7, 3) == nullptr);

	// LINE 8

	// }
	CHECK_CONDITION(map->node("composite/composite", 8, 0) == reinterpret_cast<Node*>(11));
	// new line
	CHECK_CONDITION(map->node("composite/composite", 8, 1) == nullptr);
	// any other number on the same line
	CHECK_CONDITION(map->node("composite/composite", 8, 2) == nullptr);

	// LINE 9 (does not exist)
	CHECK_CONDITION(map->node("composite/composite", 9, 0) == nullptr);

	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/composite/composite", testDir +"/composite/composite");
}};

}
