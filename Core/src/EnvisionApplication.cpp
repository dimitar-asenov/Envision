/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "EnvisionApplication.h"

#include "EnvisionManager.h"

namespace Core {

QMap<void*, EnvisionApplication::IdleFunction>& EnvisionApplication::idleActions()
{
	static QMap<void*, EnvisionApplication::IdleFunction> actions;
	return actions;
}

EnvisionApplication::EnvisionApplication(int& argc, char** argv) : QApplication{argc, argv}
{
	idleInputTimer_.setInterval(50);
	idleInputTimer_.setSingleShot(true);
	connect(&idleInputTimer_, &QTimer::timeout, this, &EnvisionApplication::userInputIdle);
}

bool EnvisionApplication::notify(QObject* receiver, QEvent* event)
{
	// This list is meant to represent user interaction events
	if ( event->spontaneous() && (
			  event->type() == QEvent::KeyPress
			  || event->type() == QEvent::KeyRelease
			  || event->type() == QEvent::DragEnter
			  || event->type() == QEvent::DragLeave
			  || event->type() == QEvent::DragMove
			  || event->type() == QEvent::Drop
			  || event->type() == QEvent::Enter
			  || event->type() == QEvent::Gesture
			  || event->type() == QEvent::HoverEnter
			  || event->type() == QEvent::HoverLeave
			  || event->type() == QEvent::HoverMove
			  || event->type() == QEvent::Leave
			  || event->type() == QEvent::MouseButtonDblClick
			  || event->type() == QEvent::MouseButtonPress
			  || event->type() == QEvent::MouseButtonRelease
			  || event->type() == QEvent::MouseMove
			  || event->type() == QEvent::NativeGesture
			  || event->type() == QEvent::Wheel
		  ))
		idleInputTimer_.start();

	EnvisionManager::processPreEventActions(receiver, event);
	auto res = QApplication::notify(receiver, event);
	EnvisionManager::processPostEventActions(receiver, event);
	return res;
}

void EnvisionApplication::addOnUserInputIdleAction(void* actionId, IdleFunction action)
{
	idleActions().insert(actionId, action);
}

void EnvisionApplication::removeOnUserInputIdleAction(void* actionId)
{
	idleActions().remove(actionId);
}

void EnvisionApplication::userInputIdle()
{
	for (auto& a : idleActions().values()) a();
}

}
