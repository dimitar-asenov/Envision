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

#include "ReferenceFinder.h"
#include "../run_support/JavaRunner.h"
#include "jdwp/messages/AllMessages.h"
#include "jdwp/DataTypes.h"
#include "../OODebugPlugin.h"

#include "JavaExport/src/exporter/JavaExporter.h"

#include "ModelBase/src/nodes/Node.h"

#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/elements/StatementItemList.h"

#include "OOVisualization/src/elements/VStatementItemList.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"
#include "VisualizationBase/src/overlays/IconOverlay.h"
#include "VisualizationBase/src/overlays/SelectionOverlay.h"

#include "InteractionBase/src/commands/CommandResult.h"

#include "Logger/src/Log.h"

namespace OODebug {

struct VariableObserver {
		VariableObserver(JavaDebugger::ValueHandler handlerFunction,
							  QList<OOModel::VariableDeclaration*> observedVariables, Model::Node* observerLocation,
							  QList<Probes::ValueCalculator> valueCalculators)
			: handlerFunc_{handlerFunction}, observedVariables_{observedVariables},
			  observerLocation_{observerLocation}, valueCalculators_{valueCalculators} {}

		// The function which handles new value(s).
		JavaDebugger::ValueHandler handlerFunc_;
		// The declarations of the variables we are observing.
		QList<OOModel::VariableDeclaration*> observedVariables_;
		// The location of the observer, this might be useful if it has an attached overlay.
		Model::Node* observerLocation_;
		// Value calculator functions
		QList<Probes::ValueCalculator> valueCalculators_;
};

struct UserVisibleBreakpoint {
		JavaDebugger::BreakpointId breakpointId_;
		Visualization::IconOverlay* breakpointOverlay_;
};

struct ProbeVisualization {
		JavaDebugger::BreakpointId breakpointId_;
		Visualization::IconOverlay* monitorOverlay_;
		PlotOverlay* plotOverlay_;
};

struct TrackedVariable {
		QList<JavaDebugger::BreakpointId> breakpointIds_;
		PlotOverlay* plotOverlay_;
};

const QString JavaDebugger::BREAKPOINT_OVERLAY_GROUP{"Breakpoint overlay"};
const QString JavaDebugger::PLOT_OVERLAY_GROUP{"PlotOverlay"};
const QString JavaDebugger::CURRENT_LINE_OVERLAY_GROUP{"CurrentLine"};
const QString JavaDebugger::MONITOR_OVERLAY_GROUP{"MonitorOverlay"};

JavaDebugger& JavaDebugger::instance()
{
	static JavaDebugger instance;
	return instance;
}

Interaction::CommandResult* JavaDebugger::debugTree(Model::TreeManager* manager,
																	 const QString& pathToProjectContainerDirectory)
{
	auto project = DCast<OOModel::Project>(manager->root());
	Q_ASSERT(project);

	auto runResult = JavaRunner::runTree(manager, pathToProjectContainerDirectory, true);
	if (runResult->code() != Interaction::CommandResult::OK) return runResult;

	// All previously set breakpoints have to be unset again.
	auto breakpointsList = setBreakpoints_.values();
	QSet<Model::Node*> breakpoints(breakpointsList.begin(), breakpointsList.end());
	unsetBreakpoints_.unite(breakpoints);
	setBreakpoints_.clear();
	breakOnLoadClasses_.clear();

	// Reset observers
	for (auto observerIt = nodeObservedBy_.cbegin(); observerIt != nodeObservedBy_.cend(); ++observerIt)
	{
		if (auto observedNode = observerIt.value()->observerLocation_)
		{
			auto visualizationIt = Visualization::Item::nodeItemsMap().find(observedNode);
			Q_ASSERT(visualizationIt != Visualization::Item::nodeItemsMap().end());
			if (auto overlay = (*visualizationIt)->overlay<PlotOverlay>(PLOT_OVERLAY_GROUP)) overlay->clear();
		}
	}
	removeHighlightFromCurrentLine();

	utils_.setExportMap(JavaExport::JavaExporter::exportMaps().map(project));
	debugConnector_.connect();
	return new Interaction::CommandResult{};
}

bool JavaDebugger::toggleBreakpoint(Visualization::Item* target, QKeyEvent* event)
{
	if (event->modifiers() == Qt::NoModifier && (event->key() == Qt::Key_F8))
	{
		// Note that we currently store user breakpoints in a QHash, theoretically we could also use a multi hash
		// so that the user can add multiple breakpoints on a node. (Currently if a script sets a user breakpoint the
		// user will also remove this one when pressing F8).
		auto node = target->node();
		auto breakpointIt = userBreakpoints_.find(node);
		if (breakpointIt != userBreakpoints_.end())
		{
			target->scene()->removeOverlay(breakpointIt.value().breakpointOverlay_);
			removeBreakpoint(breakpointIt.value().breakpointId_);
		}
		else
		{
			BreakpointId breakpointId = addBreakpoint(node, BreakpointType::User);
			addBreakpointListener(breakpointId, [this](Model::Node* node, const BreakpointEvent&) {
				auto visualization = *Visualization::Item::nodeItemsMap().find(node);
				toggleLineHighlight(visualization, true);
				return BreakpointAction::Stop;
			});

		}
		return true;
	}
	return false;
}

bool JavaDebugger::suspend(Visualization::Item*, QKeyEvent* event)
{
	if (event->modifiers() == Qt::NoModifier && (event->key() == Qt::Key_Pause))
	{
		debugConnector_.suspend();
		return true;
	}
	return false;
}

bool JavaDebugger::resume(Visualization::Item*, QKeyEvent* event)
{
	if (event->modifiers() == Qt::NoModifier && (event->key() == Qt::Key_F6))
	{
		resume();
		return true;
	}
	return false;
}

bool JavaDebugger::trackVariable(Visualization::Item* target, QKeyEvent* event)
{
	if (event->modifiers() == Qt::NoModifier && (event->key() == Qt::Key_F12))
	{
		auto node = target->node();
		Q_ASSERT(node);
		auto variableDeclaration = utils_.variableDeclarationFromStatement(DCast<OOModel::StatementItem>(node));
		if (!variableDeclaration) return false;

		// If existing remove
		auto trackedIt = trackedVariables_.find(variableDeclaration);
		if (trackedIt != trackedVariables_.end())
		{
			for (BreakpointId breakpointId : trackedIt->breakpointIds_)
				removeBreakpoint(breakpointId);
			target->scene()->removeOverlay(trackedIt->plotOverlay_);
			return true;
		}

		// Otherwise find all refs and create breakpoints for each.
		ReferenceFinder refFinder;
		refFinder.setSearchNode(node);
		auto containingMethod = node->firstAncestorOfType<OOModel::Method>();
		refFinder.visit(containingMethod);

		auto defaultTypeAndHandler = defaultPlotTypeAndValueHandlerFor({variableDeclaration});
		auto plotOverlay = new PlotOverlay{target, PlotOverlay::itemStyles().get("default"), defaultTypeAndHandler.first};
		target->addOverlay(plotOverlay, PLOT_OVERLAY_GROUP);
		auto observer = std::make_shared<VariableObserver>
				(VariableObserver{defaultTypeAndHandler.second, {variableDeclaration}, node,
				{[](QList<double> arg) { return arg[0];}}});
		QList<BreakpointId> breakpointIds;
		for (auto ref : refFinder.references())
		{
			// Breakpoints are always on StatementItems so get the first StatementItem parent node.
			auto statementNode = ref->firstAncestorOfType<OOModel::StatementItem>();
			breakpointIds << addBreakpoint(statementNode, BreakpointType::Internal);
		}
		trackedVariables_.insert(variableDeclaration, {breakpointIds, plotOverlay});

		for (BreakpointId breakpointId : breakpointIds)
			addBreakpointListener(breakpointId, createListenerFor(observer, plotOverlay));
		return true;
	}
	return false;
}

bool JavaDebugger::step(Visualization::Item*, QKeyEvent* event)
{
	if (event->modifiers() == Qt::ControlModifier && (event->key() == Qt::Key_F6))
	{
		debugConnector_.singleStep(currentThreadId_);
		resume();
		return true;
	}
	return false;
}

Interaction::CommandResult* JavaDebugger::probe(OOVisualization::VStatementItemList* itemList,
																const QStringList& arguments, int itemIndex)
{
	Q_ASSERT(!arguments.empty());
	if (itemIndex == itemList->rangeEnd()) --itemIndex; // TODO handle this properly
	auto vItem = itemList->itemAt<Visualization::Item>(itemIndex);
	Q_ASSERT(vItem);
	auto observedNode = vItem->node();

	if (arguments[0] == "-")
	{
		BreakpointId idToRemove = -1;
		if (arguments.size() > 1)
			idToRemove = arguments[1].toInt();
		for (auto it = probes_.find(observedNode); it != probes_.end() && it.key() == observedNode;)
		{
			if (idToRemove < 1 || it->breakpointId_ == idToRemove)
			{
				removeBreakpoint(it->breakpointId_);
				vItem->scene()->removeOverlay(it->monitorOverlay_);
				vItem->scene()->removeOverlay(it->plotOverlay_);
				it = probes_.erase(it);
			}
			else
				++it;
		}
		return new Interaction::CommandResult{};
	}

	auto parsedArgs = Probes::parseProbeArguments(arguments);
	QStringList variableNames = parsedArgs.second;

	auto declarationMap = utils_.findVariableDetailsIn(DCast<OOModel::StatementItemList>(itemList->node()),
																		variableNames, itemIndex);

	if (declarationMap.size() < variableNames.size())
		return new Interaction::CommandResult{new Interaction::CommandError{"Not all declarations found for probe"}};

	QList<OOModel::VariableDeclaration*> vars;
	for (auto varName : variableNames) vars << declarationMap[varName];

	auto defaultTypeAndHandler = defaultPlotTypeAndValueHandlerFor(vars);
	auto observer = std::make_shared<VariableObserver>
			(VariableObserver{defaultTypeAndHandler.second, vars, observedNode, parsedArgs.first});
	BreakpointId breakpointId = addBreakpoint(observedNode, BreakpointType::Internal);

	auto monitorOverlay = new Visualization::IconOverlay{vItem, Visualization::IconOverlay::itemStyles().get("monitor")};
	vItem->addOverlay(monitorOverlay, MONITOR_OVERLAY_GROUP);

	auto plotOverlay = new PlotOverlay{vItem, PlotOverlay::itemStyles().get("default"),
												  defaultTypeAndHandler.first, variableNames};
	vItem->addOverlay(plotOverlay, PLOT_OVERLAY_GROUP);

	probes_.insert(observedNode, {breakpointId, monitorOverlay, plotOverlay});

	addBreakpointListener(breakpointId, createListenerFor(observer, plotOverlay));

	// TODO optimally we would represent this somehow in the user interface (maybe in the plot overlay).
	OODebugPlugin::log().info(QString{"Added probe %1"}.arg(breakpointId));

	return new Interaction::CommandResult{};
}

JavaDebugger::BreakpointId JavaDebugger::addBreakpoint(Model::Node* location, BreakpointType type)
{
	auto breakpointId = ++nextBreakpointId_;
	breakpointIds_.insert(location, breakpointId);

	if (BreakpointType::User == type)
	{
		auto visualizationIt = Visualization::Item::nodeItemsMap().find(location);
		Q_ASSERT(visualizationIt != Visualization::Item::nodeItemsMap().end());
		auto visualization = *visualizationIt;
		auto breakpointOverlay = visualization->overlay<Visualization::IconOverlay>(BREAKPOINT_OVERLAY_GROUP);
		if (!breakpointOverlay)
		{
			breakpointOverlay = new Visualization::IconOverlay{visualization,
					Visualization::IconOverlay::itemStyles().get("breakpoint")};
			visualization->addOverlay(breakpointOverlay, BREAKPOINT_OVERLAY_GROUP);
		}
		userBreakpoints_.insert(location, {breakpointId, breakpointOverlay});
	}
	addBreakpointAt(location);
	return breakpointId;
}

void JavaDebugger::removeBreakpoint(BreakpointId breakpointId)
{
	Model::Node* affectedNode = nullptr;
	for (auto it = breakpointIds_.begin(); it != breakpointIds_.end(); ++it)
	{
		if (it.value() == breakpointId)
		{
			affectedNode = it.key();
			it = breakpointIds_.erase(it);
			// A breakpointId should be unique so we can break as soon as we find it.
			break;
		}
	}
	if (affectedNode != nullptr)
	{
		removeBreakpointAt(affectedNode);
		userBreakpoints_.remove(affectedNode);
	}

	breakpointListeners_.remove(breakpointId);
}

JavaDebugger::JavaDebugger()
{
	debugConnector_.addEventListener(Protocol::EventKind::CLASS_PREPARE, [this] (Event e) { handleClassPrepare(e);});
	debugConnector_.addEventListener(Protocol::EventKind::BREAKPOINT,
												[this] (Event e) { handleBreakpoint(e.breakpoint()); });
	debugConnector_.addEventListener(Protocol::EventKind::VM_START, [this] (Event e) { handleVMStart(e); });
	debugConnector_.addEventListener(Protocol::EventKind::SINGLE_STEP,
												[this] (Event e) { handleSingleStep(e.singleStep()); });
	debugConnector_.addEventListener(Protocol::EventKind::VM_DEATH, [this] (Event) {
		for (auto listener : exitListeners_) listener();
		exitListeners_.clear();
	});
}

bool JavaDebugger::isParentClassLoaded(Model::Node* node)
{
	auto containerClass = node->firstAncestorOfType<OOModel::Class>();
	qint64 id = debugConnector_.classIdOf(utils_.jvmSignatureFor(containerClass));
	return id != DebugConnector::NO_RESULT;
}

void JavaDebugger::breaktAtParentClassLoad(Model::Node* node)
{
	auto containerClass = node->firstAncestorOfType<OOModel::Class>();
	if (!breakOnLoadClasses_.contains(containerClass))
	{
		debugConnector_.breakAtClassLoad(utils_.fullNameFor(containerClass, '.'));
		breakOnLoadClasses_.insert(containerClass);
	}
}

void JavaDebugger::trySetBreakpoints()
{
	// TODO: Multiple breakpoints show weird behavior. If we auto resume (from tracked variable)
	// and the event is received twice the VM assumes that we resume both events (as they are the same location).
	//
	// This is a workaround that we don't have multiple breakpoints at one location.
	//
	// We should implement it more general, if the user places a breakpoint and that is already
	// sent and then decides to track a variable which has a reference at this location we still have a double
	// breakpoint!
	QHash<Location, Model::Node*> breakpointLocations;
	for (auto it = unsetBreakpoints_.begin(); it != unsetBreakpoints_.end(); )
	{
		if (isParentClassLoaded(*it))
		{
			breakpointLocations[utils_.nodeToLocation(*it)] = *it;
			it = unsetBreakpoints_.erase(it);
		}
		else
		{
			breaktAtParentClassLoad(*it);
			++it;
		}
	}
	for (auto it = breakpointLocations.begin(); it != breakpointLocations.end(); ++it)
	{
		qint32 requestId = debugConnector_.setBreakpoint(it.key());
		setBreakpoints_[requestId] = it.value();
	}
}

void JavaDebugger::removeBreakpointAt(Model::Node* node)
{
	unsetBreakpoints_.remove(node);

	for (auto it = setBreakpoints_.begin(); it != setBreakpoints_.end(); ++it)
	{
		if (it.value() == node)
		{
			if (debugConnector_.vmAlive()) debugConnector_.clearBreakpoint(it.key());
			setBreakpoints_.erase(it);
			break; // There should only be one breakpoint
		}
	}
}

void JavaDebugger::addBreakpointAt(Model::Node* node)
{
	if (debugConnector_.vmAlive())
	{
		if (isParentClassLoaded(node))
		{
			qint32 requestId = debugConnector_.setBreakpoint(utils_.nodeToLocation(node));
			setBreakpoints_[requestId] = node;
		}
		else
		{
			unsetBreakpoints_.insert(node);
			breaktAtParentClassLoad(node);
		}
	}
	else
	{
		unsetBreakpoints_.insert(node);
	}
}

void JavaDebugger::resume()
{
	debugConnector_.resume();
	removeHighlightFromCurrentLine();
	currentThreadId_ = 0;
}

void JavaDebugger::removeHighlightFromCurrentLine()
{
	if (currentLineItem_)
	{
		toggleLineHighlight(currentLineItem_, false);
		currentLineItem_ = nullptr;
	}
}

void JavaDebugger::handleVMStart(Event)
{
	trySetBreakpoints();
	debugConnector_.wantResume(true);
}

void JavaDebugger::handleClassPrepare(Event)
{
	trySetBreakpoints();
	debugConnector_.wantResume(true);
}

void JavaDebugger::handleBreakpoint(BreakpointEvent breakpointEvent)
{
	// Get list of breakpoint ids iterate over each id and call the handler, the handler should tell if we should continue.
	auto it = setBreakpoints_.find(breakpointEvent.requestID());
	// If we get a event for a breakpoint we don't know we have an implementation error.
	Q_ASSERT(it != setBreakpoints_.end() && *it);

	// TODO replace with find iterator loop
	BreakpointAction action = BreakpointAction::Resume;
	for (BreakpointId breakpointId : breakpointIds_.values(*it))
	{
		auto listenerIt = breakpointListeners_.find(breakpointId);
		if (listenerIt != breakpointListeners_.end())
		{
			auto currentAction = (*listenerIt)(*it, breakpointEvent);
			if (BreakpointAction::Stop == currentAction && BreakpointAction::Resume == action)
				action = currentAction;
		}
	}

	if (BreakpointAction::Resume == action)
		debugConnector_.wantResume(true);
	else
		currentThreadId_ = breakpointEvent.thread();

	auto visualization = *Visualization::Item::nodeItemsMap().find(*it);
	currentLineItem_ = visualization;
}

void JavaDebugger::handleSingleStep(SingleStepEvent singleStep)
{
	bool closingBracket = false;
	auto node = utils_.locationToNode(singleStep.location(), closingBracket);

	debugConnector_.wantResume(false);

	currentThreadId_ = singleStep.thread();
	auto visualization = *Visualization::Item::nodeItemsMap().find(node);
	currentLineItem_ = visualization;
	toggleLineHighlight(currentLineItem_, true, closingBracket);
}

QPair<PlotOverlay::PlotType, JavaDebugger::ValueHandler> JavaDebugger::defaultPlotTypeAndValueHandlerFor(
		QList<OOModel::VariableDeclaration*> variables)
{
	Q_ASSERT(!variables.empty());

	if (utils_.hasPrimitiveValueType(variables[0]))
	{
		bool allPrimitive = true;
		for (auto variable : variables)
			if (!utils_.hasPrimitiveValueType(variable)) allPrimitive = false;
		if (allPrimitive)
		{
			if (variables.size() > 1)
				return {PlotOverlay::PlotType::Scatter, &JavaDebugger::handleValues};
			else
				return {PlotOverlay::PlotType::Bars, &JavaDebugger::handleValues};
		}
	}
	else if (utils_.typeExpressionToTag(variables[0]->typeExpression()) == Protocol::Tag::ARRAY)
	{
		return {PlotOverlay::PlotType::Array, &JavaDebugger::handleArray};
	}

	Q_ASSERT(false); // We should implement something for this combination
}

void JavaDebugger::handleValues(Values values, QList<Probes::ValueCalculator> valueCalculators,
										  PlotOverlay* plotOverlay)
{
	QList<double> doubleValues;
	for (auto val : values.values()) doubleValues << utils_.doubleFromValue(val);
	QList<double> plotValues;
	for (auto extractor : valueCalculators) plotValues << extractor(doubleValues);
	if (plotValues.size() > 1)
		plotOverlay->addValues(plotValues[0], plotValues.mid(1));
	else if (plotValues.size() == 1)
		plotOverlay->addValue(plotValues[0]);
}

void JavaDebugger::handleArray(Values values, QList<Probes::ValueCalculator>, PlotOverlay* plotOverlay)
{
	auto vals = values.values();
	QList<int> indices;
	for (int i = 1; i < vals.size(); ++i) indices << utils_.doubleFromValue(vals[i]);
	int arrayLen = debugConnector_.arrayLength(vals[0].array());
	auto arrayVals = debugConnector_.arrayValues(vals[0].array(), 0, arrayLen);
	switch (arrayVals.type())
	{
		case Protocol::Tag::FLOAT: return plotOverlay->updateArrayValues(arrayVals.floats(), indices);
		case Protocol::Tag::DOUBLE: return plotOverlay->updateArrayValues(arrayVals.doubles(), indices);
		case Protocol::Tag::INT: return plotOverlay->updateArrayValues(arrayVals.ints(), indices);
		case Protocol::Tag::LONG: return plotOverlay->updateArrayValues(arrayVals.longs(), indices);
		case Protocol::Tag::SHORT: return plotOverlay->updateArrayValues(arrayVals.shorts(), indices);
		default: Q_ASSERT(false); // you shouldn't try to convert any non numeric values to double.
	}
}

void JavaDebugger::toggleLineHighlight(Visualization::Item* item, bool highlight, bool closingBracket)
{
	auto existingLineOverlay = item->overlay<Visualization::SelectionOverlay>(CURRENT_LINE_OVERLAY_GROUP);
	auto existingBracketOverlay = item->overlay<Visualization::IconOverlay>(CURRENT_LINE_OVERLAY_GROUP);
	if (highlight)
	{
		if (closingBracket)
		{
			if (existingBracketOverlay) return;
			auto overlay = new Visualization::IconOverlay{
						item, Visualization::IconOverlay::itemStyles().get("endOfMethod")};
			item->addOverlay(overlay, CURRENT_LINE_OVERLAY_GROUP);
		}
		else
		{
			if (existingLineOverlay) return;
			auto overlay = new Visualization::SelectionOverlay{
						item, Visualization::SelectionOverlay::itemStyles().get("currentStatement")};
			item->addOverlay(overlay, CURRENT_LINE_OVERLAY_GROUP);
		}

	}
	else
	{
		if (existingLineOverlay) item->scene()->removeOverlay(existingLineOverlay);
		if (existingBracketOverlay) item->scene()->removeOverlay(existingBracketOverlay);
	}
}

JavaDebugger::BreakpointListener JavaDebugger::createListenerFor(std::shared_ptr<VariableObserver> observer,
																					  PlotOverlay* plotOverlay)
{
	return [this, observer, plotOverlay](Model::Node*, const BreakpointEvent& breakpointEvent)
	{
		// Get frames
		auto frames = debugConnector_.frames(breakpointEvent.thread(), 1);
		auto location = breakpointEvent.location();
		auto variableTable = debugConnector_.variableTableForMethod(location.classId(), location.methodId());
		if (frames.frames().size() == 0)
		{
			OODebugPlugin::log().error(QString{"No frames received, error: %1"}.arg(static_cast<qint8>(frames.error())));
			return BreakpointAction::Resume;
		}
		auto currentFrame = frames.frames()[0];
		int currentIndex = currentFrame.location().methodIndex();


		QList<StackVariable> varsToGet;
		for (auto variable : observer->observedVariables_)
		{
			for (auto variableDetails : variableTable.variables())
			{
				if (variableDetails.name() == variable->name())
				{
					// Condition as in: http://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html
					//                    #JDWP_Method_VariableTable
					Q_ASSERT(variableDetails.codeIndex() <= currentIndex &&
								currentIndex < variableDetails.codeIndex() + variableDetails.length());
					varsToGet << StackVariable{variableDetails.slot(), utils_.typeExpressionToTag(
										 variable->typeExpression())};
				}
			}
		}
		auto values = debugConnector_.values(breakpointEvent.thread(), currentFrame.frameID(), varsToGet);
		Q_ASSERT(observer->handlerFunc_);
		observer->handlerFunc_(this, values, observer->valueCalculators_, plotOverlay);
		return BreakpointAction::Resume;
	};
}

}
