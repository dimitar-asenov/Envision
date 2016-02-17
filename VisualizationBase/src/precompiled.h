/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#ifndef PRECOMPILED_VISUALIZATIONBASE_H_
#define PRECOMPILED_VISUALIZATIONBASE_H_

// Include here the precompiled headers of other plug-ins that use this plug-in uses. Only the "public" part of
// those headers will be included here
#include "ModelBase/src/precompiled.h"
#include "Logger/src/precompiled.h"
#include "SelfTest/src/precompiled.h"
#include "Core/src/precompiled.h"
#include "Core/src/global.h"


// Put here includes which appear in header files. This will also be visible to other plug-in which depend on this one
// and will be included in their precompiled headers
#include <QtSvg/QSvgRenderer>

#include <QtXml/QDomDocument>

#include <QtWidgets/QLabel>
#include <QtWidgets/QGraphicsSceneDragDropEvent>
#include <QtWidgets/QGraphicsSceneContextMenuEvent>
#include <QtWidgets/QGraphicsSceneHoverEvent>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsSceneWheelEvent>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>

#include <QtGui/QStaticText>
#include <QtGui/QPainter>
#include <QtGui/QPaintEngine>
#include <QtGui/QFont>
#include <QtGui/QPen>
#include <QtGui/QWheelEvent>
#include <QtGui/QFocusEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>

#include <QtCore/QEvent>
#include <QtCore/QSharedPointer>
#include <QtCore/QPointF>

#include <QtCore/QMultiMap>

#include <cmath>
#include <type_traits>


#if defined(VisualizationBase_EXPORTS)
// Put here includes which only appear in compilation units and do not appear in headers. Precompiled headers of
// plug-ins which depend on this one will not include these headers.
#include <QtWebKitWidgets/QGraphicsWebView>
#include <QtWebKitWidgets/QWebFrame>

#include <QtSvg/QSvgGenerator>

#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>

#include <QtGui/QLinearGradient>
#include <QtGui/QRadialGradient>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QColor>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>


#endif

#endif /* PRECOMPILED_VISUALIZATIONBASE_H_ */
