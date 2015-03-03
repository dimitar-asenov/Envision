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
#include "OOModel/src/expressions/Expression.h"
#include "OOModel/src/expressions/types/TypeExpression.h"
#include "OOModel/src/expressions/types/PrimitiveTypeExpression.h"
#include "OOModel/src/expressions/types/ArrayTypeExpression.h"
#include "OOModel/src/expressions/types/ClassTypeExpression.h"
#include "OOModel/src/expressions/VariableDeclarationExpression.h"
#include "OOModel/src/types/PrimitiveType.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/statements/DeclarationStatement.h"
#include "OOModel/src/statements/LoopStatement.h"
#include "OOModel/src/elements/StatementItemList.h"

#include "OOVisualization/src/elements/VStatementItemList.h"

#include "VisualizationBase/src/items/Item.h"
#include "VisualizationBase/src/overlays/OverlayAccessor.h"
#include "VisualizationBase/src/overlays/IconOverlay.h"
#include "VisualizationBase/src/overlays/SelectionOverlay.h"

namespace OODebug {

struct VariableObserver {
		VariableObserver(JavaDebugger::ValueHandler handlerFunction,
							  QList<OOModel::VariableDeclaration*> observedVariables, Model::Node* observerLocation,
							  QList<JavaDebugger::ValueCalculator> valueCalculators = {})
			: handlerFunc_{handlerFunction}, observedVariables_{observedVariables},
			  observerLocation_{observerLocation}, valueCalculators_{valueCalculators}{}

