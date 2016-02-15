/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "CppExportPlugin.h"
#include "exporter/CppExporter.h"

#include "VisualizationBase/src/items/Item.h"
#include "SelfTest/src/TestManager.h"
#include "Core/src/Profiler.h"

namespace CppExport {

bool CppExportPlugin::initialize(Core::EnvisionManager&)
{
	Interaction::ActionRegistry::instance()->registerInputHandler("GenericHandler.TestCppExport", testExport);

	return true;
}

void CppExportPlugin::unload()
{
}

void CppExportPlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<CppExportPlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<CppExportPlugin>::runTest(testid).printResultStatistics();
}

bool CppExportPlugin::testExport(Visualization::Item *target, QKeySequence, Interaction::ActionRegistry::InputState)
{
	auto n = target;
	while (n && ! n->node()) n = n->parent();

	if (n)
		if (n->node())
		{
			Core::Profiler::start(true, "Cpp Export", "cpp-export.prof");
			CppExport::CppExporter::exportTree(n->node()->manager(), "exported");
			Core::Profiler::stop("Cpp Export");
			return true;
		}

	return false;
}

}
