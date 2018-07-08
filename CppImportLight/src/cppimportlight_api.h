#pragma once

#include "precompiled.h"

// This should be defined in the project file of the plug-in that exports symbols
#if defined(CppImportLight_EXPORTS)
	#define CPPIMPORTLIGHT_API Q_DECL_EXPORT
#else
	#define CPPIMPORTLIGHT_API Q_DECL_IMPORT
#endif
