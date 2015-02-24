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

#include "overlays/PlotOverlay.h"

#include "JavaExport/src/exporter/JavaExporter.h"

#include "ModelBase/src/nodes/Node.h"

#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/expressions/Expression.h"
#include "OOModel/src/expressions/types/TypeExpression.h"
#include "OOModel/src/expressions/types/PrimitiveTypeExpression.h"
#include "OOModel/src/expressions/types/ArrayTypeExpression.h"
#include "OOModel/src/expressions/types/ClassTypeExpression.h"
#include "OOModel/src/expressions/VariableDeclarationExpression.h"
#include "OOModel/src/types/PrimitiveType.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/statements/DeclarationStatement.h"
#include "OOModel/src/elements/StatementItemList.h"

#include "OOVisualization/src/elements/VStatementItemList.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"
#include "VisualizationBase/src/overlays/IconOverlay.h"
#include "VisualizationBase/src/overlays/SelectionOverlay.h"

namespace OODebug {

const QString JavaDebugger::BREAKPOINT_OVERLAY_GROUP{"Breakpoint overlay"};
const QString JavaDebugger::PLOT_OVERLAY_GROUP{"PlotOverlay"};
const QString JavaDebugger::CURRENT_LINE_OVERLAY_GROUP{"CurrentLine"};
const QString JavaDebugger::MONITOR_OVERLAY_GROUP{"MonitorOverlay"};

JavaDebugger& JavaDebugger::instance()
{
	static JavaDebugger instance;
	return instance;
}

bool JavaDebugger::debugTree(Model::TreeManager* manager, const QString& pathToProjectContainerDirectory)
{
	auto project = DCast<OOModel::Project>(manager->root());
	Q_ASSERT(project);

	if (!JavaRunner::runTree(manager, pathToProjectContainerDirectory, true)) return false;

	// All previously set breakpoints have to be unset again.
	unsetBreakpoints_ << setBreakpoints_.values();
	setBreakpoints_.clear();
	breakOnLoadClasses_.clear();

	// Reset probes
	for (auto probeIt = probes_.cbegin(); probeIt != probes_.cend(); ++probeIt)
	{
		auto probeNode = probeIt.key();
		auto visualizationIt = Visualization::Item::nodeItemsMap().find(probeNode);
		Q_ASSERT(visualizationIt != Visualization::Item::nodeItemsMap().end());
		auto overlay = (*visualizationIt)->overlay<PlotOverlay>(PLOT_OVERLAY_GROUP);
		Q_ASSERT(overlay);
		overlay->clearValues();
	}

	exportMap_ = JavaExport::JavaExporter::exportMaps().map(project);
	debugConnector_.connect();
	return true;
}

bool JavaDebugger::toggleBreakpoint(Visualization::Item* target, QKeyEvent* event)
{
	if (event->modifiers() == Qt::NoModifier && (event->key() == Qt::Key_F8))
	{
		auto node = target->node();
		if (auto overlay = target->overlay<Visualization::IconOverlay>(BREAKPOINT_OVERLAY_GROUP))
		{
			if (currentLineItem_ == target) currentLineItem_ = nullptr;
			target->scene()->removeOverlay(overlay);
			if (debugConnector_.vmAlive())
			{
				int index = unsetBreakpoints_.indexOf(node);
				if (index > 0)
				{
					unsetBreakpoints_.removeAt(index);
				}
				else
				{
					qint32 breakpointId = setBreakpoints_.key(node);
					debugConnector_.clearBreakpoint(breakpointId);
					setBreakpoints_.remove(breakpointId);
				}
			}
		}
		else
		{
			addBreakpointOverlay(target);
			if (debugConnector_.vmAlive())
			{
				if (isParentClassLoaded(node))
				{
					qint32 requestId = debugConnector_.setBreakpoint(nodeToLocation(node));
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
		auto variableDeclaration = variableDeclarationFromStatement(DCast<OOModel::StatementItem>(node));
		if (!variableDeclaration) return false;

		auto overlay = new PlotOverlay(target);
		target->addOverlay(overlay, PLOT_OVERLAY_GROUP);

		ReferenceFinder refFinder;
		refFinder.setSearchNode(node);
		auto containingMethod = node->firstAncestorOfType<OOModel::Method>();
		refFinder.visit(containingMethod);
		for (auto ref : refFinder.references())
		{
			trackedVariables_[ref] = variableDeclaration;
			unsetBreakpoints_ << ref;
		}
		return true;
	}
	return false;
}

bool JavaDebugger::step(Visualization::Item*, QKeyEvent* event)
{
	if (event->modifiers() == Qt::ControlModifier && (event->key() == Qt::Key_1))
	{
		debugConnector_.singleStep(currentThreadId_, Protocol::StepSize::LINE, Protocol::StepDepth::OVER);
		resume();
		return true;
	}
	return false;
}

void JavaDebugger::probe(OOVisualization::VStatementItemList* itemList, const QStringList& arguments, int itemIndex)
{
	Q_ASSERT(!arguments.empty());
	if (itemIndex == itemList->rangeEnd()) --itemIndex; // TODO handle this properly
	auto vItem = itemList->itemAt<Visualization::Item>(itemIndex);
	Q_ASSERT(vItem);

	QString xName = arguments[0];
	QString yName{};
	if (arguments.size() > 1) yName = arguments[1];

	OOModel::VariableDeclaration* xDeclaration = nullptr;
	OOModel::VariableDeclaration* yDeclaration = nullptr;
	auto statementList = DCast<OOModel::StatementItemList>(itemList->node());
	while (statementList)
	{
		for (int idx = itemIndex; idx >= 0; --idx)
		{
			if (!xDeclaration) xDeclaration = variableDeclarationFromStatement(statementList->at(idx), xName);
			if (!yDeclaration && !yName.isEmpty())
				yDeclaration = variableDeclarationFromStatement(statementList->at(idx), yName);
		}
		auto itemInParentList = statementList->firstAncestorOfType<OOModel::StatementItem>();
		statementList = nullptr; // we finished with this list
		if (itemInParentList && (!xDeclaration || (!yName.isEmpty() && !yDeclaration)))
		{
			// search in parent lists
			statementList = itemInParentList->firstAncestorOfType<OOModel::StatementItemList>();
			itemIndex = 0;
			for (auto it : *statementList)
			{
				if (it == itemInParentList) break;
				++itemIndex;
			}
		}
	}
	Q_ASSERT(xDeclaration);	Q_ASSERT(xDeclaration->name() == xName);

	probes_[vItem->node()] = {xDeclaration, yDeclaration};
	unsetBreakpoints_ << vItem->node();

	auto overlay = new Visualization::IconOverlay(vItem, Visualization::IconOverlay::itemStyles().get("monitor"));
	vItem->addOverlay(overlay, MONITOR_OVERLAY_GROUP);

	auto plotType = PlotOverlay::PlotType::Bars;
	if (yDeclaration) plotType = PlotOverlay::PlotType::Scatter;

	auto plotOverlay = new PlotOverlay(vItem, PlotOverlay::itemStyles().get("default"), plotType);
	vItem->addOverlay(plotOverlay, PLOT_OVERLAY_GROUP);
}

JavaDebugger::JavaDebugger()
{
	debugConnector_.addEventListener(Protocol::EventKind::CLASS_PREPARE, [this] (Event e) { handleClassPrepare(e);});
	debugConnector_.addEventListener(Protocol::EventKind::BREAKPOINT,
												[this] (Event e) { handleBreakpoint(e.breakpoint()); });
	debugConnector_.addEventListener(Protocol::EventKind::VM_START, [this] (Event e) { handleVMStart(e); });
	debugConnector_.addEventListener(Protocol::EventKind::SINGLE_STEP,
												[this] (Event e) { handleSingleStep(e.singleStep()); });
}

void JavaDebugger::addBreakpointOverlay(Visualization::Item* target)
{
	auto overlay = new Visualization::IconOverlay(target, Visualization::IconOverlay::itemStyles().get("breakpoint"));
	target->addOverlay(overlay, BREAKPOINT_OVERLAY_GROUP);
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

bool JavaDebugger::isParentClassLoaded(Model::Node* node)
{
	auto containerClass = node->firstAncestorOfType<OOModel::Class>();
	qint64 id = debugConnector_.getClassId(jvmSignatureFor(containerClass));
	return id != DebugConnector::NO_RESULT;
}

void JavaDebugger::breaktAtParentClassLoad(Model::Node* node)
{
	auto containerClass = node->firstAncestorOfType<OOModel::Class>();
	if (!breakOnLoadClasses_.contains(containerClass))
	{
		debugConnector_.breakAtClassLoad(fullNameFor(containerClass, '.'));
		breakOnLoadClasses_.insert(containerClass);
	}
}

Location JavaDebugger::nodeToLocation(Model::Node* node)
{
	auto method = node->firstAncestorOfType<OOModel::Method>();
	auto containerClass = method->firstAncestorOfType<OOModel::Class>();
	qint64 classId =  debugConnector_.getClassId(jvmSignatureFor(containerClass));
	Q_ASSERT(classId != debugConnector_.NO_RESULT);
	QString methodName = method->name();
	if (method->methodKind() == OOModel::Method::MethodKind::Constructor)
		methodName = "<init>";
	// TODO: function to get signature of a method: for Java classes we would need the full java library.
	// Once fixed also fix the implementation of getMethodId().
	qint64 methodId = debugConnector_.getMethodId(classId, methodName);
	Q_ASSERT(methodId != debugConnector_.NO_RESULT);

	auto tagKind = Protocol::TypeTagKind::CLASS;
	if (containerClass->constructKind() == OOModel::Class::ConstructKind::Interface)
		tagKind = Protocol::TypeTagKind::INTERFACE;
	else if (containerClass->constructKind() != OOModel::Class::ConstructKind::Class)
		Q_ASSERT(false); // This should not happen for a Java project!

	auto locations = exportMap_->locations(node);
	int line = locations.at(0).span_.startLine_ + 1;

	// get line info for this method.
	// -2 because -1 is a valid code index for native methods, see:
	// http://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html#JDWP_Method_VariableTable
	static constexpr qint64 NO_INDEX = -2;
	qint64 methodIndex = NO_INDEX;
	auto lineTable = debugConnector_.getLineTable(classId, methodId);
	for (auto val : lineTable.mappings())
		if (line == val.lineNumber()) methodIndex = val.lineCodeIndex();

	Q_ASSERT(methodIndex != NO_INDEX);
	return Location(tagKind, classId, methodId, methodIndex);
}

Model::Node* JavaDebugger::locationToNode(Location location)
{
	QString signature = debugConnector_.getSignature(location.classId());
	signature = signature.mid(1, signature.size() - 2); // remove symbol at start and ; at end.
	QString fileName = QString("src/%1.java").arg(signature);

	int line = -1;
	auto lineTable = debugConnector_.getLineTable(location.classId(), location.methodId());
	for (auto val : lineTable.mappings())
	{
		if (location.methodIndex() <= val.lineCodeIndex())
		{
			line = val.lineNumber();
			break;
		}
	}
	Q_ASSERT(line >= 0);

	if (auto node = exportMap_->node(fileName, line - 1, 0))
	{
		if (auto casted = DCast<OOModel::StatementItem>(node)) return casted;
		if (auto stmt = node->firstAncestorOfType<OOModel::StatementItem>()) return stmt;
		// If we are at the closing bracket of a method, the node will be a StatementItemList, thus we just highlight
		// the last item in this list.
		// TODO: if we could highlight somehow the end of the method this would be the better solution.
		if (auto stmtList = DCast<OOModel::StatementItemList>(node)) return stmtList->at(stmtList->size() -1);
	}
	if (!fileName.contains("java/lang"))
		Q_ASSERT(false); // for locations which are in "our" code we should find a node!
	return nullptr;
}

void JavaDebugger::resume()
{
	debugConnector_.resume();
	if (currentLineItem_)
	{
		toggleLineHighlight(currentLineItem_, false);
		currentLineItem_ = nullptr;
	}
	currentThreadId_ = 0;
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
			breakpointLocations[nodeToLocation(*it)] = *it;
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

void JavaDebugger::handleVMStart(Event)
{
	trySetBreakpoints();
	debugConnector_.resume();
}

void JavaDebugger::handleClassPrepare(Event)
{
	trySetBreakpoints();
	debugConnector_.resume();
}

void JavaDebugger::handleBreakpoint(BreakpointEvent breakpointEvent)
{
	auto it = setBreakpoints_.find(breakpointEvent.requestID());
	// If we get a event for a breakpoint we don't know we have an implementation error.
	Q_ASSERT(it != setBreakpoints_.end() && *it);
	if (trackedVariables_.contains(*it))
	{
		// Get frames
		auto frames = debugConnector_.getFrames(breakpointEvent.thread(), 1);
		auto location = breakpointEvent.location();
		auto variableTable = debugConnector_.getVariableTable(location.classId(), location.methodId());
		if (frames.frames().size() == 0)
		{
			qDebug() << "No frames received, error:" << static_cast<qint8>(frames.error());
			return;
		}
		auto currentFrame = frames.frames()[0];
		int currentIndex = currentFrame.location().methodIndex();

		auto trackedVariable = trackedVariables_[*it];
		Q_ASSERT(trackedVariable);

		QList<StackVariable> varsToGet;
		for (auto variableDetails : variableTable.variables())
		{
			if (variableDetails.name() == trackedVariable->name())
			{
				// Condition as in: http://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html
				//                    #JDWP_Method_VariableTable
				Q_ASSERT(variableDetails.codeIndex() <= currentIndex &&
							currentIndex < variableDetails.codeIndex() + variableDetails.length());
				varsToGet << StackVariable(variableDetails.slot(),
													typeExpressionToTag(trackedVariable->typeExpression()));
			}
		}

		auto values = debugConnector_.getValues(breakpointEvent.thread(), currentFrame.frameID(), varsToGet);
		Q_ASSERT(values.values().length() == varsToGet.length());
		// The variable declaration itself doesn't have a visualization but its statement parent:
		auto variableStatement = trackedVariable->firstAncestorOfType<OOModel::Statement>();
		auto visualizationIt = Visualization::Item::nodeItemsMap().find(variableStatement);
		Q_ASSERT(visualizationIt != Visualization::Item::nodeItemsMap().end());
		for (auto val : values.values())
		{
			if (val.kind() == MessagePart::cast(Protocol::Tag::INT))
			{
				auto overlay = (*visualizationIt)->overlay<PlotOverlay>(PLOT_OVERLAY_GROUP);
				Q_ASSERT(overlay);
				overlay->addValue(val.intValue());
				qDebug() << trackedVariable->name() << ":\t" << val.intValue();
			}
			else if (val.kind() == MessagePart::cast(Protocol::Tag::BOOLEAN))
				qDebug() << trackedVariable->name() << ":\t" << val.boolean();
			else if (val.kind() == MessagePart::cast(Protocol::Tag::STRING))
				qDebug() << trackedVariable->name() << ":\t" << debugConnector_.getString(val.stringId());
		}
	}
	else if (probes_.contains(*it))
	{
		// Get frames
		auto frames = debugConnector_.getFrames(breakpointEvent.thread(), 1);
		auto location = breakpointEvent.location();
		auto variableTable = debugConnector_.getVariableTable(location.classId(), location.methodId());
		if (frames.frames().size() == 0)
		{
			qDebug() << "No frames received, error:" << static_cast<qint8>(frames.error());
			return;
		}
		auto currentFrame = frames.frames()[0];
		int currentIndex = currentFrame.location().methodIndex();

		auto xVariable = probes_[*it].first;
		Q_ASSERT(xVariable);
		auto yVariable = probes_[*it].second;

		QList<StackVariable> varsToGet;
		int xInd = 0, yInd = 0;
		for (int i = 0; i < variableTable.variables().size(); ++i)
		{
			auto variableDetails = variableTable.variables()[i];
			if (variableDetails.name() == xVariable->name())
			{
				// Condition as in: http://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html
				//                    #JDWP_Method_VariableTable
				Q_ASSERT(variableDetails.codeIndex() <= currentIndex &&
							currentIndex < variableDetails.codeIndex() + variableDetails.length());
				varsToGet << StackVariable(variableDetails.slot(),
													typeExpressionToTag(xVariable->typeExpression()));
				xInd = i;
			}
			else if (yVariable && variableDetails.name() == yVariable->name())
			{
				Q_ASSERT(variableDetails.codeIndex() <= currentIndex &&
							currentIndex < variableDetails.codeIndex() + variableDetails.length());
				varsToGet << StackVariable(variableDetails.slot(),
													typeExpressionToTag(yVariable->typeExpression()));
				yInd = i;
			}
		}
		bool xFirst = xInd < yInd;

		auto values = debugConnector_.getValues(breakpointEvent.thread(), currentFrame.frameID(), varsToGet);
		Q_ASSERT(values.values().length() == varsToGet.length());
		auto nodeVisualization = Visualization::Item::nodeItemsMap().find(*it);
		Q_ASSERT(nodeVisualization != Visualization::Item::nodeItemsMap().end());
		auto overlay = (*nodeVisualization)->overlay<PlotOverlay>(PLOT_OVERLAY_GROUP);
		Q_ASSERT(overlay);
		auto vals = values.values();
		if (vals.size() == 1)
			overlay->addValue(vals[0].intValue());
		else if (vals.size() == 2)
			overlay->addValue(vals[!xFirst].intValue(), vals[xFirst].intValue());
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
		debugConnector_.resume();
	}
}

void JavaDebugger::handleSingleStep(SingleStepEvent singleStep)
{
	auto node = locationToNode(singleStep.location());
	if (!node)
	{
		// TODO: This might not always be what we want:
		// The step location is in a java file, so just do another step
		debugConnector_.singleStep(singleStep.thread(), Protocol::StepSize::LINE, Protocol::StepDepth::OUT);
		resume();
		return;
	}

	currentThreadId_ = singleStep.thread();
	auto visualization = *Visualization::Item::nodeItemsMap().find(node);
	currentLineItem_ = visualization;
	toggleLineHighlight(currentLineItem_, true);
}

Protocol::Tag JavaDebugger::typeOfVariable(OOModel::Method* containingMethod, VariableDetails variable)
{
	int numArgs = containingMethod->arguments()->size();
	// Member functions have the this pointer as first argument
	int varSlot = containingMethod->isStatic() ? variable.slot() : variable.slot() - 1;
	if (varSlot < numArgs)
	{
		auto typeExpression = containingMethod->arguments()->at(varSlot)->typeExpression();
		return typeExpressionToTag(typeExpression);
	}
	else
	{
		int neededIndex = varSlot - numArgs;
		int currentIndex = 0;
		for (auto item : *containingMethod->items())
		{
			if (item->symbolType() == Model::Node::VARIABLE)
			{
				if (currentIndex == neededIndex)
				{
					auto variableDeclaration = variableDeclarationFromStatement(item);
					Q_ASSERT(variableDeclaration);
					if (variableDeclaration->name() != variable.name())
						qDebug() << "Name differs, Decl:" << variableDeclaration->name() << "var" << variable.name();
					return typeExpressionToTag(variableDeclaration->typeExpression());
				}
				++currentIndex;
			}
		}
	}
	// If we don't find the declration something is wrong
	Q_ASSERT(false);
}

Protocol::Tag JavaDebugger::typeExpressionToTag(OOModel::Expression* e)
{
	if (auto typeExpression = DCast<OOModel::TypeExpression>(e))
	{
		if (auto primitiveType = DCast<OOModel::PrimitiveTypeExpression>(typeExpression))
		{
			switch (primitiveType->typeValue())
			{
				case OOModel::PrimitiveType::INT:
					return Protocol::Tag::INT;
				case OOModel::PrimitiveType::LONG:
					return Protocol::Tag::LONG;
				case OOModel::PrimitiveType::UNSIGNED_INT:
					return Protocol::Tag::INT;
				case OOModel::PrimitiveType::UNSIGNED_LONG:
					return Protocol::Tag::LONG;
				case OOModel::PrimitiveType::FLOAT:
					return Protocol::Tag::FLOAT;
				case OOModel::PrimitiveType::DOUBLE:
					return Protocol::Tag::DOUBLE;
				case OOModel::PrimitiveType::BOOLEAN:
					return Protocol::Tag::BOOLEAN;
				case OOModel::PrimitiveType::CHAR:
					return Protocol::Tag::CHAR;
				case OOModel::PrimitiveType::VOID:
					return Protocol::Tag::VOID;
			}
		}
		if (DCast<OOModel::ArrayTypeExpression>(typeExpression)) return Protocol::Tag::ARRAY;
		if (auto classTypeExpression = DCast<OOModel::ClassTypeExpression>(typeExpression))
		{
			auto referenceExpression = classTypeExpression->typeExpression();
			if (referenceExpression->name() == "String") return Protocol::Tag::STRING;
			return Protocol::Tag::CLASS_OBJECT;
		}
	}
	else if (auto referenceExpression = DCast<OOModel::ReferenceExpression>(e))
	{
		if (referenceExpression->name() == "String") return Protocol::Tag::STRING;
		if (DCast<OOModel::Class>(referenceExpression->target())) return Protocol::Tag::CLASS_OBJECT;
		// TODO: Handle this properly
		// Here we hit if we don't know what the target points to, thus we don't know if we deal with a class object.
		// This will probably catch us if we debug in a method with variables of Type of any "in-built" Java class
		// (except String, which is handled above)
		// Q_ASSERT(false);

		// The assertion is disabled for testing purposes.
		qDebug() << "JAVADEBUGGER: WARNING: Reference expression where target type is uknown.";
		return Protocol::Tag::CLASS_OBJECT;
	}
	// No other types possible, or we have to implement it!
	Q_ASSERT(false);
}

OOModel::VariableDeclaration* JavaDebugger::variableDeclarationFromStatement(OOModel::StatementItem* statement,
																									 QString variableName)
{
	OOModel::VariableDeclaration* variableDeclaration = nullptr;
	if (auto exprStmt = DCast<OOModel::ExpressionStatement>(statement))
	{
		if (auto varDeclarationExpr = DCast<OOModel::VariableDeclarationExpression>(exprStmt->expression()))
		{
			auto decl = varDeclarationExpr->decl();
			if (decl && (variableName.isEmpty() || decl->name() == variableName))
				variableDeclaration = decl;
		}
	}
	else if (auto declStmt = DCast<OOModel::DeclarationStatement>(statement))
	{
		auto decl = DCast<OOModel::VariableDeclaration>(declStmt->declaration());
		if (decl && (variableName.isEmpty() || decl->name() == variableName))
			variableDeclaration = decl;
	}
	return variableDeclaration;
}

void JavaDebugger::toggleLineHighlight(Visualization::Item* item, bool highlight)
{
	if (highlight)
	{
		auto overlay = new Visualization::SelectionOverlay(
					item, Visualization::SelectionOverlay::itemStyles().get("currentStatement"));
		item->addOverlay(overlay, CURRENT_LINE_OVERLAY_GROUP);
	}
	else
	{
		if (auto overlay = item->overlay<Visualization::SelectionOverlay>(CURRENT_LINE_OVERLAY_GROUP))
			 item->scene()->removeOverlay(overlay);
	}
}

} /* namespace OODebug */
