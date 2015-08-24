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

#include "VCTestProject.h"

namespace FilePersistence {

VCTestProject::VCTestProject(const QString& testDir, const QString& projectName)
	: testDir_{testDir}, projectName_{projectName == QString{} ? testDir : projectName}, repo_{init()}
{
}

QString VCTestProject::init()
{
	auto allTestsRootDir = QDir::tempPath() + "/Envision/FilePersistence/tests/VCTestProject/";
	auto testBaseDir = allTestsRootDir + testDir_;
	auto testSourceDir = testBaseDir + "/repo-source";
	auto testRepoDir = testBaseDir + "/" + projectName_;

	QDir{testSourceDir}.removeRecursively();
	QDir{testSourceDir}.mkpath(testSourceDir);
	QDir{testRepoDir}.removeRecursively();

	auto repoScript = allTestsRootDir + "/create-test-git-repo.py";
	QFile{repoScript}.remove();
	QFile::copy(":/FilePersistence/test/persisted/version-control/create-test-git-repo.py", repoScript );

	QDir sourceDir{":/FilePersistence/test/persisted/version-control/" + testDir_};
	for (auto sourceFile : sourceDir.entryInfoList(QDir::Files))
		QFile::copy(sourceFile.absoluteFilePath(), testSourceDir + "/" + sourceFile.fileName());

	auto ret = std::system(QString{"python \"" + repoScript + "\" \"" + testSourceDir + "\" \"" + testRepoDir + "\""}
					.toLatin1());

	Q_ASSERT(ret == 0);

	return testRepoDir;
}

}
