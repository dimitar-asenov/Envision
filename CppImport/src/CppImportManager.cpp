/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "CppImportManager.h"

#include "ClangFrontendActionFactory.h"
#include "CppImportException.h"

namespace CppImport {

CppImportManager::~CppImportManager()
{
	SAFE_DELETE(compilationDB_);
	SAFE_DELETE(myTool_);
}

bool CppImportManager::setImportPath(const QString& sourcePath, const QString& compilationDbPath)
{
	// set a filter to only get files which are c++ sources
	QStringList cppFilter;
	cppFilter << "*.cpp" << "*.cc" << "*.cxx";
	QDirIterator dirIterator(sourcePath, cppFilter, QDir::Files, QDirIterator::Subdirectories);
	while(dirIterator.hasNext())
		sources_.push_back(dirIterator.next().toStdString());
	if(compilationDbPath.isEmpty())
		return setCompilationDbPath(sourcePath);
	return setCompilationDbPath(compilationDbPath);
}

Model::Model*CppImportManager::createModel()
{
	myTool_ = new clang::tooling::ClangTool(*compilationDB_,sources_);
	OOModel::Project* project = new OOModel::Project("CppImport");
	ClangFrontendActionFactory* frontendActionFactory = new ClangFrontendActionFactory(project);
	myTool_->run(frontendActionFactory);
	frontendActionFactory->outputStatistics();
	SAFE_DELETE(frontendActionFactory);
	// reset the path (because clang tool changes it)
	QDir::setCurrent(qApp->applicationDirPath());
	// remove the test file if there is one
	QDir rootDir(QDir::currentPath());
	rootDir.cdUp();
	rootDir.cd(QString("CppImport").append(QDir::separator()).append("test"));
	QFile::remove(rootDir.canonicalPath() + QDir::separator() + "test.cpp");
	return new Model::Model("CppImport", project);
}

bool CppImportManager::setupTest()
{
	// setup the root dir
	QDir rootDir(QDir::currentPath());
	rootDir.cdUp();
	rootDir.cd(QString("CppImport").append(QDir::separator()).append("test"));
	QString rootPath = rootDir.canonicalPath();
	// open testSelector file to read in dir
	QFile selector(rootPath + QDir::separator() + "testSelector");
	if(!selector.open(QFile::ReadOnly))
		throw CppImportException("Could not open testSelector file");
	QTextStream inStream(&selector);
	QString testDir;
	bool isPath = false;
	while(!inStream.atEnd())
	{
		testDir = inStream.readLine();
		if(testDir.startsWith("path:"))
		{
			testDir.replace(0,5,"");
			isPath = true;
			break;
		}
		if(!testDir.startsWith("#"))
			break;
	}
	if(testDir.isEmpty())
		throw CppImportException("No test case uncommented in the testSelector file");
	if(isPath)
		rootPath = testDir;
	else
	{
		// copy the test file to the root dir
		QFile testFile(rootPath + QDir::separator() + testDir + QDir::separator() + "test.cpp");
		if(!testFile.copy(rootPath + QDir::separator() + "test.cpp"))
			throw CppImportException("Could not copy test.cpp file from " + testDir);
	}

	return setImportPath(rootPath);
}

bool CppImportManager::setCompilationDbPath(const QString& path)
{
	std::string Error;
	if(!(compilationDB_ = clang::tooling::CompilationDatabase::loadFromDirectory(path.toAscii().data(),Error)))
		throw CppImportException("No compilation database found : " + QString::fromStdString(Error));
	return true;
}

}
