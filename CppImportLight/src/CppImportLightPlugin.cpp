#include "CppImportLightPlugin.h"
#include "Parser.h"

#include "InteractionBase/src/commands/COpenDir.h"
#include "SelfTest/src/TestManager.h"
#include "Logger/src/Log.h"

namespace CppImportLight {

using Interaction::COpenDir;

Logger::Log& CppImportLightPlugin::log()
{
	static auto log = Logger::Log::getLogger("PLUGIN_NAME_LOWER");
	return *log;
}

bool CppImportLightPlugin::initialize(Core::EnvisionManager&)
{
	COpenDir::registerFileParser(std::make_unique<CppParser>());
	return true;
}

void CppImportLightPlugin::unload()
{
}

void CppImportLightPlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<CppImportLightPlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<CppImportLightPlugin>::runTest(testid).printResultStatistics();
}

}