		// The function which handles new value(s).
		JavaDebugger::ValueHandler handlerFunc_;
		// The declarations of the variables we are observing.
		QList<OOModel::VariableDeclaration*> observedVariables_;
		// The location of the observer, this might be useful if it has an attached overlay.
		Model::Node* observerLocation_;
		// Value calculator functions
		QList<JavaDebugger::ValueCalculator> valueCalculators_;
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

bool JavaDebugger::debugTree(Model::TreeManager* manager, const QString& pathToProjectContainerDirectory)
{
	auto project = DCast<OOModel::Project>(manager->root());
	Q_ASSERT(project);

	if (!JavaRunner::runTree(manager, pathToProjectContainerDirectory, true)) return false;

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
			target->scene()->removeOverlay(overlay);
			removeBreakpointAt(node);
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

		// Check if this is tracked and if so remove it.
		// TODO if there is a probe on the same line this is wrong.
		auto it = nodeObservedBy_.find(node);
		if (it != nodeObservedBy_.end())
		{
			auto ptr = it.value();
			removeObserverOverlaysAt(node, target);
			// remove all observers at the references:
			it = nodeObservedBy_.begin();
			while (it != nodeObservedBy_.end())
			{
				if (it.value() == ptr) it = nodeObservedBy_.erase(it);
				else ++it;
			}
			return true;
		}

		ReferenceFinder refFinder;
		refFinder.setSearchNode(node);
		auto containingMethod = node->firstAncestorOfType<OOModel::Method>();
		refFinder.visit(containingMethod);

		auto defaultTypeAndHandler = defaultPlotTypeAndValueHandlerFor({variableDeclaration});
		auto overlay = new PlotOverlay(target, PlotOverlay::itemStyles().get("default"), defaultTypeAndHandler.first);
		target->addOverlay(overlay, PLOT_OVERLAY_GROUP);
		auto observer = std::make_shared<VariableObserver>
				(VariableObserver(defaultTypeAndHandler.second, {variableDeclaration}, node));
		nodeObservedBy_.insertMulti(node, observer);
		for (auto ref : refFinder.references())
		{
			nodeObservedBy_.insertMulti(ref, observer);
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
		debugConnector_.singleStep(currentThreadId_);
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
	auto observedNode = vItem->node();

	if (arguments[0] == "-")
	{
		removeObserverOverlaysAt(observedNode, vItem);
		removeBreakpointAt(observedNode);
		return;
	}

	auto parsedArgs = parseProbeArguments(arguments);
	QStringList variableNames = parsedArgs.second;

	QHash<QString, OOModel::VariableDeclaration*> declarationMap;
	auto statementList = DCast<OOModel::StatementItemList>(itemList->node());
	while (statementList)
	{
		for (int idx = itemIndex; idx >= 0; --idx)
		{
			for (auto varName : variableNames)
				if (!declarationMap.contains(varName))
					if (auto decl = variableDeclarationFromStatement(statementList->at(idx), varName))
						 declarationMap[varName] = decl;
		}
		auto itemInParentList = statementList->firstAncestorOfType<OOModel::StatementItem>();
		statementList = nullptr; // we finished with this list
		if (itemInParentList && declarationMap.size() < variableNames.size())
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
	if (declarationMap.size() < variableNames.size())
	{
		// Here we should probably notify the user
		qDebug() << "Not all declarations found for probe: " << arguments;
		return;
	}

	QList<OOModel::VariableDeclaration*> vars;
	for (auto varName : variableNames) vars << declarationMap[varName];

	auto defaultTypeAndHandler = defaultPlotTypeAndValueHandlerFor(vars);
	auto observer = std::make_shared<VariableObserver>
			(VariableObserver(defaultTypeAndHandler.second, vars, observedNode, parsedArgs.first));
	nodeObservedBy_.insertMulti(observedNode, observer);
	unsetBreakpoints_ << observedNode;

	auto overlay = new Visualization::IconOverlay(vItem, Visualization::IconOverlay::itemStyles().get("monitor"));
	vItem->addOverlay(overlay, MONITOR_OVERLAY_GROUP);

	auto plotOverlay = new PlotOverlay(vItem, PlotOverlay::itemStyles().get("default"),
												  defaultTypeAndHandler.first, variableNames);
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
	qint64 id = debugConnector_.classIdOf(jvmSignatureFor(containerClass));
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
	qint64 classId =  debugConnector_.classIdOf(jvmSignatureFor(containerClass));
	Q_ASSERT(classId != debugConnector_.NO_RESULT);
	QString methodName = method->name();
	if (method->methodKind() == OOModel::Method::MethodKind::Constructor)
		methodName = "<init>";
	// TODO: function to get signature of a method: for Java classes we would need the full java library.
	// Once fixed also fix the implementation of getMethodId().
	qint64 methodId = debugConnector_.methodIdOf(classId, methodName);
	Q_ASSERT(methodId != debugConnector_.NO_RESULT);

	auto tagKind = Protocol::TypeTagKind::CLASS;
	if (containerClass->constructKind() == OOModel::Class::ConstructKind::Interface)
		tagKind = Protocol::TypeTagKind::INTERFACE;
	else if (containerClass->constructKind() != OOModel::Class::ConstructKind::Class)
		Q_ASSERT(false); // This should not happen for a Java project!

	auto locations = exportMap_->locations(node);
	int line = locations.at(0).span_.startLine_;
	for (auto loc : locations) line = std::min(line, loc.span_.startLine_);
	++line; // Envision is 0 indexed and java 1 indexed

	// get line info for this method.
	// -2 because -1 is a valid code index for native methods, see:
	// http://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html#JDWP_Method_VariableTable
	static constexpr qint64 NO_INDEX = -2;
	qint64 methodIndex = NO_INDEX;
	auto lineTable = debugConnector_.lineTable(classId, methodId);
	for (auto val : lineTable.mappings())
		if (line == val.lineNumber()) methodIndex = val.lineCodeIndex();

	Q_ASSERT(methodIndex != NO_INDEX);
	return Location(tagKind, classId, methodId, methodIndex);
}

Model::Node* JavaDebugger::locationToNode(Location location, bool& isClosingBracket)
{
	QString signature = debugConnector_.signatureOf(location.classId());
	signature = signature.mid(1, signature.size() - 2); // remove symbol at start and ; at end.
	QString fileName = QString("src/%1.java").arg(signature);

	int line = -1;
	auto lineTable = debugConnector_.lineTable(location.classId(), location.methodId());
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
		if (auto stmtItem = DCast<OOModel::StatementItem>(node)) return stmtItem;
		if (auto stmt = node->firstAncestorOfType<OOModel::StatementItem>()) return stmt;
		// If we are at the closing bracket of a method, the node will be a StatementItemList, thus we just highlight
		// the last item in this list.
		// TODO: if we could highlight somehow the end of the method this would be the better solution.
		if (auto stmtList = DCast<OOModel::StatementItemList>(node))
		{
			isClosingBracket = true;
			return stmtList->at(stmtList->size() -1);
		}
	}
	Q_ASSERT(false); // We should find a node!
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
			for (auto variableDecl : observer->observedVariables_)
			{
				for (auto variableDetails : variableTable.variables())
				{
					if (variableDetails.name() == variableDecl->name())
					{
						// Condition as in: http://docs.oracle.com/javase/7/docs/platform/jpda/jdwp/jdwp-protocol.html
						//                    #JDWP_Method_VariableTable
						Q_ASSERT(variableDetails.codeIndex() <= currentIndex &&
									currentIndex < variableDetails.codeIndex() + variableDetails.length());
						varsToGet << StackVariable(variableDetails.slot(),
															typeExpressionToTag(variableDecl->typeExpression()));
					}
				}
			}
			auto values = debugConnector_.values(breakpointEvent.thread(), currentFrame.frameID(), varsToGet);
			Q_ASSERT(observer->handlerFunc_);
			observer->handlerFunc_(this, values, observer->valueCalculators_, observer->observerLocation_);
		}
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
	bool closingBracket = false;
	auto node = locationToNode(singleStep.location(), closingBracket);

	// It might be that we have a breakpoint on the same location so cancel its resume.
	debugConnector_.cancelResume();

	currentThreadId_ = singleStep.thread();
	auto visualization = *Visualization::Item::nodeItemsMap().find(node);
	currentLineItem_ = visualization;
	toggleLineHighlight(currentLineItem_, true, closingBracket);
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
	OOModel::VariableDeclarationExpression* varDeclarationExpr = nullptr;
	if (auto exprStmt = DCast<OOModel::ExpressionStatement>(statement))
	{
		varDeclarationExpr = DCast<OOModel::VariableDeclarationExpression>(exprStmt->expression());
	}
	else if (auto declStmt = DCast<OOModel::DeclarationStatement>(statement))
	{
		auto decl = DCast<OOModel::VariableDeclaration>(declStmt->declaration());
		if (decl && (variableName.isEmpty() || decl->name() == variableName))
			variableDeclaration = decl;
	}
	else if (auto loopStmt = DCast<OOModel::LoopStatement>(statement))
	{
		varDeclarationExpr = DCast<OOModel::VariableDeclarationExpression>(loopStmt->initStep());
	}
	if (!variableDeclaration && varDeclarationExpr)
	{
		auto decl = varDeclarationExpr->decl();
		if (decl && (variableName.isEmpty() || decl->name() == variableName))
			variableDeclaration = decl;
	}
	return variableDeclaration;
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
			auto overlay = new Visualization::IconOverlay(
						item, Visualization::IconOverlay::itemStyles().get("endOfMethod"));
			item->addOverlay(overlay, CURRENT_LINE_OVERLAY_GROUP);
		}
		else
		{
			if (existingLineOverlay) return;
			auto overlay = new Visualization::SelectionOverlay(
						item, Visualization::SelectionOverlay::itemStyles().get("currentStatement"));
			item->addOverlay(overlay, CURRENT_LINE_OVERLAY_GROUP);
		}

	}
	else
	{
		if (existingLineOverlay) item->scene()->removeOverlay(existingLineOverlay);
		if (existingBracketOverlay) item->scene()->removeOverlay(existingBracketOverlay);
	}
}

