/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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
#include "../src/FilePersistencePlugin.h"
#include "../src/version_control/ChangeDescription.h"
#include "../src/version_control/Diff.h"
#include "../src/version_control/GitRepository.h"

#include "SelfTest/src/Test.h"
#include "SelfTest/src/TestAssertions.h"
#include <QStringList>

namespace FilePersistence {

class FILEPERSISTENCE_API RegexSearchInCommit
 : public SelfTest::Test<FilePersistencePlugin, RegexSearchInCommit> { public: void test()
{
	 qDebug() << "Check filepersistence";
	 GitRepository gitrepo{"projects/testProject"};

//	 QStringList rev = gitrepo.revisions();

//	 for (int i = 0; i < rev.size(); ++i)
//				qDebug() << rev.at(i).toLocal8Bit().constData();

	 const Commit* commit = gitrepo.getCommit("008414441f329f826e3796deab2eb7ad5e865746");
	 QStringList reg = commit->nodeLinesFromId("ca52af4b-250e-4c03-a1f5-492eca38f9ba", 0);

	 for (int i = 0; i < reg.size(); ++i)
				qDebug() << i << " " << reg.at(i).toLocal8Bit().constData();
	 CHECK_CONDITION(3==3);
}};
}
