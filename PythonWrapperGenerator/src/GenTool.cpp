/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "GenTool.h"

#include "GeneratorAction.h"
#include "APIData.h"
#include "APIPrinter.h"
#include "Config.h"

class ClangFrontEndActionFactory : public clang::tooling::FrontendActionFactory
{
	public:
		virtual clang::FrontendAction* create() override {
			return new GeneratorAction();
		}
};

void GenTool::run()
{
	auto pathToNamespaceMap = Config::instance().subdirsToNamespaceMap();

	for (const auto& project : projects_)
	{
		APIData::instance().includePrefix_ = project.split(QDir::separator(),
																			QString::SplitBehavior::SkipEmptyParts).last();
		auto it = pathToNamespaceMap.find(APIData::instance().includePrefix_);
		if (it != pathToNamespaceMap.end())
		{
			APIData::instance().namespaceName_ = it.value();
			std::cout << "Start processing project :" << project.toStdString() << std::endl;
			clang::tooling::ClangTool tool (*compilationDbMap_.value(project), sourcesMap_[project]);
			ClangFrontEndActionFactory frontendActionFactory;
			tool.run(&frontendActionFactory);
		}
	}

	APIPrinter printer;
	printer.print();
}

void GenTool::setSubDirPath(const QString& path)
{
	QDirIterator dirIterator(path, QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
	while (dirIterator.hasNext()) initPath(dirIterator.next());
}

void GenTool::initPath(const QString& sourcePath)
{
	// check if there is a compilation db file in this directory otherwise we exclude it
	QDir curDir(sourcePath);
	if (!curDir.exists("compile_commands.json"))
	{
		std::cout << "Ignoring directory" << sourcePath.toStdString()
					 << "because there is no compile_commands.json file" << std::endl;
		return;
	}
	// append to the projects
	projects_ << sourcePath;

	readInFiles(sourcePath);
	setCompilationDbPath(sourcePath);
}

void GenTool::readInFiles(const QString& sourcePath)
{
	QDirIterator dirIterator(sourcePath, cppFilter_, QDir::Files, QDirIterator::Subdirectories);
	auto& sources = sourcesMap_[sourcePath];
	while (dirIterator.hasNext()) sources.push_back(dirIterator.next().toStdString());
}

void GenTool::setCompilationDbPath(const QString& sourcePath)
{
	std::string Error;
	auto compDB = clang::tooling::CompilationDatabase::loadFromDirectory(sourcePath.toLatin1().data(), Error);
	Q_ASSERT(compDB);
	compilationDbMap_.insert(sourcePath, move(compDB));
}
