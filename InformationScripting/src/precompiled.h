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

#ifndef PRECOMPILED_INFORMATIONSCRIPTING_H_
#define PRECOMPILED_INFORMATIONSCRIPTING_H_

// TODO: Include here the precompiled headers of other plug-ins that use this plug-in uses. Only the "public" part of
// hose headers will be included here
#include "OOInteraction/src/precompiled.h"
#include "InteractionBase/src/precompiled.h"
#include "OOVisualization/src/precompiled.h"
#include "VisualizationBase/src/precompiled.h"
#include "OOModel/src/precompiled.h"
#include "ModelBase/src/precompiled.h"
#include "Logger/src/precompiled.h"
#include "SelfTest/src/precompiled.h"
#include "Core/src/precompiled.h"

#if defined __cplusplus
// Add C++ includes here

// Put here includes which appear in header files. This will also be visible to other plug-in which depend on this one
// and will be included in their precompiled headers


#if defined(INFORMATIONSCRIPTING_LIBRARY)
// Put here includes which only appear in compilation units and do not appear in headers. Precompiled headers of
// plug-ins which depend on this one will not include these headers.

#ifdef slots
#undef slots
#define PYTHONQT_RESTORE_KEYWORDS
#endif

#include <boost/python.hpp>

#ifdef PYTHONQT_RESTORE_KEYWORDS
#define slots Q_SLOTS
#undef PYTHONQT_RESTORE_KEYWORDS
#endif

#include <QtCore/QQueue>

#endif

#endif

#endif /* PRECOMPILED_INFORMATIONSCRIPTING_H_ */
