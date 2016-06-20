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

// GENERATED FILE: CHANGES WILL BE LOST!

#include "../AstApi.h"

#include "OOModel/src/expressions/Expression.h"
#include "OOModel/src/expressions/CommaExpression.h"
#include "OOModel/src/expressions/UnaryOperation.h"
#include "OOModel/src/expressions/MetaCallExpression.h"
#include "OOModel/src/expressions/UnfinishedOperator.h"
#include "OOModel/src/expressions/NullLiteral.h"
#include "OOModel/src/expressions/FloatLiteral.h"
#include "OOModel/src/expressions/TypeTraitExpression.h"
#include "OOModel/src/expressions/SuperExpression.h"
#include "OOModel/src/expressions/ConditionalExpression.h"
#include "OOModel/src/expressions/TypeNameOperator.h"
#include "OOModel/src/expressions/ThisExpression.h"
#include "OOModel/src/expressions/BinaryOperation.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/expressions/DeleteExpression.h"
#include "OOModel/src/expressions/LambdaExpression.h"
#include "OOModel/src/expressions/IntegerLiteral.h"
#include "OOModel/src/expressions/VariableDeclarationExpression.h"
#include "OOModel/src/expressions/StringLiteral.h"
#include "OOModel/src/expressions/InstanceOfExpression.h"
#include "OOModel/src/expressions/BooleanLiteral.h"
#include "OOModel/src/expressions/types/FunctionTypeExpression.h"
#include "OOModel/src/expressions/types/TypeQualifierExpression.h"
#include "OOModel/src/expressions/types/TypeExpression.h"
#include "OOModel/src/expressions/types/ArrayTypeExpression.h"
#include "OOModel/src/expressions/types/AutoTypeExpression.h"
#include "OOModel/src/expressions/types/ReferenceTypeExpression.h"
#include "OOModel/src/expressions/types/PrimitiveTypeExpression.h"
#include "OOModel/src/expressions/types/ClassTypeExpression.h"
#include "OOModel/src/expressions/types/PointerTypeExpression.h"
#include "OOModel/src/expressions/ReferenceExpression.h"
#include "OOModel/src/expressions/ArrayInitializer.h"
#include "OOModel/src/expressions/ThrowExpression.h"
#include "OOModel/src/expressions/ErrorExpression.h"
#include "OOModel/src/expressions/AssignmentExpression.h"
#include "OOModel/src/expressions/GlobalScopeExpression.h"
#include "OOModel/src/expressions/CharacterLiteral.h"
#include "OOModel/src/expressions/CastExpression.h"
#include "OOModel/src/expressions/NewExpression.h"
#include "OOModel/src/declarations/Project.h"
#include "OOModel/src/declarations/ExplicitTemplateInstantiation.h"
#include "OOModel/src/declarations/MetaBinding.h"
#include "OOModel/src/declarations/NameImport.h"
#include "OOModel/src/declarations/Field.h"
#include "OOModel/src/declarations/MetaCallMapping.h"
#include "OOModel/src/declarations/Declaration.h"
#include "OOModel/src/declarations/Method.h"
#include "OOModel/src/declarations/VariableDeclaration.h"
#include "OOModel/src/declarations/Module.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/MetaDefinition.h"
#include "OOModel/src/declarations/TypeAlias.h"
#include "OOModel/src/elements/MemberInitializer.h"
#include "OOModel/src/elements/Enumerator.h"
#include "OOModel/src/elements/Modifier.h"
#include "OOModel/src/elements/OOReference.h"
#include "OOModel/src/elements/CatchClause.h"
#include "OOModel/src/elements/FormalMetaArgument.h"
#include "OOModel/src/elements/FormalResult.h"
#include "OOModel/src/elements/CommentStatementItem.h"
#include "OOModel/src/elements/StatementItemList.h"
#include "OOModel/src/elements/FormalArgument.h"
#include "OOModel/src/elements/StatementItem.h"
#include "OOModel/src/elements/FormalTypeArgument.h"
#include "OOModel/src/statements/SynchronizedStatement.h"
#include "OOModel/src/statements/IfStatement.h"
#include "OOModel/src/statements/ReturnStatement.h"
#include "OOModel/src/statements/SwitchStatement.h"
#include "OOModel/src/statements/CaseStatement.h"
#include "OOModel/src/statements/BreakStatement.h"
#include "OOModel/src/statements/TryCatchFinallyStatement.h"
#include "OOModel/src/statements/DeclarationStatement.h"
#include "OOModel/src/statements/ForEachStatement.h"
#include "OOModel/src/statements/AssertStatement.h"
#include "OOModel/src/statements/Statement.h"
#include "OOModel/src/statements/ExpressionStatement.h"
#include "OOModel/src/statements/ContinueStatement.h"
#include "OOModel/src/statements/LoopStatement.h"
#include "OOModel/src/statements/Block.h"
#include "ModelBase/src/nodes/List.h"
#include "ModelBase/src/nodes/Boolean.h"
#include "ModelBase/src/nodes/Character.h"
#include "ModelBase/src/nodes/NameText.h"
#include "ModelBase/src/nodes/Float.h"
#include "ModelBase/src/nodes/Text.h"
#include "ModelBase/src/nodes/Reference.h"
#include "ModelBase/src/nodes/UsedLibrary.h"
#include "ModelBase/src/nodes/composite/CompositeNode.h"
#include "ModelBase/src/nodes/Integer.h"
#include "ModelBase/src/nodes/Node.h"

