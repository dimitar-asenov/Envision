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
#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"

#include "../src/writer/Exporter.h"
#include "../src/writer/FragmentLayouter.h"
#include "../src/writer/TextToNodeMap.h"
#include "../src/tree/SourceDir.h"
#include "../src/tree/SourceFile.h"
#include "../src/tree/TextFragment.h"
#include "../src/tree/CompositeFragment.h"

using namespace Model;

namespace Export {

class ExportJustText : public SelfTest::Test<ExportPlugin, ExportJustText> { public: void test()
{
	QString testDir = QDir::tempPath() + "/Envision/Export/tests";
	FragmentLayouter layouter{"\t"};

	SourceDir root{nullptr, "text"};
	auto& file = root.file("text");
	file.append( reinterpret_cast<Node*>(1), "Test text" ); // The pointer is irrelevant for now

	Exporter::exportToFileSystem(testDir, &root, &layouter, Exporter::ExportSpan::AllFiles);

	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text", testDir +"/text/text");
}};

class ExportAndModify : public SelfTest::Test<ExportPlugin, ExportAndModify> { public: void test()
{
	QString testDir = QDir::tempPath() + "/Envision/Export/tests";
	FragmentLayouter layouter{"\t"};

	SourceDir root{nullptr, "text"};
	auto& file = root.file("text");
	file.append( reinterpret_cast<Node*>(1), "Test text" ); // The pointer is irrelevant for now

	Exporter::exportToFileSystem(testDir, &root, &layouter, Exporter::ExportSpan::AllFiles);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text", testDir +"/text/text");

	file.append( reinterpret_cast<Node*>(2), ". Now modified" ); // The pointer is irrelevant for now

	Exporter::exportToFileSystem(testDir, &root, &layouter, Exporter::ExportSpan::AllFiles);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text_modified", testDir +"/text/text");
}};

class ExportAndDeleteFile : public SelfTest::Test<ExportPlugin, ExportAndDeleteFile> { public: void test()
{
	QString testDir = QDir::tempPath() + "/Envision/Export/tests";
	FragmentLayouter layouter{"\t"};

	SourceDir root{nullptr, "text"};
	auto& file = root.file("text");
	file.append( reinterpret_cast<Node*>(1), "Test text" ); // The pointer is irrelevant for now

	Exporter::exportToFileSystem(testDir, &root, &layouter, Exporter::ExportSpan::AllFiles);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text", testDir +"/text/text");

	SourceDir root2{nullptr, "text"};
	auto& file2 = root2.file("text2");
	file2.append( reinterpret_cast<Node*>(2), "Test text. Now modified" ); // The pointer is irrelevant for now

	Exporter::exportToFileSystem(testDir, &root2, &layouter, Exporter::ExportSpan::AllFiles);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text_modified", testDir +"/text/text2");
	CHECK_CONDITION(!QFile{testDir +"/text/text"}.exists());
}};

class ExportAndDeleteDir : public SelfTest::Test<ExportPlugin, ExportAndDeleteDir> { public: void test()
{
	QString testDir = QDir::tempPath() + "/Envision/Export/tests";
	FragmentLayouter layouter{"\t"};

	SourceDir root{nullptr, "text"};
	auto& subDir = root.subDir("sub");
	auto& file = subDir.file("text");
	file.append( reinterpret_cast<Node*>(1), "Test text" ); // The pointer is irrelevant for now

	Exporter::exportToFileSystem(testDir, &root, &layouter, Exporter::ExportSpan::AllFiles);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text", testDir +"/text/sub/text");
	CHECK_CONDITION(QDir{testDir +"/text/sub"}.exists());

	SourceDir root2{nullptr, "text"};
	auto& file2 = root2.file("text");
	file2.append( reinterpret_cast<Node*>(2), "Test text" ); // The pointer is irrelevant for now

	Exporter::exportToFileSystem(testDir, &root2, &layouter, Exporter::ExportSpan::AllFiles);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text", testDir +"/text/text");
	CHECK_CONDITION(!QDir{testDir +"/text/sub"}.exists());
}};

// From http://codeblog.vurdalakov.net/2010/04/how-to-make-qt-thread-sleep.html
class Sleep
{
	public:
	 // Causes the current thread to sleep for msecs milliseconds.
	 static void msleep(unsigned long msecs)
	 {
		  QMutex mutex;
		  mutex.lock();

		  QWaitCondition waitCondition;
		  waitCondition.wait(&mutex, msecs);

		  mutex.unlock();
	 }
};

class ExportIdentical : public SelfTest::Test<ExportPlugin, ExportIdentical> { public: void test()
{
	QString testDir = QDir::tempPath() + "/Envision/Export/tests";
	FragmentLayouter layouter{"\t"};

	SourceDir root{nullptr, "text"};
	auto& file = root.file("text");
	file.append( reinterpret_cast<Node*>(1), "Test text" ); // The pointer is irrelevant for now

	Exporter::exportToFileSystem(testDir, &root, &layouter, Exporter::ExportSpan::AllFiles);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text", testDir +"/text/text");
	auto firstFileModifiedTime = QFileInfo{testDir +"/text/text"}.lastModified();
	Sleep::msleep(1010);

	file.append( reinterpret_cast<Node*>(2), ". Now modified" ); // The pointer is irrelevant for now

	Exporter::exportToFileSystem(testDir, &root, &layouter, Exporter::ExportSpan::AllFiles);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text_modified", testDir +"/text/text");
	auto secondFileModifiedTime = QFileInfo{testDir +"/text/text"}.lastModified();
	Sleep::msleep(1010);

	Exporter::exportToFileSystem(testDir, &root, &layouter, Exporter::ExportSpan::AllFiles);
	CHECK_TEXT_FILES_EQUAL(":/Export/test/data/text/text_modified", testDir +"/text/text");
	auto thirdFileModifiedTime = QFileInfo{testDir +"/text/text"}.lastModified();

	CHECK_CONDITION(firstFileModifiedTime < secondFileModifiedTime);
	CHECK_CONDITION(secondFileModifiedTime == thirdFileModifiedTime);
}};

}
