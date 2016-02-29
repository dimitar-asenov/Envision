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
#include "../overlays/PlotOverlay.h"
#include "Probes.h"
#include "DebugUtils.h"

namespace Model {
	class Node;
	class TreeManager;
}

namespace Visualization {
	class Item;
}

namespace OOVisualization {
	class VStatementItemList;
}

namespace Interaction {
	class CommandResult;
}

namespace OODebug {

struct BreakpointEvent;
struct SingleStepEvent;
struct VariableObserver;

class OODEBUG_API JavaDebugger
{
	public:
		static JavaDebugger& instance();
		/**
		 * Starts a debug session for the current project. Returns if everything was okay in a CommandResult.
		 */
		Interaction::CommandResult* debugTree(Model::TreeManager* manager,
														  const QString& pathToProjectContainerDirectory);
		bool toggleBreakpoint(Visualization::Item* target, QKeyEvent* event);
		bool suspend(Visualization::Item* target, QKeyEvent* event);
		bool resume(Visualization::Item* target, QKeyEvent* event);
		bool trackVariable(Visualization::Item* target, QKeyEvent* event);
		bool step(Visualization::Item* target, QKeyEvent* event);

		Interaction::CommandResult* probe(OOVisualization::VStatementItemList* itemList,
													 const QStringList& arguments, int itemIndex);

		using ValueHandler = std::function<void(JavaDebugger*, Values, QList<Probes::ValueCalculator>, Model::Node*)>;

		/**
		 * Type of a Breakpoint:
		 * \a Internal means the user will not see the breakpoint.
		 * 		This is useful for breakpoints where one wants to auto resume.
		 * 			(e.g. count how many times a statement is executed)
		 * \a User means there will be an icon notifying the user about the existince of the user.
		 */
		enum class BreakpointType : int { Internal, User };
		// TODO in the future it might make sense to assign IDs to breakpoints, so that we can have multiple breakpoints
		// at a single location and still be able to remove only a single one.
		void addBreakpoint(Model::Node* location, BreakpointType type);

		using BreakpointListener = std::function<bool (Model::Node*, const BreakpointEvent&)>;
		int addBreakpointListener(Model::Node* node, BreakpointListener listener);
		void removeBreakpointListener(int id);

		using ProgramExitSingleShot = std::function<void ()>;
		/**
		 * Register a listener for the next program exit. Once the event happened the listener will be destroyed
		 * and not called in any subsequent event.
		 */
		void addProgramExitLister(ProgramExitSingleShot listener);

	private:
		JavaDebugger();

		bool isParentClassLoaded(Model::Node* node);
		void breaktAtParentClassLoad(Model::Node* node);

		void trySetBreakpoints();
		void removeBreakpointAt(Model::Node* node);
		void addBreakpointAt(Model::Node* node);

		void resume();

		void removeHighlightFromCurrentLine();

		// Event handlers
		void handleVMStart(Event);
		void handleClassPrepare(Event);
		void handleBreakpoint(BreakpointEvent breakpointEvent);
		void handleSingleStep(SingleStepEvent singleStep);

		// Probe helpers
		QPair<PlotOverlay::PlotType, ValueHandler> defaultPlotTypeAndValueHandlerFor
			(QList<OOModel::VariableDeclaration*> variables);
		void handleValues(Values values, QList<Probes::ValueCalculator> valueCalculators, Model::Node* target);
		void handleArray(Values values, QList<Probes::ValueCalculator> valueCalculators, Model::Node* target);

		// Overlay functions
		void addBreakpointOverlay(Visualization::Item* target);
		void toggleLineHighlight(Visualization::Item* item, bool highlight, bool closingBracket = false);
		PlotOverlay* plotOverlayOfNode(Model::Node* node);
		void removeObserverOverlaysAt(Model::Node* node, Visualization::Item* nodeVisualization);

		DebugConnector debugConnector_;
		DebugUtils utils_{&debugConnector_};

		// For each class we should only break at loading once, otherwise we get multiple events.
		QSet<Model::Node*> breakOnLoadClasses_;

		QList<Model::Node*> unsetBreakpoints_;
		QHash<qint32, Model::Node*> setBreakpoints_;
		Visualization::Item* currentLineItem_{};
		qint64 currentThreadId_{};

		QMultiHash<Model::Node*, std::shared_ptr<VariableObserver>> nodeObservedBy_;
		QMultiHash<Model::Node*, std::pair<int, BreakpointListener>> breakpointListeners_;
		int nextBreakpointListenerId_{0};

		QVector<ProgramExitSingleShot> exitListeners_;

		static const QString BREAKPOINT_OVERLAY_GROUP;
		static const QString PLOT_OVERLAY_GROUP;
		static const QString CURRENT_LINE_OVERLAY_GROUP;
		static const QString MONITOR_OVERLAY_GROUP;
};

inline int JavaDebugger::addBreakpointListener(Model::Node* node, BreakpointListener listener) {
	int id = nextBreakpointListenerId_++;
	breakpointListeners_.insertMulti(node, {id, listener});
	return id;
}

inline void JavaDebugger::addProgramExitLister(JavaDebugger::ProgramExitSingleShot listener)
{
	exitListeners_.push_back(listener);
}

}
