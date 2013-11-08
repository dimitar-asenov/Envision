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

#include "comments.h"
#include "handlers/HComment.h"
#include "handlers/HCommentDiagram.h"
#include "handlers/HCommentDiagramShape.h"
#include "handlers/HCommentDiagramConnector.h"
#include "handlers/HCommentImage.h"
#include "items/VComment.h"
#include "items/VCommentDiagram.h"
#include "items/VCommentDiagramShape.h"
#include "items/VCommentDiagramConnector.h"
#include "items/VCommentImage.h"

#include "ModelBase/src/test_nodes/TestNodesInitializer.h"
#include "SelfTest/src/SelfTestSuite.h"

Q_EXPORT_PLUGIN2(comments, Comments::Comments)

namespace Comments {

Core::InitializationRegistry& nodeTypeInitializationRegistry()
{
	static Core::InitializationRegistry r;
	return r;
}

Core::InitializationRegistry& itemTypeInitializationRegistry()
{
	static Core::InitializationRegistry r;
	return r;
}

bool Comments::initialize(Core::EnvisionManager&)
{
	nodeTypeInitializationRegistry().initializeAll();
	itemTypeInitializationRegistry().initializeAll();

	VComment::setDefaultClassHandler(HComment::instance());
	VCommentDiagram::setDefaultClassHandler(HCommentDiagram::instance());
	VCommentDiagramShape::setDefaultClassHandler(HCommentDiagramShape::instance());
	VCommentDiagramConnector::setDefaultClassHandler(HCommentDiagramConnector::instance());
	VCommentImage::setDefaultClassHandler(HCommentImage::instance());

	return true;
}

void Comments::unload()
{
}

void Comments::selfTest(QString testid)
{
	TestNodes::nodeTypeInitializationRegistry().initializeAll();
	if (testid.isEmpty()) SelfTest::TestManager<Comments>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<Comments>::runTest(testid).printResultStatistics();
}

}
