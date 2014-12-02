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

bool JavaDebugger::addBreakPoint(Visualization::Item* target, QKeyEvent* event)
{
	if (event->modifiers() == Qt::NoModifier && (event->key() == Qt::Key_F8))
	{
		auto it = breakpoints_.find(target);
		if (it != breakpoints_.end())
		{
			target->scene()->removeOverlay(it->overlay);
			// TODO send clear
			breakpoints_.erase(it);
		}
		else
		{
			breakpoints_[target] = BreakPoint(addBreakPointOverlay(target));
			// TODO send request
		}
		return true;
	}
	return false;
}

JavaDebugger::JavaDebugger()
{
	debugConnector_.addEventListener(Protocol::EventKind::CLASS_PREPARE, [this] (Event e) { handleClassPrepare(e);});
}

Visualization::MessageOverlay* JavaDebugger::addBreakPointOverlay(Visualization::Item* target)
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

QString JavaDebugger::jvmSignatureFor(OOModel::Class* clazz)
{
	// from JNI spec fully qualified class: http://docs.oracle.com/javase/1.5.0/docs/guide/jni/spec/types.html#wp16432
	QString signature = fullNameFor(clazz, '/');
	signature.prepend("L").append(";");
	return signature;
}

QString JavaDebugger::fullNameFor(OOModel::Class* clazz, QChar delim)
{
	QString fullName = clazz->name();
	auto mod = clazz->firstAncestorOfType<OOModel::Module>();
	while (mod)
	{
		fullName.prepend(mod->name() + delim);
		mod = mod->firstAncestorOfType<OOModel::Module>();
	}
	return fullName;
}

void JavaDebugger::handleClassPrepare(Event e)
{
	qDebug() << "Prepare" << e.classPrepare().signature();
	for (auto it = breakpoints_.begin(); it != breakpoints_.end(); ++it)
	{
		auto target = it.key();
		auto targetNode = target->node();
		auto method = targetNode->firstAncestorOfType<OOModel::Method>();
		auto clazz = method->firstAncestorOfType<OOModel::Class>();
		qDebug() << jvmSignatureFor(clazz);
	}
}

} /* namespace OODebug */
