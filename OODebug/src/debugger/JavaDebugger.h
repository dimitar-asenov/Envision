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
struct UserVisibleBreakpoint;
struct ProbeVisualization;
struct TrackedVariable;

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

		using ValueHandler = std::function<void(JavaDebugger*, Values, QList<Probes::ValueCalculator>, PlotOverlay*)>;

		/**
		 * Type of a Breakpoint:
		 * \a Internal means the user will not see the breakpoint.
		 * 		This is useful for breakpoints where one wants to auto resume.
		 * 			(e.g. count how many times a statement is executed)
		 * \a User means there will be an icon notifying the user about the existince of the user.
		 */
		enum class BreakpointType : int { Internal, User };

		/**
		 * The type of a breakpoint id.
		 */
		using BreakpointId = int;

		/**
		 * Adds a new breakpoint at the given \a location and with the given \a type.
		 * Returns the id of this breakpoint, the id is always greater than 0.
		 */
		BreakpointId addBreakpoint(Model::Node* location, BreakpointType type);
		/**
		 * Removes the breakpoint with the id \a breakpointId and any BreakpointListeners associated with it.
		 */
		void removeBreakpoint(BreakpointId breakpointId);

		/**
		 * Indicates wether we should resume after the breakpoint or not.
		 */
		enum class BreakpointAction : int {Resume, Stop};
		using BreakpointListener = std::function<BreakpointAction (Model::Node*, const BreakpointEvent&)>;
		void addBreakpointListener(qint64 breakpointId, BreakpointListener listener);

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
		void handleValues(Values values, QList<Probes::ValueCalculator> valueCalculators, PlotOverlay* plotOverlay);
		void handleArray(Values values, QList<Probes::ValueCalculator> valueCalculators, PlotOverlay* plotOverlay);

		// Overlay functions
		void toggleLineHighlight(Visualization::Item* item, bool highlight, bool closingBracket = false);

		BreakpointListener createListenerFor(std::shared_ptr<VariableObserver> observer, PlotOverlay* plotOverlay);

		DebugConnector debugConnector_;
		DebugUtils utils_{&debugConnector_};

		// For each class we should only break at loading once, otherwise we get multiple events.
		QSet<Model::Node*> breakOnLoadClasses_;

		// unset and set Breakpoint indicates the state of the debugger:
		// a set breakpoint is set in the vm-debugger and thus we have an id.
		// an unset breakpoint is not known in the vm-debugger.
		// Since a breakpoint should only be set once per location in the vm-debugger,
		// we allow multiple virtual breakpoints through the breakpiotsIds_ map.
		QSet<Model::Node*> unsetBreakpoints_;
		QHash<qint32, Model::Node*> setBreakpoints_;
		// We allow multiple 'virtual' breakpoints on a single node:
		QMultiHash<Model::Node*, BreakpointId> breakpointIds_;
		Visualization::Item* currentLineItem_{};
		qint64 currentThreadId_{};
		BreakpointId nextBreakpointId_{0};

		QMultiHash<Model::Node*, std::shared_ptr<VariableObserver>> nodeObservedBy_;
		QHash<qint64, BreakpointListener> breakpointListeners_;
		QHash<Model::Node*, UserVisibleBreakpoint> userBreakpoints_;
		QMultiHash<Model::Node*, ProbeVisualization> probes_;
		QHash<Model::Node*, TrackedVariable> trackedVariables_;

		QVector<ProgramExitSingleShot> exitListeners_;

		static const QString BREAKPOINT_OVERLAY_GROUP;
		static const QString PLOT_OVERLAY_GROUP;
		static const QString CURRENT_LINE_OVERLAY_GROUP;
		static const QString MONITOR_OVERLAY_GROUP;
};

inline void JavaDebugger::addBreakpointListener(qint64 breakpointId, BreakpointListener listener) {
	breakpointListeners_.insert(breakpointId, listener);
}

inline void JavaDebugger::addProgramExitLister(JavaDebugger::ProgramExitSingleShot listener)
{
	exitListeners_.push_back(listener);
}

}
