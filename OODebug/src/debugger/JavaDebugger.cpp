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
	unsetBreakpoints_ << setBreakpoints_.values();
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
		auto node = target->node();
		if (auto overlay = target->overlay<Visualization::IconOverlay>(BREAKPOINT_OVERLAY_GROUP))
		{
			target->scene()->removeOverlay(overlay);
			removeBreakpointAt(node);
		}
		else
		{
			addBreakpointOverlay(target);
			addBreakpointAt(node);
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

		// Check if this is tracked and if so remove it.
		// TODO if there is a probe on the same line this is wrong.
		auto it = nodeObservedBy_.find(node);
		if (it != nodeObservedBy_.end())
		{
			auto ptr = it.value();
			removeObserverOverlaysAt(node, target);
			// remove all observers at the references:
			it = nodeObservedBy_.begin();
			while (it != nodeObservedBy_.end() && it.key() == node )
			{
				if (it.value() == ptr)
				{
					removeBreakpointAt(it.key());
					it = nodeObservedBy_.erase(it);
				}
				else
				{
					++it;
				}
			}
			return true;
		}

		ReferenceFinder refFinder;
		refFinder.setSearchNode(node);
		auto containingMethod = node->firstAncestorOfType<OOModel::Method>();
		refFinder.visit(containingMethod);

		auto defaultTypeAndHandler = defaultPlotTypeAndValueHandlerFor({variableDeclaration});
		auto overlay = new PlotOverlay{target, PlotOverlay::itemStyles().get("default"), defaultTypeAndHandler.first};
		target->addOverlay(overlay, PLOT_OVERLAY_GROUP);
		auto observer = std::make_shared<VariableObserver>
				(VariableObserver{defaultTypeAndHandler.second, {variableDeclaration}, node,
				{[](QList<double> arg) { return arg[0];}}});
		nodeObservedBy_.insertMulti(node, observer);
		for (auto ref : refFinder.references())
		{
			// Breakpoints are always on StatementItems so get the first StatementItem parent node.
			auto statementNode = ref->firstAncestorOfType<OOModel::StatementItem>();
			nodeObservedBy_.insertMulti(statementNode, observer);
			addBreakpointAt(statementNode);
		}
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
		removeObserverOverlaysAt(observedNode, vItem);
		removeBreakpointAt(observedNode);
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
	nodeObservedBy_.insertMulti(observedNode, observer);
	addBreakpointAt(observedNode);

	auto overlay = new Visualization::IconOverlay{vItem, Visualization::IconOverlay::itemStyles().get("monitor")};
	vItem->addOverlay(overlay, MONITOR_OVERLAY_GROUP);

	auto plotOverlay = new PlotOverlay{vItem, PlotOverlay::itemStyles().get("default"),
												  defaultTypeAndHandler.first, variableNames};
	vItem->addOverlay(plotOverlay, PLOT_OVERLAY_GROUP);
	return new Interaction::CommandResult{};
}

void JavaDebugger::addBreakpoint(Model::Node* location, BreakpointType type)
{
	if (BreakpointType::Internal == type)
		addBreakpointAt(location);
	else
	{
		auto visualizationIt = Visualization::Item::nodeItemsMap().find(location);
		Q_ASSERT(visualizationIt != Visualization::Item::nodeItemsMap().end());
		auto visualization = *visualizationIt;
		if (!visualization->overlay<Visualization::IconOverlay>(BREAKPOINT_OVERLAY_GROUP))
		{
			addBreakpointOverlay(visualization);
			addBreakpointAt(location);
		}
	}
}

