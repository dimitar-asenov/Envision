#ifndef PRECOMPILED_PLUGINNAME_UPPERCASE_H_
#define PRECOMPILED_PLUGINNAME_UPPERCASE_H_

// TODO: Include here the precompiled headers of other plug-ins that use this plug-in uses. Only the "public" part of
// those headers will be included here
#include "ModelBase/src/precompiled.h"
#include "Logger/src/precompiled.h"
#include "SelfTest/src/precompiled.h"
#include "Core/src/precompiled.h"
#include "Core/src/global.h"


// Put here includes which appear in header files. This will also be visible to other plug-in which depend on this one
// and will be included in their precompiled headers


#if defined(PLUGINNAME_EXPORTS)
// Put here includes which only appear in compilation units and do not appear in headers. Precompiled headers of
// plug-ins which depend on this one will not include these headers.


#endif

#endif /* PRECOMPILED_PLUGINNAME_UPPERCASE_H_ */
