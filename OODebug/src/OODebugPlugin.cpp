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

#include "OODebugPlugin.h"
#include "SelfTest/src/TestManager.h"

#include "InteractionBase/src/handlers/HSceneHandlerItem.h"

#include "OOInteraction/src/handlers/HStatement.h"
#include "OOInteraction/src/handlers/HStatementItemList.h"

#include "commands/CJavaCompile.h"
#include "commands/CJavaRun.h"
#include "commands/CJavaDebug.h"
#include "commands/CProbe.h"
#include "run_support/MainMethodFinder.h"
#include "overlays/ConsoleOverlay.h"
#include "handlers/HMoveableOverlay.h"
#include "debugger/JavaDebugger.h"
#include "debugger/ReferenceFinder.h"

#include "Logger/src/Log.h"

namespace OODebug {

Logger::Log& OODebugPlugin::log()
{
	static auto log = Logger::Log::getLogger("oodebug");
	return *log;
}

bool OODebugPlugin::initialize(Core::EnvisionManager&)
{
	Interaction::HSceneHandlerItem::instance()->addCommand(new CJavaCompile{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CJavaRun{});
	Interaction::HSceneHandlerItem::instance()->addCommand(new CJavaDebug{});
	OOInteraction::HStatementItemList::instance()->addCommand(new CProbe{});

	ConsoleOverlay::setDefaultClassHandler(HMoveableOverlay::instance());
	PlotOverlay::setDefaultClassHandler(HMoveableOverlay::instance());


	MainMethodFinder::init();
	ReferenceFinder::init();

	// Register our breakpoint key handler
	// TODO: once we have a better mechanism update this.
	OOInteraction::HStatement::instance()->registerKeyPressHandler(
				[] (Visualization::Item* target, QKeyEvent* event) {
		return JavaDebugger::instance().toggleBreakpoint(target, event);
	});
	// TODO: this should be a general interaction not only on statements.
	OOInteraction::HStatement::instance()->registerKeyPressHandler(
				[] (Visualization::Item* target, QKeyEvent* event) {
		return JavaDebugger::instance().suspend(target, event);
	});
	// TODO: see above
	OOInteraction::HStatement::instance()->registerKeyPressHandler(
				[] (Visualization::Item* target, QKeyEvent* event) {
		return JavaDebugger::instance().resume(target, event);
	});
	// TODO: see above
	OOInteraction::HStatement::instance()->registerKeyPressHandler(
				[] (Visualization::Item* target, QKeyEvent* event) {
		return JavaDebugger::instance().trackVariable(target, event);
	});
	// TODO: see above
	OOInteraction::HStatement::instance()->registerKeyPressHandler(
				[] (Visualization::Item* target, QKeyEvent* event) {
		return JavaDebugger::instance().step(target, event);
	});

	return true;
}

void OODebugPlugin::unload()
{
}

void OODebugPlugin::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OODebugPlugin>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OODebugPlugin>::runTest(testid).printResultStatistics();
}

}
