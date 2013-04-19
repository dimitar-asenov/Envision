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

namespace CppImport {

CppImportManager::~CppImportManager()
{
	delete compilationDB_;
	delete myTool_;
}

bool CppImportManager::setSrcPath(QString& path)
{
	QDir dir(path);
	// set a filter to only get files which are c++ sources
	QStringList cppFilter;
	cppFilter << "*.cpp" << "*.cc" << "*.cxx";
	dir.setNameFilters(cppFilter);
	dir.setFilter(QDir::Files);
	QStringList files = dir.entryList();
	// add all files found to sources vector
	foreach (QString file, files)
	{
		sources_.push_back(dir.absoluteFilePath(file).toStdString());
	}
	return setCompilationDbPath(path);
}

void CppImportManager::createModel()
{
	myTool_ = new clang::tooling::ClangTool(*compilationDB_,sources_);
	// run overtakes pointer so no need to free it later
	myTool_->run(clang::tooling::newFrontendActionFactory<ClangConsumerFactory>());
	ClangConsumerFactory::model_->endModification();
}

bool CppImportManager::setCompilationDbPath(QString& path)
{
	std::string Error = "DATABASE NOT OK";
	compilationDB_ = nullptr;
	compilationDB_ = clang::tooling::CompilationDatabase::loadFromDirectory(path.toAscii().data(),Error);

	if(!compilationDB_)
	{
		std::cout << "ERROR NO COMPDB" << std::endl;
		return false;
	}
	return true;
}

}
