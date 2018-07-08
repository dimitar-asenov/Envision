#pragma once

#include "Core/src/EnvisionPlugin.h"
#include "cppimportlight_api.h"

namespace Logger {
class Log;
}

namespace CppImportLight {

/**
 * Implements the interface between the CppImportLight plug-in and Envision.
 *
 * The Envision core will use this interface to communicate with the plug-in. The plug-in will be initialized before
 * any other operations are performed.
 *
 * The plug-in can use the supplied EnvisionManager object to find out more about the running environment.
 */
class CppImportLightPlugin : public QObject, public Core::EnvisionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "EnvisionPlugin/1.0")
	Q_INTERFACES(Core::EnvisionPlugin)

	public:
		virtual bool initialize(Core::EnvisionManager&) override;
		virtual void unload() override;
		virtual void selfTest(QString testid) override;

		static Logger::Log& log();
};

}
