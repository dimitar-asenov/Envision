#pragma once

#include "precompiled.h"

// This should be defined in the project file of the plug-in that exports symbols
#if defined(PLUGINNAME_EXPORTS)
	#define PLUGINNAME_UPPERCASE_API Q_DECL_EXPORT
#else
	#define PLUGINNAME_UPPERCASE_API Q_DECL_IMPORT
#endif