#include "ModelBase/src/persistence/ClipboardStore.h"

#include "ModelBase/src/commands/UndoCommand.h"

namespace InformationScripting {

using namespace boost::python;

void initClassNode() {
	Model::Node* (*Node_createNewNode1)(const QString&, Model::Node*,
		 Model::PersistentStore&, bool) = &Model::Node::createNewNode;
	Model::Node* (*Node_createNewNode2)(const QString&, Model::Node*) = &Model::Node::createNewNode;
	bool (Model::Node::*Node_isSubtypeOf1)(const QString&) const = &Model::Node::isSubtypeOf;
	bool (Model::Node::*Node_isSubtypeOf2)(int) const = &Model::Node::isSubtypeOf;
	auto aClass = class_<Model::Node, boost::noncopyable>("Node", no_init);
		aClass.add_property("parent",
			make_function(&Model::Node::parent, return_internal_reference<>()),
			&Model::Node::setParent);
		aClass.def("typeName", make_function((const QString& (
			Model::Node::*)())&Model::Node::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::Node::typeId);
		aClass.def("hierarchyTypeIds", &Model::Node::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&Model::Node::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::Node::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::Node::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::Node::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::Node::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("manager", make_function(&Model::Node::manager, return_internal_reference<>()));
		aClass.def("root", make_function(&Model::Node::root, return_internal_reference<>()));
		aClass.def("children", &Model::Node::children);
		aClass.def("definesSymbol", &Model::Node::definesSymbol);
		aClass.def("isTransparentForNameResolution", &Model::Node::isTransparentForNameResolution);
		aClass.def("symbolName", make_function((const QString& (
			Model::Node::*)())&Model::Node::symbolName, return_value_policy<copy_const_reference>()));
		aClass.def("symbolType", &Model::Node::symbolType);
		aClass.def("childrenInScope", &Model::Node::childrenInScope);
		aClass.def("symbolMatches", &Model::Node::symbolMatches);
		aClass.def("revision", &Model::Node::revision);
		aClass.def("incrementRevision", &Model::Node::incrementRevision);
		aClass.def("addToRevision", &Model::Node::addToRevision);
		aClass.def("isPartiallyLoaded", &Model::Node::isPartiallyLoaded);
		aClass.def("hasPartiallyLoadedChildren", &Model::Node::hasPartiallyLoadedChildren);
		aClass.def("accessLock", make_function(&Model::Node::accessLock, return_internal_reference<>()));
		aClass.def("lowestCommonAncestor", make_function(&Model::Node::lowestCommonAncestor, return_internal_reference<>()));
		aClass.def("isAncestorOf", &Model::Node::isAncestorOf);
		aClass.def("isSameOrAncestorOf", &Model::Node::isSameOrAncestorOf);
		aClass.def("childToSubnode", make_function(&Model::Node::childToSubnode, return_internal_reference<>()));
		aClass.def("execute", &Model::Node::execute);
		aClass.def("save", &Model::Node::save);
		aClass.def("load", &Model::Node::load);
		aClass.def("isNewPersistenceUnit", &Model::Node::isNewPersistenceUnit);
		aClass.def("persistentUnitNode", make_function(&Model::Node::persistentUnitNode, return_internal_reference<>()));
		aClass.def("isTypeRegistered", &Model::Node::isTypeRegistered);
		aClass.staticmethod("isTypeRegistered");
		aClass.def("isModifyable", &Model::Node::isModifyable);
		aClass.def("replaceChild", &Model::Node::replaceChild);
		aClass.def("beginModification", &Model::Node::beginModification);
		aClass.def("endModification", &Model::Node::endModification);
		aClass.def("usedLibraries", &Model::Node::usedLibraries);
		aClass.def("toDebugString", &Model::Node::toDebugString);
		aClass.def("createNewNode", make_function(Node_createNewNode1, return_internal_reference<>()));
		aClass.def("createNewNode", make_function(Node_createNewNode2, return_internal_reference<>()));
		aClass.def("isSubtypeOf", Node_isSubtypeOf1);
		aClass.def("isSubtypeOf", Node_isSubtypeOf2);
	scope NodeScope = aClass;
	enum_<Model::Node::SymbolType>("SymbolType")
		.value("UNSPECIFIED", Model::Node::SymbolType::UNSPECIFIED)
		.value("METHOD", Model::Node::SymbolType::METHOD)
		.value("CONTAINER", Model::Node::SymbolType::CONTAINER)
		.value("VARIABLE", Model::Node::SymbolType::VARIABLE)
		.value("ANY_SYMBOL", Model::Node::SymbolType::ANY_SYMBOL);
	enum_<Model::Node::FindSymbolDirection>("FindSymbolDirection")
		.value("SEARCH_HERE", Model::Node::FindSymbolDirection::SEARCH_HERE)
		.value("SEARCH_DOWN", Model::Node::FindSymbolDirection::SEARCH_DOWN)
		.value("SEARCH_UP", Model::Node::FindSymbolDirection::SEARCH_UP)
		.value("SEARCH_UP_ORDERED", Model::Node::FindSymbolDirection::SEARCH_UP_ORDERED);
}


} /* namespace InformationScripting */