QPair<PlotOverlay::PlotType, JavaDebugger::ValueHandler> JavaDebugger::defaultPlotTypeAndValueHandlerFor(
		QList<OOModel::VariableDeclaration*> variableDeclarations)
{
	Q_ASSERT(!variableDeclarations.empty());

	if (hasPrimitiveValueType(variableDeclarations[0]))
	{
		bool allPrimitive = true;
		for (auto decl : variableDeclarations)
			if (!hasPrimitiveValueType(decl)) allPrimitive = false;
		if (allPrimitive)
		{
			if (variableDeclarations.size() > 1)
				return {PlotOverlay::PlotType::Scatter, &JavaDebugger::handleValues};
			else
				return {PlotOverlay::PlotType::Bars, &JavaDebugger::handleValues};
		}
	}
	else if (hasArrayType(variableDeclarations[0]))
	{
		return {PlotOverlay::PlotType::Array, &JavaDebugger::handleArray};
	}

	Q_ASSERT(false); // We should implement something for this combination
}

QPair<QList<JavaDebugger::ValueCalculator>, QStringList> JavaDebugger::parseProbeArguments(QStringList arguments)
{

	static const QRegularExpression NUMBER_REGEX{"^(\\d+)$"};
	static const QRegularExpression OPERATOR_REGEX{"^([+\\-*/]?)$"};

	QList<ValueCalculator> calculators;
	QStringList variableNames;
	for (int i = 0; i < arguments.size(); ++i)
	{
		QString arg = arguments[i];
		if (!OPERATOR_REGEX.match(arg).hasMatch())
		{
			bool isNumber = NUMBER_REGEX.match(arg).hasMatch();
			double value = 0;
			if (isNumber) value = arg.toDouble();
			int index = variableNames.indexOf(arg);
			if (!isNumber && -1 == index)
			{
				index = variableNames.size();
				variableNames << arg;
			}
			// lookahead
			if (i + 1 < arguments.size() && OPERATOR_REGEX.match(arguments[i+1]).hasMatch())
			{
				// TODO: If after a operator we do not have anything we just fail here:
				if (i + 2 >= arguments.size()) Q_ASSERT(false);

				auto opFunc = operatorFromString(arguments[i+1]);

				bool isNumber2 = NUMBER_REGEX.match(arguments[i+2]).hasMatch();
				double value2 = 0;
				if (isNumber2) value2 = arguments[i+2].toDouble();
				int index2 = variableNames.indexOf(arguments[i+2]);
				if (!isNumber2 && -1 == index2)
				{
					index2 = variableNames.size();
					variableNames << arguments[i+2];
				}
				if (isNumber && isNumber2)
					calculators << [value, value2, opFunc](QList<double>)
						{return opFunc(value, value2);};
				else if (isNumber)
					calculators <<  [value, index2, opFunc](QList<double> values)
						{return opFunc(value, values[index2]);};
				else if (isNumber2)
					calculators <<  [index, value2, opFunc](QList<double> values)
						{return opFunc(values[index], value2);};
				else
					calculators << [index, index2, opFunc](QList<double> values)
						{return opFunc(values[index], values[index2]);};
				i += 2;
			}
			else
			{
				// only single argument
				if (isNumber)
					calculators << [value](QList<double>) { return value; };
				else
					calculators << ([index](QList<double> values) { return values[index];});
			}
		}
	}
	return {calculators, variableNames};
}