void JavaDebugger::removeBreakpointListener(int id)
{
	for (auto it = breakpointListeners_.begin(); it != breakpointListeners_.end();)
	{
		if (it.value().first == id) it = breakpointListeners_.erase(it);
		else ++it;
	}
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
	auto it = unsetBreakpoints_.begin();
	while (it != unsetBreakpoints_.end())
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
	int index = unsetBreakpoints_.indexOf(node);
	if (index > 0) unsetBreakpoints_.removeAt(index);

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
			unsetBreakpoints_ << node;
			breaktAtParentClassLoad(node);
		}
	}
	else
	{
		unsetBreakpoints_ << node;
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
	auto it = setBreakpoints_.find(breakpointEvent.requestID());
	// If we get a event for a breakpoint we don't know we have an implementation error.
	Q_ASSERT(it != setBreakpoints_.end() && *it);

	auto observersIt = nodeObservedBy_.find(*it);
	if (observersIt != nodeObservedBy_.end())
	{
		// Get frames
		auto frames = debugConnector_.frames(breakpointEvent.thread(), 1);
		auto location = breakpointEvent.location();
		auto variableTable = debugConnector_.variableTableForMethod(location.classId(), location.methodId());
		if (frames.frames().size() == 0)
		{
			qDebug() << "No frames received, error:" << static_cast<qint8>(frames.error());
			return;
		}
		auto currentFrame = frames.frames()[0];
		int currentIndex = currentFrame.location().methodIndex();

		for (auto observer : nodeObservedBy_.values(*it))
		{
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
			observer->handlerFunc_(this, values, observer->valueCalculators_, observer->observerLocation_);
		}
	}
	auto listenerIt = breakpointListeners_.find(*it);
	while (listenerIt != breakpointListeners_.end() && listenerIt.key() == *it)
	{
		listenerIt.value().second(*it, breakpointEvent);
		++listenerIt;
	}

	auto visualization = *Visualization::Item::nodeItemsMap().find(*it);
	currentLineItem_ = visualization;
	// If we have an overlay, the user wants to stop here, otherwise it is a tracked variable and we can resume.
	if (visualization->overlay<Visualization::IconOverlay>(BREAKPOINT_OVERLAY_GROUP))
	{
		currentThreadId_ = breakpointEvent.thread();
		toggleLineHighlight(visualization, true);
	}
	else
	{
		debugConnector_.wantResume(true);
	}
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

void JavaDebugger::handleValues(Values values, QList<Probes::ValueCalculator> valueCalculators, Model::Node* target)
{
	QList<double> doubleValues;
	for (auto val : values.values()) doubleValues << utils_.doubleFromValue(val);
	QList<double> plotValues;
	for (auto extractor : valueCalculators) plotValues << extractor(doubleValues);
	if (plotValues.size() > 1)
		plotOverlayOfNode(target)->addValues(plotValues[0], plotValues.mid(1));
	else if (plotValues.size() == 1)
		plotOverlayOfNode(target)->addValue(plotValues[0]);
}

void JavaDebugger::handleArray(Values values, QList<Probes::ValueCalculator>, Model::Node* target)
{
	auto vals = values.values();
	QList<int> indices;
	for (int i = 1; i < vals.size(); ++i) indices << utils_.doubleFromValue(vals[i]);
	int arrayLen = debugConnector_.arrayLength(vals[0].array());
	auto arrayVals = debugConnector_.arrayValues(vals[0].array(), 0, arrayLen);
	switch (arrayVals.type())
	{
		case Protocol::Tag::FLOAT: return plotOverlayOfNode(target)->updateArrayValues(arrayVals.floats(), indices);
		case Protocol::Tag::DOUBLE: return plotOverlayOfNode(target)->updateArrayValues(arrayVals.doubles(), indices);
		case Protocol::Tag::INT: return plotOverlayOfNode(target)->updateArrayValues(arrayVals.ints(), indices);
		case Protocol::Tag::LONG: return plotOverlayOfNode(target)->updateArrayValues(arrayVals.longs(), indices);
		case Protocol::Tag::SHORT: return plotOverlayOfNode(target)->updateArrayValues(arrayVals.shorts(), indices);
		default: Q_ASSERT(false); // you shouldn't try to convert any non numeric values to double.
	}
}

void JavaDebugger::addBreakpointOverlay(Visualization::Item* target)
{
	auto overlay = new Visualization::IconOverlay{target, Visualization::IconOverlay::itemStyles().get("breakpoint")};
	target->addOverlay(overlay, BREAKPOINT_OVERLAY_GROUP);
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

PlotOverlay* JavaDebugger::plotOverlayOfNode(Model::Node* node)
{
	auto nodeVisualization = Visualization::Item::nodeItemsMap().find(node);
	Q_ASSERT(nodeVisualization != Visualization::Item::nodeItemsMap().end());
	auto overlay = (*nodeVisualization)->overlay<PlotOverlay>(PLOT_OVERLAY_GROUP);
	Q_ASSERT(overlay);
	return overlay;
}

void JavaDebugger::removeObserverOverlaysAt(Model::Node* node, Visualization::Item* nodeVisualization)
{
	auto it = nodeObservedBy_.find(node);
	while (it != nodeObservedBy_.end() && it.key() == node)
	{
		// it might be that there is also an observer for a tracked variable, so only remove the correct one.
		// TODO: this might is wrong if a tracked variable is declared here, but we ignore this for now.
		if (it.value()->observerLocation_ == node)
		{
			nodeObservedBy_.erase(it);
			if (auto plotOverlay = nodeVisualization->overlay<PlotOverlay>(PLOT_OVERLAY_GROUP))
				nodeVisualization->scene()->removeOverlay(plotOverlay);
			if (auto monitorOverlay = nodeVisualization->overlay<Visualization::IconOverlay>(MONITOR_OVERLAY_GROUP))
				nodeVisualization->scene()->removeOverlay(monitorOverlay);
			break;
		}
	}
}

}
