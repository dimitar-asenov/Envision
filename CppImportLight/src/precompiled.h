#ifndef PRECOMPILED_CPPIMPORTLIGHT_H_
#define PRECOMPILED_CPPIMPORTLIGHT_H_

// TODO: Include here the precompiled headers of other plug-ins that this plug-in uses. Only the "public" part of
// those headers will be included here
#include "OOInteraction/src/precompiled.h"
#include "InteractionBase/src/precompiled.h"
#include "OOVisualization/src/precompiled.h"
#include "VisualizationBase/src/precompiled.h"
#include "OOModel/src/precompiled.h"
#include "ModelBase/src/precompiled.h"
#include "Logger/src/precompiled.h"
#include "SelfTest/src/precompiled.h"
#include "Core/src/precompiled.h"
#include "Core/src/global.h"


// Put here includes which appear in header files. This will also be visible to other plug-in which depend on this one
// and will be included in their precompiled headers


#if defined(CppImportLight_EXPORTS)
// Put here includes which only appear in compilation units and do not appear in headers. Precompiled headers of
// plug-ins which depend on this one will not include these headers.

#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>

#endif

#endif /* PRECOMPILED_CPPIMPORTLIGHT_H_ */
