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

#include "EnvisionManager.h"
#include "EnvisionException.h"
#include "TestRunner.h"

namespace Core {

EnvisionManager::~EnvisionManager()
{
	exit();
}

QList<EnvisionManager::EventPrePostAction>& EnvisionManager::preEventActions()
{
	static QList<EventPrePostAction> a;
	return a;
}

QList<EnvisionManager::EventPrePostAction>& EnvisionManager::postEventActions()
{
	static QList<EventPrePostAction> a;
	return a;
}

QList<PluginInfo> EnvisionManager::getAllLoadedPluginsInfo()
{
	if (exitSet_) return QList<PluginInfo>();
	if (pm_) return pm_->getAllLoadedPluginsInfo();
	throw EnvisionException{"The Envision Manager has no Plugin Manager set"};
}

QMainWindow* EnvisionManager::getMainWindow()
{
	return mainWindow_;
}

void EnvisionManager::setPluginManager(PluginManager* pm)
{
	pm_ = pm;
}

void EnvisionManager::setMainWindow(QMainWindow* mainWindow)
{
	mainWindow_ = mainWindow;
}


void EnvisionManager::setTestRunner(TestRunner* tr)
{
	testRunner_ = tr;
}

void EnvisionManager::exit()
{
	exitSet_ = true;
}

void EnvisionManager::addPreEventAction(EventPrePostAction action)
{
	preEventActions().append(action);
}

void EnvisionManager::addPostEventAction(EventPrePostAction action)
{
	postEventActions().append(action);
}

void EnvisionManager::processPreEventActions(QObject* receiver, QEvent* event)
{
	for (auto a : preEventActions()) a(receiver, event);
}

void EnvisionManager::processPostEventActions(QObject* receiver, QEvent* event)
{
	for (auto a : postEventActions()) a(receiver, event);
}

bool EnvisionManager::areSelfTestsPending() const
{
	return !testRunner_->requestedTests().isEmpty();
}

}
