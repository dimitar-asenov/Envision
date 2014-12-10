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

#pragma once

#include "../oodebug_api.h"
#include "jdwp/DebugConnector.h"
#include "metadata/Breakpoint.h"
#include "metadata/JavaMethod.h"

namespace Export {
	class TextToNodeMap;
}

namespace Model {
	class Node;
	class TreeManager;
}

namespace OOModel {
	class Class;
	class Expression;
	class Method;
}

namespace Visualization {
	class Item;
	class MessageOverlay;
}

namespace OODebug {

class Location;
class BreakpointEvent;
class VariableDetails;

class OODEBUG_API JavaDebugger
{
	public:
		static JavaDebugger& instance();
		void debugTree(Model::TreeManager* manager, const QString& pathToProjectContainerDirectory);
		bool addBreakpoint(Visualization::Item* target, QKeyEvent* event);
		bool resume(Visualization::Item* target, QKeyEvent* event);

	private:
		JavaDebugger();
		Visualization::MessageOverlay* addBreakpointOverlay(Visualization::Item* target);
		QString jvmSignatureFor(OOModel::Class* theClass);
		/**
		 * Returns a String with all containing module names split by \a delimiter in front of the \a theClass name.
		 */
		QString fullNameFor(OOModel::Class* theClass, QChar delimiter);

		/**
		 * Tries to translate the \a node into a \a Location and stores the result in \a resolvedLocation.
		 * If it succeeds true is returned and the \a resolvedLocation is set, otherwise false is returned.
		 */
		bool nodeToLocation(Model::Node* node, Location& resolvedLocation);

		void handleClassPrepare(Event e);
		void handleBreakpoint(BreakpointEvent breakpointEvent);

		Protocol::Tag typeOfVariable(OOModel::Method* containingMethod, VariableDetails variable);
		Protocol::Tag typeExpressionToTag(OOModel::Expression* e);

		DebugConnector debugConnector_;

		QHash<Visualization::Item*, Breakpoint> breakpoints_;
		Visualization::Item* currentBreakpointKey_{};

		QHash<QPair<qint64, qint64>, JavaMethod> methodInfos_;

		std::shared_ptr<Export::TextToNodeMap> exportMap_;
};

} /* namespace OODebug */
