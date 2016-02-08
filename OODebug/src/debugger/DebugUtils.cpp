/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "DebugUtils.h"

#include "jdwp/DataTypes.h"
#include "jdwp/messages/AllMessages.h"
#include "jdwp/DebugConnector.h"

#include "JavaExport/src/exporter/JavaExporter.h"

#include "ModelBase/src/nodes/Node.h"

#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/VariableDeclaration.h"
#include "OOModel/src/expressions/Expression.h"
#include "OOModel/src/expressions/types/TypeExpression.h"
#include "OOModel/src/expressions/types/PrimitiveTypeExpression.h"
#include "OOModel/src/expressions/types/ArrayTypeExpression.h"
#include "OOModel/src/expressions/types/ClassTypeExpression.h"
#include "OOModel/src/expressions/VariableDeclarationExpression.h"
#include "OOModel/src/statements/DeclarationStatement.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/statements/LoopStatement.h"
#include "OOModel/src/elements/StatementItemList.h"
#include "OOModel/src/types/PrimitiveType.h"


namespace OODebug {

DebugUtils::DebugUtils(DebugConnector* connector) : debugConnector_{connector} {}

void DebugUtils::setExportMap(std::shared_ptr<Export::TextToNodeMap> map)
{
	exportMap_ = map;
}

QString DebugUtils::jvmSignatureFor(OOModel::Class* theClass)
{
	// from JNI spec fully qualified class: http://docs.oracle.com/javase/1.5.0/docs/guide/jni/spec/types.html#wp16432
	QString signature = fullNameFor(theClass, '/');
	signature.prepend("L").append(";");
	return signature;
}

QString DebugUtils::fullNameFor(OOModel::Class* theClass, QChar delimiter)
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

Location DebugUtils::nodeToLocation(Model::Node* node)
{
	auto method = node->firstAncestorOfType<OOModel::Method>();
	auto containerClass = method->firstAncestorOfType<OOModel::Class>();
	qint64 classId =  debugConnector_->classIdOf(jvmSignatureFor(containerClass));
	Q_ASSERT(classId != debugConnector_->NO_RESULT);
	QString methodName = method->name();
	if (method->methodKind() == OOModel::Method::MethodKind::Constructor)
		methodName = "<init>";
	// TODO: function to get signature of a method: for Java classes we would need the full java library.
	// Once fixed also fix the implementation of getMethodId().
	qint64 methodId = debugConnector_->methodIdOf(classId, methodName);
	Q_ASSERT(methodId != debugConnector_->NO_RESULT);

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
	auto lineTable = debugConnector_->lineTable(classId, methodId);
	for (auto val : lineTable.mappings())
		if (line == val.lineNumber()) methodIndex = val.lineCodeIndex();

	Q_ASSERT(methodIndex != NO_INDEX);
	return Location{tagKind, classId, methodId, methodIndex};
}

Model::Node* DebugUtils::locationToNode(Location location, bool& isClosingBracket)
{
	QString signature = debugConnector_->signatureOf(location.classId());
	signature = signature.mid(1, signature.size() - 2); // remove symbol at start and ; at end.
	QString fileName = QString{"src/%1.java"}.arg(signature);

	int line = -1;
	auto lineTable = debugConnector_->lineTable(location.classId(), location.methodId());
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
		if (auto stmtList = DCast<OOModel::StatementItemList>(node))
		{
			isClosingBracket = true;
			return stmtList->at(stmtList->size() -1);
		}
	}
	Q_ASSERT(false); // We should find a node!
}

double DebugUtils::doubleFromValue(Value v)
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

Protocol::Tag DebugUtils::typeOfVariable(OOModel::Method* containingMethod, VariableDetails variable)
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

Protocol::Tag DebugUtils::typeExpressionToTag(OOModel::Expression* e)
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

bool DebugUtils::hasPrimitiveValueType(OOModel::VariableDeclaration* variableDeclaration)
{
	Protocol::Tag tag = typeExpressionToTag(variableDeclaration->typeExpression());
	switch (tag)
	{
		case Protocol::Tag::INT: return true;
		case Protocol::Tag::LONG: return true;
		case Protocol::Tag::FLOAT: return true;
		case Protocol::Tag::DOUBLE: return true;
		case Protocol::Tag::SHORT: return true;
		default: break;
	}
	return false;
}

OOModel::VariableDeclaration* DebugUtils::variableDeclarationFromStatement(OOModel::StatementItem* statement,
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

QHash<QString, OOModel::VariableDeclaration*> DebugUtils::findVariableDetailsIn(
		OOModel::StatementItemList* statementList, QStringList variableNames, int indexFrom)
{
	QHash<QString, OOModel::VariableDeclaration*> declarationMap;
	int itemIndex = indexFrom;
	auto stmtList = statementList; // working copy

	if (!stmtList || variableNames.empty()) return {};

	while (stmtList)
	{
		for (int idx = itemIndex; idx >= 0; --idx)
		{
			for (auto varName : variableNames)
				if (!declarationMap.contains(varName))
					if (auto decl = variableDeclarationFromStatement(stmtList->at(idx), varName))
						 declarationMap[varName] = decl;
		}
		auto itemInParentList = stmtList->firstAncestorOfType<OOModel::StatementItem>();
		stmtList = nullptr; // we finished with this list
		if (itemInParentList && declarationMap.size() < variableNames.size())
		{
			// search in parent lists
			stmtList = itemInParentList->firstAncestorOfType<OOModel::StatementItemList>();
			itemIndex = 0;
			for (auto it : *stmtList)
			{
				if (it == itemInParentList) break;
				++itemIndex;
			}
		}
	}
	if (declarationMap.size() < variableNames.size())
	{
		// Try to look in the method arguments for the variable
		auto method = statementList->firstAncestorOfType<OOModel::Method>();
		Q_ASSERT(method); // SatementItemList outside method ??
		for (auto arg : *method->arguments())
			for (auto varName : variableNames)
				if (!declarationMap.contains(varName) && arg->name() == varName) declarationMap[varName] = arg;
	}
	return declarationMap;
}

}