JavaDebugger::ValueOperator JavaDebugger::operatorFromString(QString operatorString)
{
	if (operatorString == "+") return [](double a, double b) { return a + b; };
	else if (operatorString == "-") return [](double a, double b) { return a - b; };
	else if (operatorString == "*") return [](double a, double b) { return a * b; };
	else if (operatorString == "/") return [](double a, double b) { return a / b; };
	Q_ASSERT(false);
}

void JavaDebugger::handleValues(Values values, QList<ValueCalculator> valueCalculators, Model::Node* target)
{
	QList<double> doubleValues;
	for (auto val : values.values()) doubleValues << doubleFromValue(val);
	QList<double> plotValues;
	for (auto extractor : valueCalculators) plotValues << extractor(doubleValues);
	if (plotValues.size() > 1)
		plotOverlayOfNode(target)->addValue(plotValues[0], plotValues[1]);
	else if (plotValues.size() == 1)
		plotOverlayOfNode(target)->addValue(plotValues[0]);
}

void JavaDebugger::handleArray(Values values, QList<ValueCalculator>, Model::Node* target)
{
	auto vals = values.values();
	QList<int> indices;
	for (int i = 1; i < vals.size(); ++i) indices << doubleFromValue(vals[i]);
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

double JavaDebugger::doubleFromValue(Value v)
{
	switch (v.type())
	{
		case Protocol::Tag::FLOAT: return v.floatValue();
		case Protocol::Tag::DOUBLE: return v.doubleValue();
		case Protocol::Tag::INT: return v.intValue();
		case Protocol::Tag::LONG: return v.longValue();
		case Protocol::Tag::SHORT: return v.shortValue();
		default: Q_ASSERT(false); // you shouldn't try to convert any non numeric values to double.
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

bool JavaDebugger::hasPrimitiveValueType(OOModel::VariableDeclaration* decl)
{
	if (auto typeExpression = DCast<OOModel::TypeExpression>(decl->typeExpression()))
	{
		if (auto primitiveType = DCast<OOModel::PrimitiveTypeExpression>(typeExpression))
		{
			switch (primitiveType->typeValue())
			{
				case OOModel::PrimitiveType::INT: return true;
				case OOModel::PrimitiveType::LONG: return true;
				case OOModel::PrimitiveType::UNSIGNED_INT: return true;
				case OOModel::PrimitiveType::UNSIGNED_LONG: return true;
				case OOModel::PrimitiveType::FLOAT: return true;
				case OOModel::PrimitiveType::DOUBLE: return true;
				case OOModel::PrimitiveType::BOOLEAN: return true;
				case OOModel::PrimitiveType::CHAR: return true;
				default: break;
			}
		}
	}
	return false;
}

bool JavaDebugger::hasArrayType(OOModel::VariableDeclaration* decl)
{
	if (auto typeExpression = DCast<OOModel::TypeExpression>(decl->typeExpression()))
		if (DCast<OOModel::ArrayTypeExpression>(typeExpression)) return true;
	return false;
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

} /* namespace OODebug */
