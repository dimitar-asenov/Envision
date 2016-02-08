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

#include "CppImportManager.h"

#include "ClangFrontendActionFactory.h"
#include "CppImportException.h"

#include "ModelBase/src/model/TreeManager.h"

namespace CppImport {

CppImportManager::~CppImportManager()
{
	// clean the maps
	auto vectors = sourcesMap_.values();
	for (auto sv : vectors)
		SAFE_DELETE(sv);
	sourcesMap_.clear();
	SAFE_DELETE(compilationDb_);
}

void CppImportManager::setImportPaths(QStringList sourcePaths, const bool subProjects)
{
	projectPath_ = mainProjectPath(sourcePaths.first());
	compilationDb_ = findCompilationDatabase(sourcePaths.first());

	for (auto relativeSourcePath : sourcePaths)
	{
		auto sourcePath = QDir{relativeSourcePath}.absolutePath();
		if (subProjects)
		{
			QDirIterator dirIterator(sourcePath, QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
			while (dirIterator.hasNext())
				initPath(dirIterator.next());
		}
		else
			initPath(sourcePath);
	}
}

clang::tooling::CompilationDatabase* CppImportManager::findCompilationDatabase(const QString& path)
{
	clang::tooling::CompilationDatabase* result{};

	QDir dir{path};
	do
	{
		std::string Error;
		result = clang::tooling::CompilationDatabase::loadFromDirectory(dir.absolutePath().toLatin1().data(),
																										Error).release();
		if (!result && !dir.cdUp())
			throw CppImportException{"No compilation database found: " + QString::fromStdString(Error)};
	} while (!result);

	return result;
}

Model::TreeManager* CppImportManager::createTreeManager(const bool statisticsPerProject)
{
	auto project = new OOModel::Project{projectPath_.split(QDir::separator()).last()};
	auto log = new CppImportLogger{};
	auto visitor = new ClangAstVisitor{project, projectPath_, log};

	for (QString s : projects_)
	{
		qDebug() << "Start processing project :" << s;
		auto tool = new clang::tooling::ClangTool{*compilationDb_, *sourcesMap_.value(s)};
		auto frontendActionFactory = new ClangFrontendActionFactory{visitor, log};
		tool->run(frontendActionFactory);
		// statistics
		if (statisticsPerProject)
			log->outputStatistics();
		//clean up
		SAFE_DELETE(frontendActionFactory);
		SAFE_DELETE(tool);
	}

	visitor->endEntireImport();

	// statistics
	if (!statisticsPerProject)
		log->outputStatistics();
	SAFE_DELETE(visitor);
	SAFE_DELETE(log);
	// reset the path (because clang tool changes it)
	QDir::setCurrent(qApp->applicationDirPath());
	return new Model::TreeManager{"CppImport", project};
}

void CppImportManager::setupTest()
{
	// setup the root dir
	QDir rootDir(QDir::currentPath());
	rootDir.cdUp();
	rootDir.cd(QString{"CppImport"}.append(QDir::separator()).append("test"));
	QString rootPath = rootDir.canonicalPath();
	// open testSelector file to read in dir
	QFile selector(rootPath + QDir::separator() + "testSelector");
	if (!selector.open(QFile::ReadOnly))
		throw CppImportException{"Could not open testSelector file"};
	QTextStream inStream(&selector);
	QString testDir;
	while (!inStream.atEnd())
	{
		testDir = inStream.readLine();
		if (testDir.trimmed().isEmpty())
			continue;
		else if (testDir.startsWith("path:"))
		{
			testDir.replace(0, 5, "");
			return setImportPaths(testDir.split(':'));
		}
		else if (testDir.startsWith("spath:"))
		{
			testDir.replace(0, 6, "");
			return setImportPaths(testDir.split(':'), true);
		}
		else if (!testDir.startsWith("#"))
		{
			rootPath.append(QDir::separator()).append(testDir);
			createCompilationDbForTest(rootPath);
			return setImportPaths({rootPath});
		}
	}
	throw CppImportException{"No test case set in the testSelector file"};
}

void CppImportManager::initPath(const QString& sourcePath)
{
	// check if there is a compilation db file in this directory otherwise we exclude it
	if (!compilationDb_)
	{
		qDebug() << "Ignoring directory" << sourcePath << "because there is no compile_commands.json file";
		return;
	}
	// append to the projects
	projects_.append(sourcePath);

	readInFiles(sourcePath);
}

QString CppImportManager::mainProjectPath(const QString& path)
{
	QDir dir{path};
	auto result = dir.absolutePath();

	do
	{
		auto currentPath = dir.absolutePath();
		if (QFile::exists(currentPath + QDir::separator() + "CMakeLists.txt"))
			result = currentPath;
	} while (dir.cdUp());

	return result;
}

void CppImportManager::readInFiles(const QString& sourcePath)
{
	if (!sourcePath.endsWith(".cpp"))
	{
		QDirIterator dirIterator(sourcePath, cppFilter_, QDir::Files, QDirIterator::Subdirectories);
		auto sources = new std::vector<std::string>{};
		while (dirIterator.hasNext())
			sources->push_back(dirIterator.next().toStdString());
		sourcesMap_.insert(sourcePath, sources);
	}
	else
		sourcesMap_.insert(sourcePath, new std::vector<std::string>{sourcePath.toStdString()});
}

void CppImportManager::setCompilationDbPath(const QString& sourcePath)
{
	std::string Error;
	auto compDB = clang::tooling::CompilationDatabase::loadFromDirectory(sourcePath.toLatin1().data(), Error);
	if (!compDB) throw CppImportException{"No compilation database found : " + QString::fromStdString(Error)};
}

void CppImportManager::createCompilationDbForTest(const QString& testPath)
{
	QDir curDir(testPath);
	if (!curDir.exists("compile_commands.json"))
	{
		QString test = testPath.split(QDir::separator()).last();
		QFile db(testPath + QDir::separator() + "compile_commands.json");
		if (!db.open(QFile::ReadWrite))
			throw CppImportException{"Cannot create compile_commands file for test: " + test};
		QTextStream out(&db);
		out << "[" << endl;
		out << "{" << "\"directory\":" << "\"" << testPath << "\"," << endl;
		// TODO: this is platform dependent
		out << "\"command\":" << "\"/usr/bin/clang++ -std=c++0x -Irelative " <<
				"-I/usr/lib/llvm-3.4/include/clang/ -c -o test.o test.cpp\"," << endl;
		out << "\"file\":" << "\"test.cpp\"" << endl;
		out << "}" << endl << "]" << endl;
		db.close();
	}
}

}
