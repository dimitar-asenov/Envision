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

#include "JavaDebugger.h"

#include "../run_support/java/JavaRunner.h"
#include "jdwp/messages/EventSet.h"
#include "jdwp/Location.h"

#include "ModelBase/src/nodes/Node.h"

#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Module.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"
#include "VisualizationBase/src/overlays/MessageOverlay.h"

namespace OODebug {

JavaDebugger& JavaDebugger::instance()
{
	static JavaDebugger instance;
	return instance;
}

void JavaDebugger::debugTree(Model::TreeManager* manager, const QString& pathToProjectContainerDirectory)
{
	Model::Node* mainContainer = JavaRunner::runTree(manager, pathToProjectContainerDirectory, true);
	// Find the class name where the main method is in.
	auto mainClass = mainContainer->firstAncestorOfType<OOModel::Class>();
	Q_ASSERT(mainClass);

	QString mainClassName = fullNameFor(mainClass, '.');
	debugConnector_.connect(mainClassName);
}

bool JavaDebugger::addBreakpoint(Visualization::Item* target, QKeyEvent* event)
{
	if (event->modifiers() == Qt::NoModifier && (event->key() == Qt::Key_F8))
	{
		auto it = breakpoints_.find(target);
		if (it != breakpoints_.end())
		{
			target->scene()->removeOverlay(it->overlay_);
			if (debugConnector_.vmRunning() && it->requestId_ > 0)
				debugConnector_.clearBreakpoint(it->requestId_);
			breakpoints_.erase(it);
		}
		else
		{
			auto breakpoint = Breakpoint(addBreakpointOverlay(target));
			if (debugConnector_.vmRunning())
				breakpoint.requestId_ = debugConnector_.sendBreakpoint(nodeToLocation(target->node()));
			breakpoints_[target] = breakpoint;
		}
		return true;
	}
	return false;
}

bool JavaDebugger::resume(Visualization::Item*, QKeyEvent* event)
{
	if (event->modifiers() == Qt::NoModifier && (event->key() == Qt::Key_F6))
	{
		debugConnector_.resume();
		return true;
	}
	return false;
}

JavaDebugger::JavaDebugger()
{
	debugConnector_.addEventListener(Protocol::EventKind::CLASS_PREPARE, [this] (Event e) { handleClassPrepare(e);});
}

Visualization::MessageOverlay* JavaDebugger::addBreakpointOverlay(Visualization::Item* target)
{
	// TODO: Use a custom overlay for breakpoints.
	static const QString overlayGroupName("Breakpoint overlay");
	auto scene = target->scene();
	// TODO: QUESTION: overlayGroup could just create a group if there is none?
	auto overlayGroup = scene->overlayGroup(overlayGroupName);

	if (!overlayGroup) overlayGroup = scene->addOverlayGroup(overlayGroupName);
	auto overlay = new Visualization::MessageOverlay(target,
																	 [](Visualization::MessageOverlay *){
																		  return QString("BP");
																	  });
	overlayGroup->addOverlay(makeOverlay(overlay));
	return overlay;
}

QString JavaDebugger::jvmSignatureFor(OOModel::Class* theClass)
{
	// from JNI spec fully qualified class: http://docs.oracle.com/javase/1.5.0/docs/guide/jni/spec/types.html#wp16432
	QString signature = fullNameFor(theClass, '/');
	signature.prepend("L").append(";");
	return signature;
}

QString JavaDebugger::fullNameFor(OOModel::Class* theClass, QChar delimiter)
{
	QString fullName = theClass->name();
	auto module = theClass->firstAncestorOfType<OOModel::Module>();
	while (module)
	{
		fullName.prepend(module->name() + delimiter);
		module = module->firstAncestorOfType<OOModel::Module>();
	}
	return fullName;
}

Location JavaDebugger::nodeToLocation(Model::Node* node)
{
	auto method = node->firstAncestorOfType<OOModel::Method>();
	auto containerClass = method->firstAncestorOfType<OOModel::Class>();
	qint64 classId =  debugConnector_.getClassId(jvmSignatureFor(containerClass));
	// TODO: function to get signature of a method: for Java classes we would need the full java library.
	// Once fixed also fix the implementation of getMethodId().
	qint64 methodId = debugConnector_.getMethodId(classId, method->name());
	Q_ASSERT(methodId != -1);

	auto tagKind = Protocol::TypeTagKind::CLASS;
	if (containerClass->constructKind() == OOModel::Class::ConstructKind::Interface)
		tagKind = Protocol::TypeTagKind::INTERFACE;
	else if (containerClass->constructKind() != OOModel::Class::ConstructKind::Class)
		Q_ASSERT(0); // This should not happen for a Java project!

	// TODO: fix method index
	return Location(tagKind, classId, methodId, 0);
}

void JavaDebugger::handleClassPrepare(Event)
{
	for (auto it = breakpoints_.begin(); it != breakpoints_.end(); ++it)
	{
		auto target = it.key();
		auto targetNode = target->node();
		it.value().requestId_ = debugConnector_.sendBreakpoint(nodeToLocation(targetNode));
	}
	debugConnector_.resume();
}

} /* namespace OODebug */
