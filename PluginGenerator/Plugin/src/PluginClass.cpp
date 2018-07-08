#include "PLUGINNAMEPlugin.h"
#include "SelfTest/src/TestManager.h"
#include "Logger/src/Log.h"

namespace NAMESPACE {

Logger::Log& PLUGINNAMEPlugin::log()
{
	static auto log = Logger::Log::getLogger("PLUGIN_NAME_LOWER");
	return *log;
}

bool PLUGINNAMEPlugin::initialize(Core::EnvisionManager&)
{
	return true;
}

void PLUGINNAMEPlugin::unload()
{
}

void PLUGINNAMEPlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<PLUGINNAMEPlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<PLUGINNAMEPlugin>::runTest(testid).printResultStatistics();
}

}
