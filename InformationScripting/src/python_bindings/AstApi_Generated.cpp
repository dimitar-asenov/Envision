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

#include "AstApi.h"

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

void initClassCompositeNode() {
	Model::Node* (Model::CompositeNode::*CompositeNode_get1)(const QString&) const = &Model::CompositeNode::get;
	Model::Node* (Model::CompositeNode::*CompositeNode_get2)(const
		Model::CompositeIndex&) const = &Model::CompositeNode::get;
	Model::CompositeIndex (*CompositeNode_registerNewAttribute1)(const
		Model::Attribute&) = &Model::CompositeNode::registerNewAttribute;
	bool (Model::CompositeNode::*CompositeNode_isSubtypeOf1)(const QString&) const = &Model::CompositeNode::isSubtypeOf;
	bool (Model::CompositeNode::*CompositeNode_isSubtypeOf2)(int) const = &Model::CompositeNode::isSubtypeOf;
	Model::CompositeIndex (Model::CompositeNode::*CompositeNode_indexOf1)(
		const QString&) const = &Model::CompositeNode::indexOf;
	Model::CompositeIndex (Model::CompositeNode::*CompositeNode_indexOf2)(
		Model::Node*) const = &Model::CompositeNode::indexOf;
	void (Model::CompositeNode::*CompositeNode_remove1)(QString) = &Model::CompositeNode::remove;
	void (Model::CompositeNode::*CompositeNode_remove2)(Model::Node*) = &Model::CompositeNode::remove;
	void (Model::CompositeNode::*CompositeNode_remove3)(const Model::CompositeIndex&) = &Model::CompositeNode::remove;
	auto aClass = class_<Model::CompositeNode, bases<Model::Node>>("CompositeNode");
		aClass.add_property("comment",
			make_function(&Model::CompositeNode::comment, return_internal_reference<>()),
			&Model::CompositeNode::setComment);
		aClass.def("typeName", make_function((const QString& (
			Model::CompositeNode::*)())&Model::CompositeNode::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::CompositeNode::typeId);
		aClass.def("hierarchyTypeIds", &Model::CompositeNode::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&Model::CompositeNode::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::CompositeNode::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::CompositeNode::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::CompositeNode::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::CompositeNode::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("children", &Model::CompositeNode::children);
		aClass.def("replaceChild", &Model::CompositeNode::replaceChild);
		aClass.def("set", &Model::CompositeNode::set);
		aClass.def("save", &Model::CompositeNode::save);
		aClass.def("load", &Model::CompositeNode::load);
		aClass.def("hasAttribute", &Model::CompositeNode::hasAttribute);
		aClass.def("getAllAttributes", &Model::CompositeNode::getAllAttributes);
		aClass.def("getMetaData", make_function(&Model::CompositeNode::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("registerExtensionId", &Model::CompositeNode::registerExtensionId);
		aClass.staticmethod("registerExtensionId");
		aClass.def("meta", make_function((const Model::AttributeChain&
			(Model::CompositeNode::*)())&Model::CompositeNode::meta, return_value_policy<copy_const_reference>()));
		aClass.def("get", make_function(CompositeNode_get1, return_internal_reference<>()));
		aClass.def("get", make_function(CompositeNode_get2, return_internal_reference<>()));
		aClass.def("registerNewAttribute", CompositeNode_registerNewAttribute1);
		aClass.def("isSubtypeOf", CompositeNode_isSubtypeOf1);
		aClass.def("isSubtypeOf", CompositeNode_isSubtypeOf2);
		aClass.def("indexOf", CompositeNode_indexOf1);
		aClass.def("indexOf", CompositeNode_indexOf2);
		aClass.def("remove", CompositeNode_remove1);
		aClass.def("remove", CompositeNode_remove2);
		aClass.def("remove", CompositeNode_remove3);
}

void initClassModifier() {
	bool (OOModel::Modifier::*Modifier_isSubtypeOf1)(const QString&) const = &OOModel::Modifier::isSubtypeOf;
	bool (OOModel::Modifier::*Modifier_isSubtypeOf2)(int) const = &OOModel::Modifier::isSubtypeOf;
	auto aClass = class_<OOModel::Modifier, bases<Model::Node>>("Modifier");
		aClass.def("typeName", make_function((const QString& (
			OOModel::Modifier::*)())&OOModel::Modifier::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Modifier::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Modifier::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&OOModel::Modifier::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Modifier::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Modifier::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Modifier::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Modifier::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("get", &OOModel::Modifier::get);
		aClass.def("isSet", &OOModel::Modifier::isSet);
		aClass.def("set", &OOModel::Modifier::set);
		aClass.def("clear", &OOModel::Modifier::clear);
		aClass.def("save", &OOModel::Modifier::save);
		aClass.def("load", &OOModel::Modifier::load);
		aClass.def("isSubtypeOf", Modifier_isSubtypeOf1);
		aClass.def("isSubtypeOf", Modifier_isSubtypeOf2);
	scope ModifierScope = aClass;
	enum_<OOModel::Modifier::ModifierFlag>("ModifierFlag")
		.value("None", OOModel::Modifier::ModifierFlag::None)
		.value("Public", OOModel::Modifier::ModifierFlag::Public)
		.value("Private", OOModel::Modifier::ModifierFlag::Private)
		.value("Protected", OOModel::Modifier::ModifierFlag::Protected)
		.value("Static", OOModel::Modifier::ModifierFlag::Static)
		.value("Final", OOModel::Modifier::ModifierFlag::Final)
		.value("Abstract", OOModel::Modifier::ModifierFlag::Abstract)
		.value("Virtual", OOModel::Modifier::ModifierFlag::Virtual)
		.value("Override", OOModel::Modifier::ModifierFlag::Override)
		.value("Inline", OOModel::Modifier::ModifierFlag::Inline)
		.value("Const", OOModel::Modifier::ModifierFlag::Const)
		.value("ConstExpr", OOModel::Modifier::ModifierFlag::ConstExpr)
		.value("Default", OOModel::Modifier::ModifierFlag::Default)
		.value("Deleted", OOModel::Modifier::ModifierFlag::Deleted)
		.value("Explicit", OOModel::Modifier::ModifierFlag::Explicit);
}

void initClassReference() {
	bool (Model::Reference::*Reference_isSubtypeOf1)(const QString&) const = &Model::Reference::isSubtypeOf;
	bool (Model::Reference::*Reference_isSubtypeOf2)(int) const = &Model::Reference::isSubtypeOf;
	auto aClass = class_<Model::Reference, bases<Model::Node>>("Reference");
		aClass.add_property("name",
			make_function((const QString& (Model::Reference::*)(
				))&Model::Reference::name, return_value_policy<copy_const_reference>()),
			&Model::Reference::setName);
		aClass.def("typeName", make_function((const QString& (
			Model::Reference::*)())&Model::Reference::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::Reference::typeId);
		aClass.def("hierarchyTypeIds", &Model::Reference::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&Model::Reference::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::Reference::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::Reference::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::Reference::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::Reference::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("target", make_function(&Model::Reference::target, return_internal_reference<>()));
		aClass.def("save", &Model::Reference::save);
		aClass.def("load", &Model::Reference::load);
		aClass.def("resolve", &Model::Reference::resolve);
		aClass.def("isResolved", &Model::Reference::isResolved);
		aClass.def("computeTarget", make_function(&Model::Reference::computeTarget, return_internal_reference<>()));
		aClass.def("unresolveAll", &Model::Reference::unresolveAll);
		aClass.staticmethod("unresolveAll");
		aClass.def("unresolveNames", &Model::Reference::unresolveNames);
		aClass.staticmethod("unresolveNames");
		aClass.def("unresolveAfterNewSubTree", &Model::Reference::unresolveAfterNewSubTree);
		aClass.staticmethod("unresolveAfterNewSubTree");
		aClass.def("addUnresolutionSteps", &Model::Reference::addUnresolutionSteps);
		aClass.staticmethod("addUnresolutionSteps");
		aClass.def("unresolveIfNameIntroduced", &Model::Reference::unresolveIfNameIntroduced);
		aClass.staticmethod("unresolveIfNameIntroduced");
		aClass.def("resolvePending", &Model::Reference::resolvePending);
		aClass.staticmethod("resolvePending");
		aClass.def("isSubtypeOf", Reference_isSubtypeOf1);
		aClass.def("isSubtypeOf", Reference_isSubtypeOf2);
}

void initClassList() {
	bool (Model::List::*List_isSubtypeOf1)(const QString&) const = &Model::List::isSubtypeOf;
	bool (Model::List::*List_isSubtypeOf2)(int) const = &Model::List::isSubtypeOf;
	void (Model::List::*List_remove1)(Model::Node*) = &Model::List::remove;
	void (Model::List::*List_remove2)(int) = &Model::List::remove;
	auto aClass = class_<Model::List, bases<Model::Node>>("List");
		aClass.def("typeName", make_function((const QString& (
			Model::List::*)())&Model::List::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::List::typeId);
		aClass.def("hierarchyTypeIds", &Model::List::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&Model::List::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::List::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::List::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::List::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::List::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("save", &Model::List::save);
		aClass.def("load", &Model::List::load);
		aClass.def("children", &Model::List::children);
		aClass.def("size", &Model::List::size);
		aClass.def("isEmpty", &Model::List::isEmpty);
		aClass.def("indexOf", &Model::List::indexOf);
		aClass.def("indexToSubnode", &Model::List::indexToSubnode);
		aClass.def("contains", &Model::List::contains);
		aClass.def("append", &Model::List::append);
		aClass.def("prepend", &Model::List::prepend);
		aClass.def("insert", &Model::List::insert);
		aClass.def("paste", &Model::List::paste);
		aClass.def("clear", &Model::List::clear);
		aClass.def("nodes", make_function((const QVector<Model::Node*>&
			(Model::List::*)())&Model::List::nodes, return_value_policy<copy_const_reference>()));
		aClass.def("isTransparentForNameResolution", &Model::List::isTransparentForNameResolution);
		aClass.def("replaceChild", &Model::List::replaceChild);
		aClass.def("createDefaultElement", make_function(&Model::List::createDefaultElement, return_internal_reference<>()));
		aClass.def("lowerTypeBoundForElements", &Model::List::lowerTypeBoundForElements);
		aClass.def("isSubtypeOf", List_isSubtypeOf1);
		aClass.def("isSubtypeOf", List_isSubtypeOf2);
		aClass.def("remove", List_remove1);
		aClass.def("remove", List_remove2);
}

void initClassBoolean() {
	bool (Model::Boolean::*Boolean_isSubtypeOf1)(const QString&) const = &Model::Boolean::isSubtypeOf;
	bool (Model::Boolean::*Boolean_isSubtypeOf2)(int) const = &Model::Boolean::isSubtypeOf;
	auto aClass = class_<Model::Boolean, bases<Model::Node>>("Boolean");
		aClass.def("typeName", make_function((const QString& (
			Model::Boolean::*)())&Model::Boolean::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::Boolean::typeId);
		aClass.def("hierarchyTypeIds", &Model::Boolean::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&Model::Boolean::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::Boolean::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::Boolean::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::Boolean::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::Boolean::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("get", &Model::Boolean::get);
		aClass.def("set", &Model::Boolean::set);
		aClass.def("save", &Model::Boolean::save);
		aClass.def("load", &Model::Boolean::load);
		aClass.def("isSubtypeOf", Boolean_isSubtypeOf1);
		aClass.def("isSubtypeOf", Boolean_isSubtypeOf2);
}

void initClassCharacter() {
	bool (Model::Character::*Character_isSubtypeOf1)(const QString&) const = &Model::Character::isSubtypeOf;
	bool (Model::Character::*Character_isSubtypeOf2)(int) const = &Model::Character::isSubtypeOf;
	auto aClass = class_<Model::Character, bases<Model::Node>>("Character");
		aClass.def("typeName", make_function((const QString& (
			Model::Character::*)())&Model::Character::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::Character::typeId);
		aClass.def("hierarchyTypeIds", &Model::Character::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&Model::Character::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::Character::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::Character::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::Character::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::Character::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("get", &Model::Character::get);
		aClass.def("set", &Model::Character::set);
		aClass.def("save", &Model::Character::save);
		aClass.def("load", &Model::Character::load);
		aClass.def("isSubtypeOf", Character_isSubtypeOf1);
		aClass.def("isSubtypeOf", Character_isSubtypeOf2);
}

void initClassText() {
	bool (Model::Text::*Text_isSubtypeOf1)(const QString&) const = &Model::Text::isSubtypeOf;
	bool (Model::Text::*Text_isSubtypeOf2)(int) const = &Model::Text::isSubtypeOf;
	auto aClass = class_<Model::Text, bases<Model::Node>>("Text");
		aClass.def("typeName", make_function((const QString& (
			Model::Text::*)())&Model::Text::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::Text::typeId);
		aClass.def("hierarchyTypeIds", &Model::Text::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&Model::Text::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::Text::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::Text::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::Text::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::Text::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("get", make_function((const QString& (Model::Text::*)(
			))&Model::Text::get, return_value_policy<copy_const_reference>()));
		aClass.def("operator const QString &", make_function((const
			QString& (Model::Text::*)())&Model::Text::operator const QString &, return_value_policy<copy_const_reference>()));
		aClass.def("set", &Model::Text::set);
		aClass.def("save", &Model::Text::save);
		aClass.def("load", &Model::Text::load);
		aClass.def("isSubtypeOf", Text_isSubtypeOf1);
		aClass.def("isSubtypeOf", Text_isSubtypeOf2);
}

void initClassFloat() {
	bool (Model::Float::*Float_isSubtypeOf1)(const QString&) const = &Model::Float::isSubtypeOf;
	bool (Model::Float::*Float_isSubtypeOf2)(int) const = &Model::Float::isSubtypeOf;
	auto aClass = class_<Model::Float, bases<Model::Node>>("Float");
		aClass.def("typeName", make_function((const QString& (
			Model::Float::*)())&Model::Float::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::Float::typeId);
		aClass.def("hierarchyTypeIds", &Model::Float::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&Model::Float::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::Float::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::Float::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::Float::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::Float::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("get", &Model::Float::get);
		aClass.def("set", &Model::Float::set);
		aClass.def("save", &Model::Float::save);
		aClass.def("load", &Model::Float::load);
		aClass.def("isSubtypeOf", Float_isSubtypeOf1);
		aClass.def("isSubtypeOf", Float_isSubtypeOf2);
}

void initClassInteger() {
	bool (Model::Integer::*Integer_isSubtypeOf1)(const QString&) const = &Model::Integer::isSubtypeOf;
	bool (Model::Integer::*Integer_isSubtypeOf2)(int) const = &Model::Integer::isSubtypeOf;
	auto aClass = class_<Model::Integer, bases<Model::Node>>("Integer");
		aClass.def("typeName", make_function((const QString& (
			Model::Integer::*)())&Model::Integer::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::Integer::typeId);
		aClass.def("hierarchyTypeIds", &Model::Integer::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&Model::Integer::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::Integer::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::Integer::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::Integer::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::Integer::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("get", &Model::Integer::get);
		aClass.def("set", &Model::Integer::set);
		aClass.def("save", &Model::Integer::save);
		aClass.def("load", &Model::Integer::load);
		aClass.def("isSubtypeOf", Integer_isSubtypeOf1);
		aClass.def("isSubtypeOf", Integer_isSubtypeOf2);
}

void initClassExpression() {
	Model::CompositeIndex (*Expression_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::Expression::registerNewAttribute;
	Model::CompositeIndex (*Expression_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Expression::registerNewAttribute;
	bool (OOModel::Expression::*Expression_isSubtypeOf1)(const QString&) const = &OOModel::Expression::isSubtypeOf;
	bool (OOModel::Expression::*Expression_isSubtypeOf2)(int) const = &OOModel::Expression::isSubtypeOf;
	auto aClass = class_<OOModel::Expression, bases<Model::CompositeNode>>("Expression");
		aClass.def("typeName", make_function((const QString& (
			OOModel::Expression::*)())&OOModel::Expression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Expression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Expression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::Expression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Expression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Expression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Expression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Expression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::Expression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::Expression::type);
		aClass.def("topMostExpressionParent", make_function(
			&OOModel::Expression::topMostExpressionParent, return_internal_reference<>()));
		aClass.def("registerNewAttribute", Expression_registerNewAttribute1);
		aClass.def("registerNewAttribute", Expression_registerNewAttribute2);
		aClass.def("isSubtypeOf", Expression_isSubtypeOf1);
		aClass.def("isSubtypeOf", Expression_isSubtypeOf2);
}

void initClassDeclaration() {
	Model::CompositeIndex (*Declaration_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::Declaration::registerNewAttribute;
	Model::CompositeIndex (*Declaration_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::Declaration::registerNewAttribute;
	bool (OOModel::Declaration::*Declaration_isSubtypeOf1)(const QString&) const = &OOModel::Declaration::isSubtypeOf;
	bool (OOModel::Declaration::*Declaration_isSubtypeOf2)(int) const = &OOModel::Declaration::isSubtypeOf;
	auto aClass = class_<OOModel::Declaration, bases<Model::CompositeNode>>("Declaration");
		aClass.add_property("name",
			make_function((const QString& (OOModel::Declaration::*)(
				))&OOModel::Declaration::name, return_value_policy<copy_const_reference>()),
			&OOModel::Declaration::setName);
		aClass.add_property("annotations",
			make_function(&OOModel::Declaration::annotations, return_internal_reference<>()),
			&OOModel::Declaration::setAnnotations);
		aClass.add_property("modifiers",
			make_function(&OOModel::Declaration::modifiers, return_internal_reference<>()),
			&OOModel::Declaration::setModifiers);
		aClass.add_property("subDeclarations",
			make_function(&OOModel::Declaration::subDeclarations, return_internal_reference<>()),
			&OOModel::Declaration::setSubDeclarations);
		aClass.add_property("metaCalls",
			make_function(&OOModel::Declaration::metaCalls, return_internal_reference<>()),
			&OOModel::Declaration::setMetaCalls);
		aClass.def("typeName", make_function((const QString& (
			OOModel::Declaration::*)())&OOModel::Declaration::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Declaration::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Declaration::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::Declaration::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Declaration::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Declaration::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Declaration::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Declaration::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::Declaration::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("nameNode", make_function(&OOModel::Declaration::nameNode, return_internal_reference<>()));
		aClass.def("definesSymbol", &OOModel::Declaration::definesSymbol);
		aClass.def("symbolName", make_function((const QString& (
			OOModel::Declaration::*)())&OOModel::Declaration::symbolName, return_value_policy<copy_const_reference>()));
		aClass.def("symbolType", &OOModel::Declaration::symbolType);
		aClass.def("registerNewAttribute", Declaration_registerNewAttribute1);
		aClass.def("registerNewAttribute", Declaration_registerNewAttribute2);
		aClass.def("isSubtypeOf", Declaration_isSubtypeOf1);
		aClass.def("isSubtypeOf", Declaration_isSubtypeOf2);
}

void initClassMemberInitializer() {
	bool (OOModel::MemberInitializer::*MemberInitializer_isSubtypeOf1)(
		const QString&) const = &OOModel::MemberInitializer::isSubtypeOf;
	bool (OOModel::MemberInitializer::*MemberInitializer_isSubtypeOf2)(
		int) const = &OOModel::MemberInitializer::isSubtypeOf;
	Model::CompositeIndex (*MemberInitializer_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MemberInitializer::registerNewAttribute;
	Model::CompositeIndex (*MemberInitializer_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MemberInitializer::registerNewAttribute;
	auto aClass = class_<OOModel::MemberInitializer, bases<Model::CompositeNode>>("MemberInitializer");
		aClass.add_property("arguments",
			make_function(&OOModel::MemberInitializer::arguments, return_internal_reference<>()),
			&OOModel::MemberInitializer::setArguments);
		aClass.add_property("memberReference",
			make_function(&OOModel::MemberInitializer::memberReference, return_internal_reference<>()),
			&OOModel::MemberInitializer::setMemberReference);
		aClass.def("typeName", make_function((const QString& (
		OOModel::MemberInitializer::*)())&OOModel::MemberInitializer::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::MemberInitializer::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::MemberInitializer::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::MemberInitializer::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::MemberInitializer::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::MemberInitializer::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::MemberInitializer::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::MemberInitializer::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::MemberInitializer::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", MemberInitializer_isSubtypeOf1);
		aClass.def("isSubtypeOf", MemberInitializer_isSubtypeOf2);
		aClass.def("registerNewAttribute", MemberInitializer_registerNewAttribute1);
		aClass.def("registerNewAttribute", MemberInitializer_registerNewAttribute2);
}

void initClassEnumerator() {
	Model::CompositeIndex (*Enumerator_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::Enumerator::registerNewAttribute;
	Model::CompositeIndex (*Enumerator_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Enumerator::registerNewAttribute;
	bool (OOModel::Enumerator::*Enumerator_isSubtypeOf1)(const QString&) const = &OOModel::Enumerator::isSubtypeOf;
	bool (OOModel::Enumerator::*Enumerator_isSubtypeOf2)(int) const = &OOModel::Enumerator::isSubtypeOf;
	auto aClass = class_<OOModel::Enumerator, bases<Model::CompositeNode>>("Enumerator");
		aClass.add_property("name",
			make_function((const QString& (OOModel::Enumerator::*)(
				))&OOModel::Enumerator::name, return_value_policy<copy_const_reference>()),
			&OOModel::Enumerator::setName);
		aClass.add_property("value",
			make_function(&OOModel::Enumerator::value, return_internal_reference<>()),
			&OOModel::Enumerator::setValue);
		aClass.def("typeName", make_function((const QString& (
			OOModel::Enumerator::*)())&OOModel::Enumerator::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Enumerator::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Enumerator::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::Enumerator::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Enumerator::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Enumerator::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Enumerator::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Enumerator::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::Enumerator::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("nameNode", make_function(&OOModel::Enumerator::nameNode, return_internal_reference<>()));
		aClass.def("definesSymbol", &OOModel::Enumerator::definesSymbol);
		aClass.def("symbolName", make_function((const QString& (
			OOModel::Enumerator::*)())&OOModel::Enumerator::symbolName, return_value_policy<copy_const_reference>()));
		aClass.def("symbolType", &OOModel::Enumerator::symbolType);
		aClass.def("registerNewAttribute", Enumerator_registerNewAttribute1);
		aClass.def("registerNewAttribute", Enumerator_registerNewAttribute2);
		aClass.def("isSubtypeOf", Enumerator_isSubtypeOf1);
		aClass.def("isSubtypeOf", Enumerator_isSubtypeOf2);
}

void initClassCatchClause() {
	bool (OOModel::CatchClause::*CatchClause_isSubtypeOf1)(const QString&) const = &OOModel::CatchClause::isSubtypeOf;
	bool (OOModel::CatchClause::*CatchClause_isSubtypeOf2)(int) const = &OOModel::CatchClause::isSubtypeOf;
	Model::CompositeIndex (*CatchClause_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::CatchClause::registerNewAttribute;
	Model::CompositeIndex (*CatchClause_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::CatchClause::registerNewAttribute;
	auto aClass = class_<OOModel::CatchClause, bases<Model::CompositeNode>>("CatchClause");
		aClass.add_property("exceptionToCatch",
			make_function(&OOModel::CatchClause::exceptionToCatch, return_internal_reference<>()),
			&OOModel::CatchClause::setExceptionToCatch);
		aClass.add_property("body",
			make_function(&OOModel::CatchClause::body, return_internal_reference<>()),
			&OOModel::CatchClause::setBody);
		aClass.def("typeName", make_function((const QString& (
			OOModel::CatchClause::*)())&OOModel::CatchClause::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::CatchClause::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::CatchClause::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::CatchClause::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::CatchClause::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::CatchClause::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::CatchClause::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::CatchClause::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::CatchClause::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", CatchClause_isSubtypeOf1);
		aClass.def("isSubtypeOf", CatchClause_isSubtypeOf2);
		aClass.def("registerNewAttribute", CatchClause_registerNewAttribute1);
		aClass.def("registerNewAttribute", CatchClause_registerNewAttribute2);
}

void initClassFormalResult() {
	Model::CompositeIndex (*FormalResult_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FormalResult::registerNewAttribute;
	Model::CompositeIndex (*FormalResult_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FormalResult::registerNewAttribute;
	bool (OOModel::FormalResult::*FormalResult_isSubtypeOf1)(const QString&) const = &OOModel::FormalResult::isSubtypeOf;
	bool (OOModel::FormalResult::*FormalResult_isSubtypeOf2)(int) const = &OOModel::FormalResult::isSubtypeOf;
	auto aClass = class_<OOModel::FormalResult, bases<Model::CompositeNode>>("FormalResult");
		aClass.add_property("name",
			make_function((const QString& (OOModel::FormalResult::*)(
				))&OOModel::FormalResult::name, return_value_policy<copy_const_reference>()),
			&OOModel::FormalResult::setName);
		aClass.add_property("typeExpression",
			make_function(&OOModel::FormalResult::typeExpression, return_internal_reference<>()),
			&OOModel::FormalResult::setTypeExpression);
		aClass.def("typeName", make_function((const QString& (
			OOModel::FormalResult::*)())&OOModel::FormalResult::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::FormalResult::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::FormalResult::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::FormalResult::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::FormalResult::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::FormalResult::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::FormalResult::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::FormalResult::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::FormalResult::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("nameNode", make_function(&OOModel::FormalResult::nameNode, return_internal_reference<>()));
		aClass.def("definesSymbol", &OOModel::FormalResult::definesSymbol);
		aClass.def("symbolName", make_function((const QString& (
			OOModel::FormalResult::*)())&OOModel::FormalResult::symbolName, return_value_policy<copy_const_reference>()));
		aClass.def("symbolType", &OOModel::FormalResult::symbolType);
		aClass.def("registerNewAttribute", FormalResult_registerNewAttribute1);
		aClass.def("registerNewAttribute", FormalResult_registerNewAttribute2);
		aClass.def("isSubtypeOf", FormalResult_isSubtypeOf1);
		aClass.def("isSubtypeOf", FormalResult_isSubtypeOf2);
}

void initClassStatementItem() {
	bool (OOModel::StatementItem::*StatementItem_isSubtypeOf1)(
		const QString&) const = &OOModel::StatementItem::isSubtypeOf;
	bool (OOModel::StatementItem::*StatementItem_isSubtypeOf2)(int) const = &OOModel::StatementItem::isSubtypeOf;
	Model::CompositeIndex (*StatementItem_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::StatementItem::registerNewAttribute;
	Model::CompositeIndex (*StatementItem_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::StatementItem::registerNewAttribute;
	auto aClass = class_<OOModel::StatementItem, bases<Model::CompositeNode>>("StatementItem");
		aClass.def("typeName", make_function((const QString& (
			OOModel::StatementItem::*)())&OOModel::StatementItem::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::StatementItem::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::StatementItem::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::StatementItem::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::StatementItem::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::StatementItem::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::StatementItem::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::StatementItem::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::StatementItem::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", StatementItem_isSubtypeOf1);
		aClass.def("isSubtypeOf", StatementItem_isSubtypeOf2);
		aClass.def("registerNewAttribute", StatementItem_registerNewAttribute1);
		aClass.def("registerNewAttribute", StatementItem_registerNewAttribute2);
}

void initClassFormalTypeArgument() {
	Model::CompositeIndex (*FormalTypeArgument_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FormalTypeArgument::registerNewAttribute;
	Model::CompositeIndex (*FormalTypeArgument_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FormalTypeArgument::registerNewAttribute;
	bool (OOModel::FormalTypeArgument::*FormalTypeArgument_isSubtypeOf1)(
		const QString&) const = &OOModel::FormalTypeArgument::isSubtypeOf;
	bool (OOModel::FormalTypeArgument::*FormalTypeArgument_isSubtypeOf2)(
		int) const = &OOModel::FormalTypeArgument::isSubtypeOf;
	auto aClass = class_<OOModel::FormalTypeArgument, bases<Model::CompositeNode>>("FormalTypeArgument");
		aClass.add_property("name",
			make_function((const QString& (OOModel::FormalTypeArgument::*)(
				))&OOModel::FormalTypeArgument::name, return_value_policy<copy_const_reference>()),
			&OOModel::FormalTypeArgument::setName);
		aClass.add_property("subTypeOfExpression",
			make_function(&OOModel::FormalTypeArgument::subTypeOfExpression, return_internal_reference<>()),
			&OOModel::FormalTypeArgument::setSubTypeOfExpression);
		aClass.add_property("superTypeOfExpression",
			make_function(&OOModel::FormalTypeArgument::superTypeOfExpression, return_internal_reference<>()),
			&OOModel::FormalTypeArgument::setSuperTypeOfExpression);
		aClass.add_property("specializationExpression",
			make_function(&OOModel::FormalTypeArgument::specializationExpression, return_internal_reference<>()),
			&OOModel::FormalTypeArgument::setSpecializationExpression);
		aClass.add_property("defaultType",
			make_function(&OOModel::FormalTypeArgument::defaultType, return_internal_reference<>()),
			&OOModel::FormalTypeArgument::setDefaultType);
		aClass.def("typeName", make_function((const QString& (
		OOModel::FormalTypeArgument::*)())&OOModel::FormalTypeArgument::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::FormalTypeArgument::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::FormalTypeArgument::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::FormalTypeArgument::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::FormalTypeArgument::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::FormalTypeArgument::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::FormalTypeArgument::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::FormalTypeArgument::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::FormalTypeArgument::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("nameNode", make_function(&OOModel::FormalTypeArgument::nameNode, return_internal_reference<>()));
		aClass.def("definesSymbol", &OOModel::FormalTypeArgument::definesSymbol);
		aClass.def("symbolName", make_function((const QString& (
		OOModel::FormalTypeArgument::*)())&OOModel::FormalTypeArgument::symbolName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("symbolType", &OOModel::FormalTypeArgument::symbolType);
		aClass.def("registerNewAttribute", FormalTypeArgument_registerNewAttribute1);
		aClass.def("registerNewAttribute", FormalTypeArgument_registerNewAttribute2);
		aClass.def("isSubtypeOf", FormalTypeArgument_isSubtypeOf1);
		aClass.def("isSubtypeOf", FormalTypeArgument_isSubtypeOf2);
}

void initClassUsedLibrary() {
	Model::CompositeIndex (*UsedLibrary_registerNewAttribute1)(
		const Model::Attribute&) = &Model::UsedLibrary::registerNewAttribute;
	Model::CompositeIndex (*UsedLibrary_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &Model::UsedLibrary::registerNewAttribute;
	bool (Model::UsedLibrary::*UsedLibrary_isSubtypeOf1)(const QString&) const = &Model::UsedLibrary::isSubtypeOf;
	bool (Model::UsedLibrary::*UsedLibrary_isSubtypeOf2)(int) const = &Model::UsedLibrary::isSubtypeOf;
	auto aClass = class_<Model::UsedLibrary, bases<Model::CompositeNode>>("UsedLibrary");
		aClass.add_property("name",
			make_function((const QString& (Model::UsedLibrary::*)(
				))&Model::UsedLibrary::name, return_value_policy<copy_const_reference>()),
			&Model::UsedLibrary::setName);
		aClass.def("typeName", make_function((const QString& (
			Model::UsedLibrary::*)())&Model::UsedLibrary::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::UsedLibrary::typeId);
		aClass.def("hierarchyTypeIds", &Model::UsedLibrary::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&Model::UsedLibrary::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::UsedLibrary::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::UsedLibrary::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::UsedLibrary::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::UsedLibrary::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&Model::UsedLibrary::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("nameNode", make_function(&Model::UsedLibrary::nameNode, return_internal_reference<>()));
		aClass.def("usedLibraries", &Model::UsedLibrary::usedLibraries);
		aClass.def("libraryRoot", make_function(&Model::UsedLibrary::libraryRoot, return_internal_reference<>()));
		aClass.def("libraryManager", make_function(&Model::UsedLibrary::libraryManager, return_internal_reference<>()));
		aClass.def("loadLibrary", &Model::UsedLibrary::loadLibrary);
		aClass.def("registerNewAttribute", UsedLibrary_registerNewAttribute1);
		aClass.def("registerNewAttribute", UsedLibrary_registerNewAttribute2);
		aClass.def("isSubtypeOf", UsedLibrary_isSubtypeOf1);
		aClass.def("isSubtypeOf", UsedLibrary_isSubtypeOf2);
}

void initClassOOReference() {
	bool (OOModel::OOReference::*OOReference_isSubtypeOf1)(const QString&) const = &OOModel::OOReference::isSubtypeOf;
	bool (OOModel::OOReference::*OOReference_isSubtypeOf2)(int) const = &OOModel::OOReference::isSubtypeOf;
	auto aClass = class_<OOModel::OOReference, bases<Model::Reference>>("OOReference");
		aClass.def("typeName", make_function((const QString& (
			OOModel::OOReference::*)())&OOModel::OOReference::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::OOReference::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::OOReference::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::OOReference::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::OOReference::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::OOReference::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::OOReference::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::OOReference::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("computeTarget", make_function(&OOModel::OOReference::computeTarget, return_internal_reference<>()));
		aClass.def("unresolveOOReferencesAfterSubTree", &OOModel::OOReference::unresolveOOReferencesAfterSubTree);
		aClass.staticmethod("unresolveOOReferencesAfterSubTree");
		aClass.def("isSubtypeOf", OOReference_isSubtypeOf1);
		aClass.def("isSubtypeOf", OOReference_isSubtypeOf2);
}

void initClassTypedListOfStatementItem() {
	using TypedListOfStatementItem = Model::TypedList<OOModel::StatementItem>;
	bool (Model::TypedList<OOModel::StatementItem>::*TypedListOfStatementItem_isSubtypeOf1)(
		const QString&) const = &TypedListOfStatementItem::isSubtypeOf;
	bool (Model::TypedList<OOModel::StatementItem>::*TypedListOfStatementItem_isSubtypeOf2)(
		int) const = &TypedListOfStatementItem::isSubtypeOf;
	auto aClass = class_<Model::TypedList<OOModel::StatementItem>, bases<Model::List>>("TypedListOfStatementItem");
		aClass.def("typeName", make_function((const QString& (
			TypedListOfStatementItem::*)())&TypedListOfStatementItem::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &TypedListOfStatementItem::typeId);
		aClass.def("hierarchyTypeIds", &TypedListOfStatementItem::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&TypedListOfStatementItem::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &TypedListOfStatementItem::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &TypedListOfStatementItem::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&TypedListOfStatementItem::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&TypedListOfStatementItem::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("first", make_function(&TypedListOfStatementItem::first, return_internal_reference<>()));
		aClass.def("last", make_function(&TypedListOfStatementItem::last, return_internal_reference<>()));
		aClass.def("at", make_function(&TypedListOfStatementItem::at, return_internal_reference<>()));
		aClass.def("replaceChild", &TypedListOfStatementItem::replaceChild);
		aClass.def("createDefaultElement", make_function(
			&TypedListOfStatementItem::createDefaultElement, return_internal_reference<>()));
		aClass.def("lowerTypeBoundForElements", &TypedListOfStatementItem::lowerTypeBoundForElements);
		aClass.def("isSubtypeOf", TypedListOfStatementItem_isSubtypeOf1);
		aClass.def("isSubtypeOf", TypedListOfStatementItem_isSubtypeOf2);
}

void initClassNameText() {
	bool (Model::NameText::*NameText_isSubtypeOf1)(const QString&) const = &Model::NameText::isSubtypeOf;
	bool (Model::NameText::*NameText_isSubtypeOf2)(int) const = &Model::NameText::isSubtypeOf;
	auto aClass = class_<Model::NameText, bases<Model::Text>>("NameText");
		aClass.def("typeName", make_function((const QString& (
			Model::NameText::*)())&Model::NameText::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &Model::NameText::typeId);
		aClass.def("hierarchyTypeIds", &Model::NameText::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&Model::NameText::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &Model::NameText::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &Model::NameText::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&Model::NameText::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&Model::NameText::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("isSubtypeOf", NameText_isSubtypeOf1);
		aClass.def("isSubtypeOf", NameText_isSubtypeOf2);
}

void initClassCommaExpression() {
	Model::CompositeIndex (*CommaExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::CommaExpression::registerNewAttribute;
	Model::CompositeIndex (*CommaExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::CommaExpression::registerNewAttribute;
	bool (OOModel::CommaExpression::*CommaExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::CommaExpression::isSubtypeOf;
	bool (OOModel::CommaExpression::*CommaExpression_isSubtypeOf2)(int) const = &OOModel::CommaExpression::isSubtypeOf;
	auto aClass = class_<OOModel::CommaExpression, bases<OOModel::Expression>>("CommaExpression");
		aClass.add_property("left",
			make_function(&OOModel::CommaExpression::left, return_internal_reference<>()),
			&OOModel::CommaExpression::setLeft);
		aClass.add_property("right",
			make_function(&OOModel::CommaExpression::right, return_internal_reference<>()),
			&OOModel::CommaExpression::setRight);
		aClass.def("typeName", make_function((const QString& (
			OOModel::CommaExpression::*)())&OOModel::CommaExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::CommaExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::CommaExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::CommaExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::CommaExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::CommaExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::CommaExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::CommaExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::CommaExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("allSubOperands", &OOModel::CommaExpression::allSubOperands);
		aClass.def("type", &OOModel::CommaExpression::type);
		aClass.def("registerNewAttribute", CommaExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", CommaExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", CommaExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", CommaExpression_isSubtypeOf2);
}

void initClassUnaryOperation() {
	Model::CompositeIndex (*UnaryOperation_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::UnaryOperation::registerNewAttribute;
	Model::CompositeIndex (*UnaryOperation_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::UnaryOperation::registerNewAttribute;
	bool (OOModel::UnaryOperation::*UnaryOperation_isSubtypeOf1)(const
		QString&) const = &OOModel::UnaryOperation::isSubtypeOf;
	bool (OOModel::UnaryOperation::*UnaryOperation_isSubtypeOf2)(int) const = &OOModel::UnaryOperation::isSubtypeOf;
	auto aClass = class_<OOModel::UnaryOperation, bases<OOModel::Expression>>("UnaryOperation");
		aClass.add_property("operand",
			make_function(&OOModel::UnaryOperation::operand, return_internal_reference<>()),
			&OOModel::UnaryOperation::setOperand);
		aClass.add_property("op",
			&OOModel::UnaryOperation::op,
			&OOModel::UnaryOperation::setOp);
		aClass.def("typeName", make_function((const QString& (
			OOModel::UnaryOperation::*)())&OOModel::UnaryOperation::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::UnaryOperation::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::UnaryOperation::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::UnaryOperation::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::UnaryOperation::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::UnaryOperation::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::UnaryOperation::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::UnaryOperation::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::UnaryOperation::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::UnaryOperation::type);
		aClass.def("registerNewAttribute", UnaryOperation_registerNewAttribute1);
		aClass.def("registerNewAttribute", UnaryOperation_registerNewAttribute2);
		aClass.def("isSubtypeOf", UnaryOperation_isSubtypeOf1);
		aClass.def("isSubtypeOf", UnaryOperation_isSubtypeOf2);
	scope UnaryOperationScope = aClass;
	enum_<OOModel::UnaryOperation::OperatorTypes>("OperatorTypes")
		.value("PREINCREMENT", OOModel::UnaryOperation::OperatorTypes::PREINCREMENT)
		.value("PREDECREMENT", OOModel::UnaryOperation::OperatorTypes::PREDECREMENT)
		.value("POSTINCREMENT", OOModel::UnaryOperation::OperatorTypes::POSTINCREMENT)
		.value("POSTDECREMENT", OOModel::UnaryOperation::OperatorTypes::POSTDECREMENT)
		.value("PLUS", OOModel::UnaryOperation::OperatorTypes::PLUS)
		.value("MINUS", OOModel::UnaryOperation::OperatorTypes::MINUS)
		.value("NOT", OOModel::UnaryOperation::OperatorTypes::NOT)
		.value("COMPLEMENT", OOModel::UnaryOperation::OperatorTypes::COMPLEMENT)
		.value("PARENTHESIS", OOModel::UnaryOperation::OperatorTypes::PARENTHESIS)
		.value("DEREFERENCE", OOModel::UnaryOperation::OperatorTypes::DEREFERENCE)
		.value("ADDRESSOF", OOModel::UnaryOperation::OperatorTypes::ADDRESSOF);
}

void initClassMetaCallExpression() {
	Model::CompositeIndex (*MetaCallExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MetaCallExpression::registerNewAttribute;
	Model::CompositeIndex (*MetaCallExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MetaCallExpression::registerNewAttribute;
	bool (OOModel::MetaCallExpression::*MetaCallExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::MetaCallExpression::isSubtypeOf;
	bool (OOModel::MetaCallExpression::*MetaCallExpression_isSubtypeOf2)(
		int) const = &OOModel::MetaCallExpression::isSubtypeOf;
	auto aClass = class_<OOModel::MetaCallExpression, bases<OOModel::Expression>>("MetaCallExpression");
		aClass.add_property("callee",
			make_function(&OOModel::MetaCallExpression::callee, return_internal_reference<>()),
			&OOModel::MetaCallExpression::setCallee);
		aClass.add_property("arguments",
			make_function(&OOModel::MetaCallExpression::arguments, return_internal_reference<>()),
			&OOModel::MetaCallExpression::setArguments);
		aClass.def("typeName", make_function((const QString& (
		OOModel::MetaCallExpression::*)())&OOModel::MetaCallExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::MetaCallExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::MetaCallExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::MetaCallExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::MetaCallExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::MetaCallExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::MetaCallExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::MetaCallExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::MetaCallExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("metaDefinition", make_function(
			&OOModel::MetaCallExpression::metaDefinition, return_internal_reference<>()));
		aClass.def("generatedTree", make_function(
			&OOModel::MetaCallExpression::generatedTree, return_internal_reference<>()));
		aClass.def("registerNewAttribute", MetaCallExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", MetaCallExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", MetaCallExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", MetaCallExpression_isSubtypeOf2);
}

void initClassUnfinishedOperator() {
	bool (OOModel::UnfinishedOperator::*UnfinishedOperator_isSubtypeOf1)(
		const QString&) const = &OOModel::UnfinishedOperator::isSubtypeOf;
	bool (OOModel::UnfinishedOperator::*UnfinishedOperator_isSubtypeOf2)(
		int) const = &OOModel::UnfinishedOperator::isSubtypeOf;
	Model::CompositeIndex (*UnfinishedOperator_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::UnfinishedOperator::registerNewAttribute;
	Model::CompositeIndex (*UnfinishedOperator_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::UnfinishedOperator::registerNewAttribute;
	auto aClass = class_<OOModel::UnfinishedOperator, bases<OOModel::Expression>>("UnfinishedOperator");
		aClass.add_property("delimiters",
			make_function(&OOModel::UnfinishedOperator::delimiters, return_internal_reference<>()),
			&OOModel::UnfinishedOperator::setDelimiters);
		aClass.add_property("operands",
			make_function(&OOModel::UnfinishedOperator::operands, return_internal_reference<>()),
			&OOModel::UnfinishedOperator::setOperands);
		aClass.def("typeName", make_function((const QString& (
		OOModel::UnfinishedOperator::*)())&OOModel::UnfinishedOperator::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::UnfinishedOperator::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::UnfinishedOperator::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::UnfinishedOperator::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::UnfinishedOperator::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::UnfinishedOperator::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::UnfinishedOperator::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::UnfinishedOperator::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::UnfinishedOperator::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::UnfinishedOperator::type);
		aClass.def("isSubtypeOf", UnfinishedOperator_isSubtypeOf1);
		aClass.def("isSubtypeOf", UnfinishedOperator_isSubtypeOf2);
		aClass.def("registerNewAttribute", UnfinishedOperator_registerNewAttribute1);
		aClass.def("registerNewAttribute", UnfinishedOperator_registerNewAttribute2);
}

void initClassNullLiteral() {
	bool (OOModel::NullLiteral::*NullLiteral_isSubtypeOf1)(const QString&) const = &OOModel::NullLiteral::isSubtypeOf;
	bool (OOModel::NullLiteral::*NullLiteral_isSubtypeOf2)(int) const = &OOModel::NullLiteral::isSubtypeOf;
	Model::CompositeIndex (*NullLiteral_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::NullLiteral::registerNewAttribute;
	Model::CompositeIndex (*NullLiteral_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::NullLiteral::registerNewAttribute;
	auto aClass = class_<OOModel::NullLiteral, bases<OOModel::Expression>>("NullLiteral");
		aClass.def("typeName", make_function((const QString& (
			OOModel::NullLiteral::*)())&OOModel::NullLiteral::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::NullLiteral::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::NullLiteral::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::NullLiteral::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::NullLiteral::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::NullLiteral::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::NullLiteral::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::NullLiteral::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::NullLiteral::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::NullLiteral::type);
		aClass.def("isSubtypeOf", NullLiteral_isSubtypeOf1);
		aClass.def("isSubtypeOf", NullLiteral_isSubtypeOf2);
		aClass.def("registerNewAttribute", NullLiteral_registerNewAttribute1);
		aClass.def("registerNewAttribute", NullLiteral_registerNewAttribute2);
}

void initClassFloatLiteral() {
	Model::CompositeIndex (*FloatLiteral_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FloatLiteral::registerNewAttribute;
	Model::CompositeIndex (*FloatLiteral_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FloatLiteral::registerNewAttribute;
	bool (OOModel::FloatLiteral::*FloatLiteral_isSubtypeOf1)(const QString&) const = &OOModel::FloatLiteral::isSubtypeOf;
	bool (OOModel::FloatLiteral::*FloatLiteral_isSubtypeOf2)(int) const = &OOModel::FloatLiteral::isSubtypeOf;
	auto aClass = class_<OOModel::FloatLiteral, bases<OOModel::Expression>>("FloatLiteral");
		aClass.add_property("value",
			make_function((const QString& (OOModel::FloatLiteral::*)(
				))&OOModel::FloatLiteral::value, return_value_policy<copy_const_reference>()),
			&OOModel::FloatLiteral::setValue);
		aClass.def("typeName", make_function((const QString& (
			OOModel::FloatLiteral::*)())&OOModel::FloatLiteral::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::FloatLiteral::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::FloatLiteral::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::FloatLiteral::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::FloatLiteral::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::FloatLiteral::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::FloatLiteral::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::FloatLiteral::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::FloatLiteral::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("valueNode", make_function(&OOModel::FloatLiteral::valueNode, return_internal_reference<>()));
		aClass.def("type", &OOModel::FloatLiteral::type);
		aClass.def("registerNewAttribute", FloatLiteral_registerNewAttribute1);
		aClass.def("registerNewAttribute", FloatLiteral_registerNewAttribute2);
		aClass.def("isSubtypeOf", FloatLiteral_isSubtypeOf1);
		aClass.def("isSubtypeOf", FloatLiteral_isSubtypeOf2);
}

void initClassTypeTraitExpression() {
	Model::CompositeIndex (*TypeTraitExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::TypeTraitExpression::registerNewAttribute;
	Model::CompositeIndex (*TypeTraitExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::TypeTraitExpression::registerNewAttribute;
	bool (OOModel::TypeTraitExpression::*TypeTraitExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::TypeTraitExpression::isSubtypeOf;
	bool (OOModel::TypeTraitExpression::*TypeTraitExpression_isSubtypeOf2)(
		int) const = &OOModel::TypeTraitExpression::isSubtypeOf;
	auto aClass = class_<OOModel::TypeTraitExpression, bases<OOModel::Expression>>("TypeTraitExpression");
		aClass.add_property("operand",
			make_function(&OOModel::TypeTraitExpression::operand, return_internal_reference<>()),
			&OOModel::TypeTraitExpression::setOperand);
		aClass.add_property("typeTraitKind",
			&OOModel::TypeTraitExpression::typeTraitKind,
			&OOModel::TypeTraitExpression::setTypeTraitKind);
		aClass.def("typeName", make_function((const QString& (
		OOModel::TypeTraitExpression::*)())&OOModel::TypeTraitExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::TypeTraitExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::TypeTraitExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::TypeTraitExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::TypeTraitExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::TypeTraitExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::TypeTraitExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::TypeTraitExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::TypeTraitExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::TypeTraitExpression::type);
		aClass.def("registerNewAttribute", TypeTraitExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", TypeTraitExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", TypeTraitExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", TypeTraitExpression_isSubtypeOf2);
	scope TypeTraitExpressionScope = aClass;
	enum_<OOModel::TypeTraitExpression::TypeTraitKind>("TypeTraitKind")
		.value("SizeOf", OOModel::TypeTraitExpression::TypeTraitKind::SizeOf)
		.value("AlignOf", OOModel::TypeTraitExpression::TypeTraitKind::AlignOf)
		.value("TypeId", OOModel::TypeTraitExpression::TypeTraitKind::TypeId);
}

void initClassSuperExpression() {
	bool (OOModel::SuperExpression::*SuperExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::SuperExpression::isSubtypeOf;
	bool (OOModel::SuperExpression::*SuperExpression_isSubtypeOf2)(int) const = &OOModel::SuperExpression::isSubtypeOf;
	Model::CompositeIndex (*SuperExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::SuperExpression::registerNewAttribute;
	Model::CompositeIndex (*SuperExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::SuperExpression::registerNewAttribute;
	auto aClass = class_<OOModel::SuperExpression, bases<OOModel::Expression>>("SuperExpression");
		aClass.def("typeName", make_function((const QString& (
			OOModel::SuperExpression::*)())&OOModel::SuperExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::SuperExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::SuperExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::SuperExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::SuperExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::SuperExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::SuperExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::SuperExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::SuperExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::SuperExpression::type);
		aClass.def("isSubtypeOf", SuperExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", SuperExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", SuperExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", SuperExpression_registerNewAttribute2);
}

void initClassConditionalExpression() {
	bool (OOModel::ConditionalExpression::*ConditionalExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ConditionalExpression::isSubtypeOf;
	bool (OOModel::ConditionalExpression::*ConditionalExpression_isSubtypeOf2)(
		int) const = &OOModel::ConditionalExpression::isSubtypeOf;
	Model::CompositeIndex (*ConditionalExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ConditionalExpression::registerNewAttribute;
	Model::CompositeIndex (*ConditionalExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ConditionalExpression::registerNewAttribute;
	auto aClass = class_<OOModel::ConditionalExpression, bases<OOModel::Expression>>("ConditionalExpression");
		aClass.add_property("condition",
			make_function(&OOModel::ConditionalExpression::condition, return_internal_reference<>()),
			&OOModel::ConditionalExpression::setCondition);
		aClass.add_property("trueExpression",
			make_function(&OOModel::ConditionalExpression::trueExpression, return_internal_reference<>()),
			&OOModel::ConditionalExpression::setTrueExpression);
		aClass.add_property("falseExpression",
			make_function(&OOModel::ConditionalExpression::falseExpression, return_internal_reference<>()),
			&OOModel::ConditionalExpression::setFalseExpression);
		aClass.def("typeName", make_function((const QString& (
		OOModel::ConditionalExpression::*)())&OOModel::ConditionalExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ConditionalExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ConditionalExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ConditionalExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ConditionalExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ConditionalExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ConditionalExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ConditionalExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ConditionalExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::ConditionalExpression::type);
		aClass.def("isSubtypeOf", ConditionalExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", ConditionalExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", ConditionalExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", ConditionalExpression_registerNewAttribute2);
}

void initClassTypeNameOperator() {
	bool (OOModel::TypeNameOperator::*TypeNameOperator_isSubtypeOf1)(
		const QString&) const = &OOModel::TypeNameOperator::isSubtypeOf;
	bool (OOModel::TypeNameOperator::*TypeNameOperator_isSubtypeOf2)(int) const = &OOModel::TypeNameOperator::isSubtypeOf;
	Model::CompositeIndex (*TypeNameOperator_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::TypeNameOperator::registerNewAttribute;
	Model::CompositeIndex (*TypeNameOperator_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::TypeNameOperator::registerNewAttribute;
	auto aClass = class_<OOModel::TypeNameOperator, bases<OOModel::Expression>>("TypeNameOperator");
		aClass.add_property("typeExpression",
			make_function(&OOModel::TypeNameOperator::typeExpression, return_internal_reference<>()),
			&OOModel::TypeNameOperator::setTypeExpression);
		aClass.def("typeName", make_function((const QString& (
			OOModel::TypeNameOperator::*)())&OOModel::TypeNameOperator::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::TypeNameOperator::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::TypeNameOperator::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::TypeNameOperator::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::TypeNameOperator::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::TypeNameOperator::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::TypeNameOperator::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::TypeNameOperator::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::TypeNameOperator::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::TypeNameOperator::type);
		aClass.def("isSubtypeOf", TypeNameOperator_isSubtypeOf1);
		aClass.def("isSubtypeOf", TypeNameOperator_isSubtypeOf2);
		aClass.def("registerNewAttribute", TypeNameOperator_registerNewAttribute1);
		aClass.def("registerNewAttribute", TypeNameOperator_registerNewAttribute2);
}

void initClassThisExpression() {
	bool (OOModel::ThisExpression::*ThisExpression_isSubtypeOf1)(const
		QString&) const = &OOModel::ThisExpression::isSubtypeOf;
	bool (OOModel::ThisExpression::*ThisExpression_isSubtypeOf2)(int) const = &OOModel::ThisExpression::isSubtypeOf;
	Model::CompositeIndex (*ThisExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ThisExpression::registerNewAttribute;
	Model::CompositeIndex (*ThisExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ThisExpression::registerNewAttribute;
	auto aClass = class_<OOModel::ThisExpression, bases<OOModel::Expression>>("ThisExpression");
		aClass.def("typeName", make_function((const QString& (
			OOModel::ThisExpression::*)())&OOModel::ThisExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ThisExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ThisExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ThisExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ThisExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ThisExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ThisExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ThisExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ThisExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::ThisExpression::type);
		aClass.def("isSubtypeOf", ThisExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", ThisExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", ThisExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", ThisExpression_registerNewAttribute2);
}

void initClassBinaryOperation() {
	Model::CompositeIndex (*BinaryOperation_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::BinaryOperation::registerNewAttribute;
	Model::CompositeIndex (*BinaryOperation_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::BinaryOperation::registerNewAttribute;
	bool (OOModel::BinaryOperation::*BinaryOperation_isSubtypeOf1)(
		const QString&) const = &OOModel::BinaryOperation::isSubtypeOf;
	bool (OOModel::BinaryOperation::*BinaryOperation_isSubtypeOf2)(int) const = &OOModel::BinaryOperation::isSubtypeOf;
	auto aClass = class_<OOModel::BinaryOperation, bases<OOModel::Expression>>("BinaryOperation");
		aClass.add_property("left",
			make_function(&OOModel::BinaryOperation::left, return_internal_reference<>()),
			&OOModel::BinaryOperation::setLeft);
		aClass.add_property("right",
			make_function(&OOModel::BinaryOperation::right, return_internal_reference<>()),
			&OOModel::BinaryOperation::setRight);
		aClass.add_property("op",
			&OOModel::BinaryOperation::op,
			&OOModel::BinaryOperation::setOp);
		aClass.def("typeName", make_function((const QString& (
			OOModel::BinaryOperation::*)())&OOModel::BinaryOperation::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::BinaryOperation::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::BinaryOperation::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::BinaryOperation::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::BinaryOperation::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::BinaryOperation::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::BinaryOperation::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::BinaryOperation::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::BinaryOperation::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::BinaryOperation::type);
		aClass.def("registerNewAttribute", BinaryOperation_registerNewAttribute1);
		aClass.def("registerNewAttribute", BinaryOperation_registerNewAttribute2);
		aClass.def("isSubtypeOf", BinaryOperation_isSubtypeOf1);
		aClass.def("isSubtypeOf", BinaryOperation_isSubtypeOf2);
	scope BinaryOperationScope = aClass;
	enum_<OOModel::BinaryOperation::OperatorTypes>("OperatorTypes")
		.value("TIMES", OOModel::BinaryOperation::OperatorTypes::TIMES)
		.value("DIVIDE", OOModel::BinaryOperation::OperatorTypes::DIVIDE)
		.value("REMAINDER", OOModel::BinaryOperation::OperatorTypes::REMAINDER)
		.value("PLUS", OOModel::BinaryOperation::OperatorTypes::PLUS)
		.value("MINUS", OOModel::BinaryOperation::OperatorTypes::MINUS)
		.value("LEFT_SHIFT", OOModel::BinaryOperation::OperatorTypes::LEFT_SHIFT)
		.value("RIGHT_SHIFT_SIGNED", OOModel::BinaryOperation::OperatorTypes::RIGHT_SHIFT_SIGNED)
		.value("RIGHT_SHIFT_UNSIGNED", OOModel::BinaryOperation::OperatorTypes::RIGHT_SHIFT_UNSIGNED)
		.value("LESS", OOModel::BinaryOperation::OperatorTypes::LESS)
		.value("GREATER", OOModel::BinaryOperation::OperatorTypes::GREATER)
		.value("LESS_EQUALS", OOModel::BinaryOperation::OperatorTypes::LESS_EQUALS)
		.value("GREATER_EQUALS", OOModel::BinaryOperation::OperatorTypes::GREATER_EQUALS)
		.value("EQUALS", OOModel::BinaryOperation::OperatorTypes::EQUALS)
		.value("NOT_EQUALS", OOModel::BinaryOperation::OperatorTypes::NOT_EQUALS)
		.value("XOR", OOModel::BinaryOperation::OperatorTypes::XOR)
		.value("AND", OOModel::BinaryOperation::OperatorTypes::AND)
		.value("OR", OOModel::BinaryOperation::OperatorTypes::OR)
		.value("CONDITIONAL_AND", OOModel::BinaryOperation::OperatorTypes::CONDITIONAL_AND)
		.value("CONDITIONAL_OR", OOModel::BinaryOperation::OperatorTypes::CONDITIONAL_OR)
		.value("ARRAY_INDEX", OOModel::BinaryOperation::OperatorTypes::ARRAY_INDEX)
		.value("POINTER_TO_MEMBER", OOModel::BinaryOperation::OperatorTypes::POINTER_TO_MEMBER)
		.value("POINTER_POINTER_TO_MEMBER", OOModel::BinaryOperation::OperatorTypes::POINTER_POINTER_TO_MEMBER);
}

void initClassEmptyExpression() {
	bool (OOModel::EmptyExpression::*EmptyExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::EmptyExpression::isSubtypeOf;
	bool (OOModel::EmptyExpression::*EmptyExpression_isSubtypeOf2)(int) const = &OOModel::EmptyExpression::isSubtypeOf;
	Model::CompositeIndex (*EmptyExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::EmptyExpression::registerNewAttribute;
	Model::CompositeIndex (*EmptyExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::EmptyExpression::registerNewAttribute;
	auto aClass = class_<OOModel::EmptyExpression, bases<OOModel::Expression>>("EmptyExpression");
		aClass.def("typeName", make_function((const QString& (
			OOModel::EmptyExpression::*)())&OOModel::EmptyExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::EmptyExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::EmptyExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::EmptyExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::EmptyExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::EmptyExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::EmptyExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::EmptyExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::EmptyExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::EmptyExpression::type);
		aClass.def("isSubtypeOf", EmptyExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", EmptyExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", EmptyExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", EmptyExpression_registerNewAttribute2);
}

void initClassMethodCallExpression() {
	Model::CompositeIndex (*MethodCallExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MethodCallExpression::registerNewAttribute;
	Model::CompositeIndex (*MethodCallExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MethodCallExpression::registerNewAttribute;
	OOModel::Method* (OOModel::MethodCallExpression::*MethodCallExpression_methodDefinition1)(
		) = &OOModel::MethodCallExpression::methodDefinition;
	bool (OOModel::MethodCallExpression::*MethodCallExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::MethodCallExpression::isSubtypeOf;
	bool (OOModel::MethodCallExpression::*MethodCallExpression_isSubtypeOf2)(
		int) const = &OOModel::MethodCallExpression::isSubtypeOf;
	auto aClass = class_<OOModel::MethodCallExpression, bases<OOModel::Expression>>("MethodCallExpression");
		aClass.add_property("callee",
			make_function(&OOModel::MethodCallExpression::callee, return_internal_reference<>()),
			&OOModel::MethodCallExpression::setCallee);
		aClass.add_property("arguments",
			make_function(&OOModel::MethodCallExpression::arguments, return_internal_reference<>()),
			&OOModel::MethodCallExpression::setArguments);
		aClass.add_property("methodCallKind",
			&OOModel::MethodCallExpression::methodCallKind,
			&OOModel::MethodCallExpression::setMethodCallKind);
		aClass.def("typeName", make_function((const QString& (
		OOModel::MethodCallExpression::*)())&OOModel::MethodCallExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::MethodCallExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::MethodCallExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::MethodCallExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::MethodCallExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::MethodCallExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::MethodCallExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::MethodCallExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::MethodCallExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::MethodCallExpression::type);
		aClass.def("registerNewAttribute", MethodCallExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", MethodCallExpression_registerNewAttribute2);
		aClass.def("methodDefinition", make_function(MethodCallExpression_methodDefinition1, return_internal_reference<>()));
		aClass.def("isSubtypeOf", MethodCallExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", MethodCallExpression_isSubtypeOf2);
	scope MethodCallExpressionScope = aClass;
	enum_<OOModel::MethodCallExpression::MethodCallKind>("MethodCallKind")
		.value("Call", OOModel::MethodCallExpression::MethodCallKind::Call)
		.value("ListConstruction", OOModel::MethodCallExpression::MethodCallKind::ListConstruction)
		.value("CallConstruction", OOModel::MethodCallExpression::MethodCallKind::CallConstruction);
}

void initClassDeleteExpression() {
	Model::CompositeIndex (*DeleteExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::DeleteExpression::registerNewAttribute;
	Model::CompositeIndex (*DeleteExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::DeleteExpression::registerNewAttribute;
	bool (OOModel::DeleteExpression::*DeleteExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::DeleteExpression::isSubtypeOf;
	bool (OOModel::DeleteExpression::*DeleteExpression_isSubtypeOf2)(int) const = &OOModel::DeleteExpression::isSubtypeOf;
	auto aClass = class_<OOModel::DeleteExpression, bases<OOModel::Expression>>("DeleteExpression");
		aClass.add_property("expr",
			make_function(&OOModel::DeleteExpression::expr, return_internal_reference<>()),
			&OOModel::DeleteExpression::setExpr);
		aClass.add_property("isArray",
			&OOModel::DeleteExpression::isArray,
			&OOModel::DeleteExpression::setIsArray);
		aClass.def("typeName", make_function((const QString& (
			OOModel::DeleteExpression::*)())&OOModel::DeleteExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::DeleteExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::DeleteExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::DeleteExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::DeleteExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::DeleteExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::DeleteExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::DeleteExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::DeleteExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isArrayNode", make_function(&OOModel::DeleteExpression::isArrayNode, return_internal_reference<>()));
		aClass.def("type", &OOModel::DeleteExpression::type);
		aClass.def("registerNewAttribute", DeleteExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", DeleteExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", DeleteExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", DeleteExpression_isSubtypeOf2);
}

void initClassLambdaExpression() {
	Model::CompositeIndex (*LambdaExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::LambdaExpression::registerNewAttribute;
	Model::CompositeIndex (*LambdaExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::LambdaExpression::registerNewAttribute;
	bool (OOModel::LambdaExpression::*LambdaExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::LambdaExpression::isSubtypeOf;
	bool (OOModel::LambdaExpression::*LambdaExpression_isSubtypeOf2)(int) const = &OOModel::LambdaExpression::isSubtypeOf;
	auto aClass = class_<OOModel::LambdaExpression, bases<OOModel::Expression>>("LambdaExpression");
		aClass.add_property("arguments",
			make_function(&OOModel::LambdaExpression::arguments, return_internal_reference<>()),
			&OOModel::LambdaExpression::setArguments);
		aClass.add_property("results",
			make_function(&OOModel::LambdaExpression::results, return_internal_reference<>()),
			&OOModel::LambdaExpression::setResults);
		aClass.add_property("captures",
			make_function(&OOModel::LambdaExpression::captures, return_internal_reference<>()),
			&OOModel::LambdaExpression::setCaptures);
		aClass.add_property("body",
			make_function(&OOModel::LambdaExpression::body, return_internal_reference<>()),
			&OOModel::LambdaExpression::setBody);
		aClass.add_property("defaultCaptureType",
			&OOModel::LambdaExpression::defaultCaptureType,
			&OOModel::LambdaExpression::setDefaultCaptureType);
		aClass.def("typeName", make_function((const QString& (
			OOModel::LambdaExpression::*)())&OOModel::LambdaExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::LambdaExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::LambdaExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::LambdaExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::LambdaExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::LambdaExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::LambdaExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::LambdaExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::LambdaExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::LambdaExpression::type);
		aClass.def("registerNewAttribute", LambdaExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", LambdaExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", LambdaExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", LambdaExpression_isSubtypeOf2);
	scope LambdaExpressionScope = aClass;
	enum_<OOModel::LambdaExpression::DefaultCaptureType>("DefaultCaptureType")
		.value("None", OOModel::LambdaExpression::DefaultCaptureType::None)
		.value("Value", OOModel::LambdaExpression::DefaultCaptureType::Value)
		.value("Reference", OOModel::LambdaExpression::DefaultCaptureType::Reference);
}

void initClassIntegerLiteral() {
	Model::CompositeIndex (*IntegerLiteral_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::IntegerLiteral::registerNewAttribute;
	Model::CompositeIndex (*IntegerLiteral_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::IntegerLiteral::registerNewAttribute;
	bool (OOModel::IntegerLiteral::*IntegerLiteral_isSubtypeOf1)(const
		QString&) const = &OOModel::IntegerLiteral::isSubtypeOf;
	bool (OOModel::IntegerLiteral::*IntegerLiteral_isSubtypeOf2)(int) const = &OOModel::IntegerLiteral::isSubtypeOf;
	auto aClass = class_<OOModel::IntegerLiteral, bases<OOModel::Expression>>("IntegerLiteral");
		aClass.add_property("value",
			make_function((const QString& (OOModel::IntegerLiteral::*)(
				))&OOModel::IntegerLiteral::value, return_value_policy<copy_const_reference>()),
			&OOModel::IntegerLiteral::setValue);
		aClass.def("typeName", make_function((const QString& (
			OOModel::IntegerLiteral::*)())&OOModel::IntegerLiteral::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::IntegerLiteral::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::IntegerLiteral::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::IntegerLiteral::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::IntegerLiteral::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::IntegerLiteral::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::IntegerLiteral::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::IntegerLiteral::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::IntegerLiteral::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("valueNode", make_function(&OOModel::IntegerLiteral::valueNode, return_internal_reference<>()));
		aClass.def("valueAsInt", &OOModel::IntegerLiteral::valueAsInt);
		aClass.def("type", &OOModel::IntegerLiteral::type);
		aClass.def("registerNewAttribute", IntegerLiteral_registerNewAttribute1);
		aClass.def("registerNewAttribute", IntegerLiteral_registerNewAttribute2);
		aClass.def("isSubtypeOf", IntegerLiteral_isSubtypeOf1);
		aClass.def("isSubtypeOf", IntegerLiteral_isSubtypeOf2);
}

void initClassVariableDeclarationExpression() {
	Model::CompositeIndex (*VariableDeclarationExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::VariableDeclarationExpression::registerNewAttribute;
	Model::CompositeIndex (*VariableDeclarationExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::VariableDeclarationExpression::registerNewAttribute;
	bool (OOModel::VariableDeclarationExpression::*VariableDeclarationExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::VariableDeclarationExpression::isSubtypeOf;
	bool (OOModel::VariableDeclarationExpression::*VariableDeclarationExpression_isSubtypeOf2)(
		int) const = &OOModel::VariableDeclarationExpression::isSubtypeOf;
	auto aClass = class_<OOModel::VariableDeclarationExpression,
		 bases<OOModel::Expression>>("VariableDeclarationExpression");
		aClass.add_property("decl",
			make_function(&OOModel::VariableDeclarationExpression::decl, return_internal_reference<>()),
			&OOModel::VariableDeclarationExpression::setDecl);
		aClass.def("typeName", make_function((const QString& (
		OOModel::VariableDeclarationExpression::*)(
			))&OOModel::VariableDeclarationExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::VariableDeclarationExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::VariableDeclarationExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::VariableDeclarationExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::VariableDeclarationExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::VariableDeclarationExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::VariableDeclarationExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::VariableDeclarationExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(
			&OOModel::VariableDeclarationExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("definesSymbol", &OOModel::VariableDeclarationExpression::definesSymbol);
		aClass.def("symbolName", make_function((const QString& (
		OOModel::VariableDeclarationExpression::*)(
			))&OOModel::VariableDeclarationExpression::symbolName, return_value_policy<copy_const_reference>()));
		aClass.def("symbolType", &OOModel::VariableDeclarationExpression::symbolType);
		aClass.def("type", &OOModel::VariableDeclarationExpression::type);
		aClass.def("registerNewAttribute", VariableDeclarationExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", VariableDeclarationExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", VariableDeclarationExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", VariableDeclarationExpression_isSubtypeOf2);
}

void initClassStringLiteral() {
	Model::CompositeIndex (*StringLiteral_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::StringLiteral::registerNewAttribute;
	Model::CompositeIndex (*StringLiteral_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::StringLiteral::registerNewAttribute;
	bool (OOModel::StringLiteral::*StringLiteral_isSubtypeOf1)(
		const QString&) const = &OOModel::StringLiteral::isSubtypeOf;
	bool (OOModel::StringLiteral::*StringLiteral_isSubtypeOf2)(int) const = &OOModel::StringLiteral::isSubtypeOf;
	auto aClass = class_<OOModel::StringLiteral, bases<OOModel::Expression>>("StringLiteral");
		aClass.add_property("value",
			make_function((const QString& (OOModel::StringLiteral::*)(
				))&OOModel::StringLiteral::value, return_value_policy<copy_const_reference>()),
			&OOModel::StringLiteral::setValue);
		aClass.def("typeName", make_function((const QString& (
			OOModel::StringLiteral::*)())&OOModel::StringLiteral::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::StringLiteral::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::StringLiteral::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::StringLiteral::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::StringLiteral::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::StringLiteral::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::StringLiteral::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::StringLiteral::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::StringLiteral::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("valueNode", make_function(&OOModel::StringLiteral::valueNode, return_internal_reference<>()));
		aClass.def("type", &OOModel::StringLiteral::type);
		aClass.def("registerNewAttribute", StringLiteral_registerNewAttribute1);
		aClass.def("registerNewAttribute", StringLiteral_registerNewAttribute2);
		aClass.def("isSubtypeOf", StringLiteral_isSubtypeOf1);
		aClass.def("isSubtypeOf", StringLiteral_isSubtypeOf2);
}

void initClassInstanceOfExpression() {
	bool (OOModel::InstanceOfExpression::*InstanceOfExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::InstanceOfExpression::isSubtypeOf;
	bool (OOModel::InstanceOfExpression::*InstanceOfExpression_isSubtypeOf2)(
		int) const = &OOModel::InstanceOfExpression::isSubtypeOf;
	Model::CompositeIndex (*InstanceOfExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::InstanceOfExpression::registerNewAttribute;
	Model::CompositeIndex (*InstanceOfExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::InstanceOfExpression::registerNewAttribute;
	auto aClass = class_<OOModel::InstanceOfExpression, bases<OOModel::Expression>>("InstanceOfExpression");
		aClass.add_property("expr",
			make_function(&OOModel::InstanceOfExpression::expr, return_internal_reference<>()),
			&OOModel::InstanceOfExpression::setExpr);
		aClass.add_property("typeExpression",
			make_function(&OOModel::InstanceOfExpression::typeExpression, return_internal_reference<>()),
			&OOModel::InstanceOfExpression::setTypeExpression);
		aClass.def("typeName", make_function((const QString& (
		OOModel::InstanceOfExpression::*)())&OOModel::InstanceOfExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::InstanceOfExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::InstanceOfExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::InstanceOfExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::InstanceOfExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::InstanceOfExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::InstanceOfExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::InstanceOfExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::InstanceOfExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::InstanceOfExpression::type);
		aClass.def("isSubtypeOf", InstanceOfExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", InstanceOfExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", InstanceOfExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", InstanceOfExpression_registerNewAttribute2);
}

void initClassBooleanLiteral() {
	Model::CompositeIndex (*BooleanLiteral_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::BooleanLiteral::registerNewAttribute;
	Model::CompositeIndex (*BooleanLiteral_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::BooleanLiteral::registerNewAttribute;
	bool (OOModel::BooleanLiteral::*BooleanLiteral_isSubtypeOf1)(const
		QString&) const = &OOModel::BooleanLiteral::isSubtypeOf;
	bool (OOModel::BooleanLiteral::*BooleanLiteral_isSubtypeOf2)(int) const = &OOModel::BooleanLiteral::isSubtypeOf;
	auto aClass = class_<OOModel::BooleanLiteral, bases<OOModel::Expression>>("BooleanLiteral");
		aClass.add_property("value",
			&OOModel::BooleanLiteral::value,
			&OOModel::BooleanLiteral::setValue);
		aClass.def("typeName", make_function((const QString& (
			OOModel::BooleanLiteral::*)())&OOModel::BooleanLiteral::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::BooleanLiteral::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::BooleanLiteral::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::BooleanLiteral::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::BooleanLiteral::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::BooleanLiteral::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::BooleanLiteral::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::BooleanLiteral::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::BooleanLiteral::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("valueNode", make_function(&OOModel::BooleanLiteral::valueNode, return_internal_reference<>()));
		aClass.def("type", &OOModel::BooleanLiteral::type);
		aClass.def("registerNewAttribute", BooleanLiteral_registerNewAttribute1);
		aClass.def("registerNewAttribute", BooleanLiteral_registerNewAttribute2);
		aClass.def("isSubtypeOf", BooleanLiteral_isSubtypeOf1);
		aClass.def("isSubtypeOf", BooleanLiteral_isSubtypeOf2);
}

void initClassTypeExpression() {
	bool (OOModel::TypeExpression::*TypeExpression_isSubtypeOf1)(const
		QString&) const = &OOModel::TypeExpression::isSubtypeOf;
	bool (OOModel::TypeExpression::*TypeExpression_isSubtypeOf2)(int) const = &OOModel::TypeExpression::isSubtypeOf;
	Model::CompositeIndex (*TypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::TypeExpression::registerNewAttribute;
	Model::CompositeIndex (*TypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::TypeExpression::registerNewAttribute;
	auto aClass = class_<OOModel::TypeExpression, bases<OOModel::Expression>>("TypeExpression");
		aClass.def("typeName", make_function((const QString& (
			OOModel::TypeExpression::*)())&OOModel::TypeExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::TypeExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::TypeExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::TypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::TypeExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::TypeExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::TypeExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::TypeExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::TypeExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", TypeExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", TypeExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", TypeExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", TypeExpression_registerNewAttribute2);
}

void initClassReferenceExpression() {
	Model::CompositeIndex (*ReferenceExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ReferenceExpression::registerNewAttribute;
	Model::CompositeIndex (*ReferenceExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ReferenceExpression::registerNewAttribute;
	bool (OOModel::ReferenceExpression::*ReferenceExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ReferenceExpression::isSubtypeOf;
	bool (OOModel::ReferenceExpression::*ReferenceExpression_isSubtypeOf2)(
		int) const = &OOModel::ReferenceExpression::isSubtypeOf;
	auto aClass = class_<OOModel::ReferenceExpression, bases<OOModel::Expression>>("ReferenceExpression");
		aClass.add_property("prefix",
			make_function(&OOModel::ReferenceExpression::prefix, return_internal_reference<>()),
			&OOModel::ReferenceExpression::setPrefix);
		aClass.add_property("ref",
			make_function(&OOModel::ReferenceExpression::ref, return_internal_reference<>()),
			&OOModel::ReferenceExpression::setRef);
		aClass.add_property("typeArguments",
			make_function(&OOModel::ReferenceExpression::typeArguments, return_internal_reference<>()),
			&OOModel::ReferenceExpression::setTypeArguments);
		aClass.add_property("memberKind",
			&OOModel::ReferenceExpression::memberKind,
			&OOModel::ReferenceExpression::setMemberKind);
		aClass.add_property("name",
			make_function((const QString& (OOModel::ReferenceExpression::*)(
				))&OOModel::ReferenceExpression::name, return_value_policy<copy_const_reference>()),
			&OOModel::ReferenceExpression::setName);
		aClass.def("typeName", make_function((const QString& (
		OOModel::ReferenceExpression::*)())&OOModel::ReferenceExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ReferenceExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ReferenceExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ReferenceExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ReferenceExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ReferenceExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ReferenceExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ReferenceExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ReferenceExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("target", make_function(&OOModel::ReferenceExpression::target, return_internal_reference<>()));
		aClass.def("type", &OOModel::ReferenceExpression::type);
		aClass.def("registerNewAttribute", ReferenceExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", ReferenceExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", ReferenceExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", ReferenceExpression_isSubtypeOf2);
	scope ReferenceExpressionScope = aClass;
	enum_<OOModel::ReferenceExpression::MemberKind>("MemberKind")
		.value("Dot", OOModel::ReferenceExpression::MemberKind::Dot)
		.value("Pointer", OOModel::ReferenceExpression::MemberKind::Pointer)
		.value("Static", OOModel::ReferenceExpression::MemberKind::Static)
		.value("Template", OOModel::ReferenceExpression::MemberKind::Template);
}

void initClassArrayInitializer() {
	bool (OOModel::ArrayInitializer::*ArrayInitializer_isSubtypeOf1)(
		const QString&) const = &OOModel::ArrayInitializer::isSubtypeOf;
	bool (OOModel::ArrayInitializer::*ArrayInitializer_isSubtypeOf2)(int) const = &OOModel::ArrayInitializer::isSubtypeOf;
	Model::CompositeIndex (*ArrayInitializer_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ArrayInitializer::registerNewAttribute;
	Model::CompositeIndex (*ArrayInitializer_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ArrayInitializer::registerNewAttribute;
	auto aClass = class_<OOModel::ArrayInitializer, bases<OOModel::Expression>>("ArrayInitializer");
		aClass.add_property("values",
			make_function(&OOModel::ArrayInitializer::values, return_internal_reference<>()),
			&OOModel::ArrayInitializer::setValues);
		aClass.def("typeName", make_function((const QString& (
			OOModel::ArrayInitializer::*)())&OOModel::ArrayInitializer::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ArrayInitializer::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ArrayInitializer::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ArrayInitializer::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ArrayInitializer::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ArrayInitializer::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ArrayInitializer::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ArrayInitializer::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ArrayInitializer::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::ArrayInitializer::type);
		aClass.def("isSubtypeOf", ArrayInitializer_isSubtypeOf1);
		aClass.def("isSubtypeOf", ArrayInitializer_isSubtypeOf2);
		aClass.def("registerNewAttribute", ArrayInitializer_registerNewAttribute1);
		aClass.def("registerNewAttribute", ArrayInitializer_registerNewAttribute2);
}

void initClassThrowExpression() {
	bool (OOModel::ThrowExpression::*ThrowExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ThrowExpression::isSubtypeOf;
	bool (OOModel::ThrowExpression::*ThrowExpression_isSubtypeOf2)(int) const = &OOModel::ThrowExpression::isSubtypeOf;
	Model::CompositeIndex (*ThrowExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ThrowExpression::registerNewAttribute;
	Model::CompositeIndex (*ThrowExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ThrowExpression::registerNewAttribute;
	auto aClass = class_<OOModel::ThrowExpression, bases<OOModel::Expression>>("ThrowExpression");
		aClass.add_property("expr",
			make_function(&OOModel::ThrowExpression::expr, return_internal_reference<>()),
			&OOModel::ThrowExpression::setExpr);
		aClass.def("typeName", make_function((const QString& (
			OOModel::ThrowExpression::*)())&OOModel::ThrowExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ThrowExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ThrowExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ThrowExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ThrowExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ThrowExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ThrowExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ThrowExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ThrowExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::ThrowExpression::type);
		aClass.def("isSubtypeOf", ThrowExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", ThrowExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", ThrowExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", ThrowExpression_registerNewAttribute2);
}

void initClassErrorExpression() {
	Model::CompositeIndex (*ErrorExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ErrorExpression::registerNewAttribute;
	Model::CompositeIndex (*ErrorExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ErrorExpression::registerNewAttribute;
	bool (OOModel::ErrorExpression::*ErrorExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ErrorExpression::isSubtypeOf;
	bool (OOModel::ErrorExpression::*ErrorExpression_isSubtypeOf2)(int) const = &OOModel::ErrorExpression::isSubtypeOf;
	auto aClass = class_<OOModel::ErrorExpression, bases<OOModel::Expression>>("ErrorExpression");
		aClass.add_property("prefix",
			make_function((const QString& (OOModel::ErrorExpression::*)(
				))&OOModel::ErrorExpression::prefix, return_value_policy<copy_const_reference>()),
			&OOModel::ErrorExpression::setPrefix);
		aClass.add_property("arg",
			make_function(&OOModel::ErrorExpression::arg, return_internal_reference<>()),
			&OOModel::ErrorExpression::setArg);
		aClass.add_property("postfix",
			make_function((const QString& (OOModel::ErrorExpression::*)(
				))&OOModel::ErrorExpression::postfix, return_value_policy<copy_const_reference>()),
			&OOModel::ErrorExpression::setPostfix);
		aClass.def("typeName", make_function((const QString& (
			OOModel::ErrorExpression::*)())&OOModel::ErrorExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ErrorExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ErrorExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ErrorExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ErrorExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ErrorExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ErrorExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ErrorExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ErrorExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("prefixNode", make_function(&OOModel::ErrorExpression::prefixNode, return_internal_reference<>()));
		aClass.def("postfixNode", make_function(&OOModel::ErrorExpression::postfixNode, return_internal_reference<>()));
		aClass.def("type", &OOModel::ErrorExpression::type);
		aClass.def("registerNewAttribute", ErrorExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", ErrorExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", ErrorExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", ErrorExpression_isSubtypeOf2);
}

void initClassAssignmentExpression() {
	Model::CompositeIndex (*AssignmentExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::AssignmentExpression::registerNewAttribute;
	Model::CompositeIndex (*AssignmentExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::AssignmentExpression::registerNewAttribute;
	bool (OOModel::AssignmentExpression::*AssignmentExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::AssignmentExpression::isSubtypeOf;
	bool (OOModel::AssignmentExpression::*AssignmentExpression_isSubtypeOf2)(
		int) const = &OOModel::AssignmentExpression::isSubtypeOf;
	auto aClass = class_<OOModel::AssignmentExpression, bases<OOModel::Expression>>("AssignmentExpression");
		aClass.add_property("left",
			make_function(&OOModel::AssignmentExpression::left, return_internal_reference<>()),
			&OOModel::AssignmentExpression::setLeft);
		aClass.add_property("right",
			make_function(&OOModel::AssignmentExpression::right, return_internal_reference<>()),
			&OOModel::AssignmentExpression::setRight);
		aClass.add_property("op",
			&OOModel::AssignmentExpression::op,
			&OOModel::AssignmentExpression::setOp);
		aClass.def("typeName", make_function((const QString& (
		OOModel::AssignmentExpression::*)())&OOModel::AssignmentExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::AssignmentExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::AssignmentExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::AssignmentExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::AssignmentExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::AssignmentExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::AssignmentExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::AssignmentExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::AssignmentExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::AssignmentExpression::type);
		aClass.def("registerNewAttribute", AssignmentExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", AssignmentExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", AssignmentExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", AssignmentExpression_isSubtypeOf2);
	scope AssignmentExpressionScope = aClass;
	enum_<OOModel::AssignmentExpression::AssignmentTypes>("AssignmentTypes")
		.value("ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::ASSIGN)
		.value("PLUS_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::PLUS_ASSIGN)
		.value("MINUS_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::MINUS_ASSIGN)
		.value("TIMES_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::TIMES_ASSIGN)
		.value("DIVIDE_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::DIVIDE_ASSIGN)
		.value("BIT_AND_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::BIT_AND_ASSIGN)
		.value("BIT_OR_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::BIT_OR_ASSIGN)
		.value("BIT_XOR_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::BIT_XOR_ASSIGN)
		.value("REMAINDER_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::REMAINDER_ASSIGN)
		.value("LEFT_SHIFT_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::LEFT_SHIFT_ASSIGN)
		.value("RIGHT_SHIFT_SIGNED_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::RIGHT_SHIFT_SIGNED_ASSIGN)
		.value("RIGHT_SHIFT_UNSIGNED_ASSIGN", OOModel::AssignmentExpression::AssignmentTypes::RIGHT_SHIFT_UNSIGNED_ASSIGN);
}

void initClassGlobalScopeExpression() {
	bool (OOModel::GlobalScopeExpression::*GlobalScopeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::GlobalScopeExpression::isSubtypeOf;
	bool (OOModel::GlobalScopeExpression::*GlobalScopeExpression_isSubtypeOf2)(
		int) const = &OOModel::GlobalScopeExpression::isSubtypeOf;
	Model::CompositeIndex (*GlobalScopeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::GlobalScopeExpression::registerNewAttribute;
	Model::CompositeIndex (*GlobalScopeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::GlobalScopeExpression::registerNewAttribute;
	auto aClass = class_<OOModel::GlobalScopeExpression, bases<OOModel::Expression>>("GlobalScopeExpression");
		aClass.def("typeName", make_function((const QString& (
		OOModel::GlobalScopeExpression::*)())&OOModel::GlobalScopeExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::GlobalScopeExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::GlobalScopeExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::GlobalScopeExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::GlobalScopeExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::GlobalScopeExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::GlobalScopeExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::GlobalScopeExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::GlobalScopeExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::GlobalScopeExpression::type);
		aClass.def("isSubtypeOf", GlobalScopeExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", GlobalScopeExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", GlobalScopeExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", GlobalScopeExpression_registerNewAttribute2);
}

void initClassCharacterLiteral() {
	Model::CompositeIndex (*CharacterLiteral_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::CharacterLiteral::registerNewAttribute;
	Model::CompositeIndex (*CharacterLiteral_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::CharacterLiteral::registerNewAttribute;
	bool (OOModel::CharacterLiteral::*CharacterLiteral_isSubtypeOf1)(
		const QString&) const = &OOModel::CharacterLiteral::isSubtypeOf;
	bool (OOModel::CharacterLiteral::*CharacterLiteral_isSubtypeOf2)(int) const = &OOModel::CharacterLiteral::isSubtypeOf;
	auto aClass = class_<OOModel::CharacterLiteral, bases<OOModel::Expression>>("CharacterLiteral");
		aClass.add_property("value",
			make_function((const QString& (
				OOModel::CharacterLiteral::*)())&OOModel::CharacterLiteral::value, return_value_policy<copy_const_reference>()),
			&OOModel::CharacterLiteral::setValue);
		aClass.def("typeName", make_function((const QString& (
			OOModel::CharacterLiteral::*)())&OOModel::CharacterLiteral::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::CharacterLiteral::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::CharacterLiteral::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::CharacterLiteral::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::CharacterLiteral::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::CharacterLiteral::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::CharacterLiteral::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::CharacterLiteral::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::CharacterLiteral::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("valueNode", make_function(&OOModel::CharacterLiteral::valueNode, return_internal_reference<>()));
		aClass.def("type", &OOModel::CharacterLiteral::type);
		aClass.def("registerNewAttribute", CharacterLiteral_registerNewAttribute1);
		aClass.def("registerNewAttribute", CharacterLiteral_registerNewAttribute2);
		aClass.def("isSubtypeOf", CharacterLiteral_isSubtypeOf1);
		aClass.def("isSubtypeOf", CharacterLiteral_isSubtypeOf2);
}

void initClassCastExpression() {
	Model::CompositeIndex (*CastExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::CastExpression::registerNewAttribute;
	Model::CompositeIndex (*CastExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::CastExpression::registerNewAttribute;
	bool (OOModel::CastExpression::*CastExpression_isSubtypeOf1)(const
		QString&) const = &OOModel::CastExpression::isSubtypeOf;
	bool (OOModel::CastExpression::*CastExpression_isSubtypeOf2)(int) const = &OOModel::CastExpression::isSubtypeOf;
	auto aClass = class_<OOModel::CastExpression, bases<OOModel::Expression>>("CastExpression");
		aClass.add_property("expr",
			make_function(&OOModel::CastExpression::expr, return_internal_reference<>()),
			&OOModel::CastExpression::setExpr);
		aClass.add_property("type",
			&OOModel::CastExpression::type,
			&OOModel::CastExpression::setType);
		aClass.add_property("castKind",
			&OOModel::CastExpression::castKind,
			&OOModel::CastExpression::setCastKind);
		aClass.def("typeName", make_function((const QString& (
			OOModel::CastExpression::*)())&OOModel::CastExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::CastExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::CastExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::CastExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::CastExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::CastExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::CastExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::CastExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::CastExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("castType", make_function(&OOModel::CastExpression::castType, return_internal_reference<>()));
		aClass.def("registerNewAttribute", CastExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", CastExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", CastExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", CastExpression_isSubtypeOf2);
	scope CastExpressionScope = aClass;
	enum_<OOModel::CastExpression::CastKind>("CastKind")
		.value("Default", OOModel::CastExpression::CastKind::Default)
		.value("ConstCast", OOModel::CastExpression::CastKind::ConstCast)
		.value("DynamicCast", OOModel::CastExpression::CastKind::DynamicCast)
		.value("ReinterpretCast", OOModel::CastExpression::CastKind::ReinterpretCast)
		.value("StaticCast", OOModel::CastExpression::CastKind::StaticCast)
		.value("FunctionalCast", OOModel::CastExpression::CastKind::FunctionalCast);
}

void initClassNewExpression() {
	bool (OOModel::NewExpression::*NewExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::NewExpression::isSubtypeOf;
	bool (OOModel::NewExpression::*NewExpression_isSubtypeOf2)(int) const = &OOModel::NewExpression::isSubtypeOf;
	Model::CompositeIndex (*NewExpression_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::NewExpression::registerNewAttribute;
	Model::CompositeIndex (*NewExpression_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::NewExpression::registerNewAttribute;
	auto aClass = class_<OOModel::NewExpression, bases<OOModel::Expression>>("NewExpression");
		aClass.add_property("newType",
			make_function(&OOModel::NewExpression::newType, return_internal_reference<>()),
			&OOModel::NewExpression::setNewType);
		aClass.add_property("dimensions",
			make_function(&OOModel::NewExpression::dimensions, return_internal_reference<>()),
			&OOModel::NewExpression::setDimensions);
		aClass.add_property("initializer",
			make_function(&OOModel::NewExpression::initializer, return_internal_reference<>()),
			&OOModel::NewExpression::setInitializer);
		aClass.def("typeName", make_function((const QString& (
			OOModel::NewExpression::*)())&OOModel::NewExpression::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::NewExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::NewExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::NewExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::NewExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::NewExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::NewExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::NewExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::NewExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::NewExpression::type);
		aClass.def("isSubtypeOf", NewExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", NewExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", NewExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", NewExpression_registerNewAttribute2);
}

void initClassProject() {
	Model::CompositeIndex (*Project_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::Project::registerNewAttribute;
	Model::CompositeIndex (*Project_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::Project::registerNewAttribute;
	bool (OOModel::Project::*Project_isSubtypeOf1)(const QString&) const = &OOModel::Project::isSubtypeOf;
	bool (OOModel::Project::*Project_isSubtypeOf2)(int) const = &OOModel::Project::isSubtypeOf;
	auto aClass = class_<OOModel::Project, bases<OOModel::Declaration>>("Project");
		aClass.add_property("projects",
			make_function(&OOModel::Project::projects, return_internal_reference<>()),
			&OOModel::Project::setProjects);
		aClass.add_property("modules",
			make_function(&OOModel::Project::modules, return_internal_reference<>()),
			&OOModel::Project::setModules);
		aClass.add_property("classes",
			make_function(&OOModel::Project::classes, return_internal_reference<>()),
			&OOModel::Project::setClasses);
		aClass.add_property("methods",
			make_function(&OOModel::Project::methods, return_internal_reference<>()),
			&OOModel::Project::setMethods);
		aClass.add_property("fields",
			make_function(&OOModel::Project::fields, return_internal_reference<>()),
			&OOModel::Project::setFields);
		aClass.add_property("libraries",
			make_function(&OOModel::Project::libraries, return_internal_reference<>()),
			&OOModel::Project::setLibraries);
		aClass.add_property("implicitBaseType",
			make_function(&OOModel::Project::implicitBaseType, return_internal_reference<>()),
			&OOModel::Project::setImplicitBaseType);
		aClass.add_property("implicitEnumType",
			make_function(&OOModel::Project::implicitEnumType, return_internal_reference<>()),
			&OOModel::Project::setImplicitEnumType);
		aClass.add_property("implicitArrayType",
			make_function(&OOModel::Project::implicitArrayType, return_internal_reference<>()),
			&OOModel::Project::setImplicitArrayType);
		aClass.def("typeName", make_function((const QString& (
			OOModel::Project::*)())&OOModel::Project::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Project::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Project::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&OOModel::Project::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Project::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Project::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Project::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Project::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::Project::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isTransparentForNameResolution", &OOModel::Project::isTransparentForNameResolution);
		aClass.def("symbolType", &OOModel::Project::symbolType);
		aClass.def("usedLibraries", &OOModel::Project::usedLibraries);
		aClass.def("isNewPersistenceUnit", &OOModel::Project::isNewPersistenceUnit);
		aClass.def("registerNewAttribute", Project_registerNewAttribute1);
		aClass.def("registerNewAttribute", Project_registerNewAttribute2);
		aClass.def("isSubtypeOf", Project_isSubtypeOf1);
		aClass.def("isSubtypeOf", Project_isSubtypeOf2);
}

void initClassExplicitTemplateInstantiation() {
	Model::CompositeIndex (*ExplicitTemplateInstantiation_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ExplicitTemplateInstantiation::registerNewAttribute;
	Model::CompositeIndex (*ExplicitTemplateInstantiation_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ExplicitTemplateInstantiation::registerNewAttribute;
	bool (OOModel::ExplicitTemplateInstantiation::*ExplicitTemplateInstantiation_isSubtypeOf1)(
		const QString&) const = &OOModel::ExplicitTemplateInstantiation::isSubtypeOf;
	bool (OOModel::ExplicitTemplateInstantiation::*ExplicitTemplateInstantiation_isSubtypeOf2)(
		int) const = &OOModel::ExplicitTemplateInstantiation::isSubtypeOf;
	auto aClass = class_<OOModel::ExplicitTemplateInstantiation,
		 bases<OOModel::Declaration>>("ExplicitTemplateInstantiation");
		aClass.add_property("instantiatedClass",
			make_function(&OOModel::ExplicitTemplateInstantiation::instantiatedClass, return_internal_reference<>()),
			&OOModel::ExplicitTemplateInstantiation::setInstantiatedClass);
		aClass.def("typeName", make_function((const QString& (
		OOModel::ExplicitTemplateInstantiation::*)(
			))&OOModel::ExplicitTemplateInstantiation::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ExplicitTemplateInstantiation::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ExplicitTemplateInstantiation::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ExplicitTemplateInstantiation::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ExplicitTemplateInstantiation::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ExplicitTemplateInstantiation::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ExplicitTemplateInstantiation::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ExplicitTemplateInstantiation::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(
			&OOModel::ExplicitTemplateInstantiation::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("definesSymbol", &OOModel::ExplicitTemplateInstantiation::definesSymbol);
		aClass.def("symbolType", &OOModel::ExplicitTemplateInstantiation::symbolType);
		aClass.def("registerNewAttribute", ExplicitTemplateInstantiation_registerNewAttribute1);
		aClass.def("registerNewAttribute", ExplicitTemplateInstantiation_registerNewAttribute2);
		aClass.def("isSubtypeOf", ExplicitTemplateInstantiation_isSubtypeOf1);
		aClass.def("isSubtypeOf", ExplicitTemplateInstantiation_isSubtypeOf2);
}

void initClassMetaBinding() {
	bool (OOModel::MetaBinding::*MetaBinding_isSubtypeOf1)(const QString&) const = &OOModel::MetaBinding::isSubtypeOf;
	bool (OOModel::MetaBinding::*MetaBinding_isSubtypeOf2)(int) const = &OOModel::MetaBinding::isSubtypeOf;
	Model::CompositeIndex (*MetaBinding_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MetaBinding::registerNewAttribute;
	Model::CompositeIndex (*MetaBinding_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MetaBinding::registerNewAttribute;
	auto aClass = class_<OOModel::MetaBinding, bases<OOModel::Declaration>>("MetaBinding");
		aClass.add_property("input",
			make_function(&OOModel::MetaBinding::input, return_internal_reference<>()),
			&OOModel::MetaBinding::setInput);
		aClass.add_property("mappings",
			make_function(&OOModel::MetaBinding::mappings, return_internal_reference<>()),
			&OOModel::MetaBinding::setMappings);
		aClass.def("typeName", make_function((const QString& (
			OOModel::MetaBinding::*)())&OOModel::MetaBinding::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::MetaBinding::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::MetaBinding::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::MetaBinding::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::MetaBinding::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::MetaBinding::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::MetaBinding::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::MetaBinding::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::MetaBinding::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("symbolType", &OOModel::MetaBinding::symbolType);
		aClass.def("isSubtypeOf", MetaBinding_isSubtypeOf1);
		aClass.def("isSubtypeOf", MetaBinding_isSubtypeOf2);
		aClass.def("registerNewAttribute", MetaBinding_registerNewAttribute1);
		aClass.def("registerNewAttribute", MetaBinding_registerNewAttribute2);
}

void initClassNameImport() {
	Model::CompositeIndex (*NameImport_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::NameImport::registerNewAttribute;
	Model::CompositeIndex (*NameImport_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::NameImport::registerNewAttribute;
	bool (OOModel::NameImport::*NameImport_isSubtypeOf1)(const QString&) const = &OOModel::NameImport::isSubtypeOf;
	bool (OOModel::NameImport::*NameImport_isSubtypeOf2)(int) const = &OOModel::NameImport::isSubtypeOf;
	auto aClass = class_<OOModel::NameImport, bases<OOModel::Declaration>>("NameImport");
		aClass.add_property("importedName",
			make_function(&OOModel::NameImport::importedName, return_internal_reference<>()),
			&OOModel::NameImport::setImportedName);
		aClass.add_property("importAll",
			&OOModel::NameImport::importAll,
			&OOModel::NameImport::setImportAll);
		aClass.def("typeName", make_function((const QString& (
			OOModel::NameImport::*)())&OOModel::NameImport::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::NameImport::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::NameImport::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::NameImport::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::NameImport::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::NameImport::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::NameImport::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::NameImport::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::NameImport::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("importAllNode", make_function(&OOModel::NameImport::importAllNode, return_internal_reference<>()));
		aClass.def("definesSymbol", &OOModel::NameImport::definesSymbol);
		aClass.def("registerNewAttribute", NameImport_registerNewAttribute1);
		aClass.def("registerNewAttribute", NameImport_registerNewAttribute2);
		aClass.def("isSubtypeOf", NameImport_isSubtypeOf1);
		aClass.def("isSubtypeOf", NameImport_isSubtypeOf2);
}

void initClassVariableDeclaration() {
	Model::CompositeIndex (*VariableDeclaration_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::VariableDeclaration::registerNewAttribute;
	Model::CompositeIndex (*VariableDeclaration_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::VariableDeclaration::registerNewAttribute;
	bool (OOModel::VariableDeclaration::*VariableDeclaration_isSubtypeOf1)(
		const QString&) const = &OOModel::VariableDeclaration::isSubtypeOf;
	bool (OOModel::VariableDeclaration::*VariableDeclaration_isSubtypeOf2)(
		int) const = &OOModel::VariableDeclaration::isSubtypeOf;
	auto aClass = class_<OOModel::VariableDeclaration, bases<OOModel::Declaration>>("VariableDeclaration");
		aClass.add_property("typeExpression",
			make_function(&OOModel::VariableDeclaration::typeExpression, return_internal_reference<>()),
			&OOModel::VariableDeclaration::setTypeExpression);
		aClass.add_property("initialValue",
			make_function(&OOModel::VariableDeclaration::initialValue, return_internal_reference<>()),
			&OOModel::VariableDeclaration::setInitialValue);
		aClass.add_property("initializationKind",
			&OOModel::VariableDeclaration::initializationKind,
			&OOModel::VariableDeclaration::setInitializationKind);
		aClass.def("typeName", make_function((const QString& (
		OOModel::VariableDeclaration::*)())&OOModel::VariableDeclaration::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::VariableDeclaration::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::VariableDeclaration::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::VariableDeclaration::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::VariableDeclaration::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::VariableDeclaration::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::VariableDeclaration::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::VariableDeclaration::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::VariableDeclaration::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("symbolType", &OOModel::VariableDeclaration::symbolType);
		aClass.def("registerNewAttribute", VariableDeclaration_registerNewAttribute1);
		aClass.def("registerNewAttribute", VariableDeclaration_registerNewAttribute2);
		aClass.def("isSubtypeOf", VariableDeclaration_isSubtypeOf1);
		aClass.def("isSubtypeOf", VariableDeclaration_isSubtypeOf2);
	scope VariableDeclarationScope = aClass;
	enum_<OOModel::VariableDeclaration::InitializationKind>("InitializationKind")
		.value("StandardInitialization", OOModel::VariableDeclaration::InitializationKind::StandardInitialization)
		.value("CallInitialization", OOModel::VariableDeclaration::InitializationKind::CallInitialization);
}

void initClassMetaCallMapping() {
	bool (OOModel::MetaCallMapping::*MetaCallMapping_isSubtypeOf1)(
		const QString&) const = &OOModel::MetaCallMapping::isSubtypeOf;
	bool (OOModel::MetaCallMapping::*MetaCallMapping_isSubtypeOf2)(int) const = &OOModel::MetaCallMapping::isSubtypeOf;
	Model::CompositeIndex (*MetaCallMapping_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MetaCallMapping::registerNewAttribute;
	Model::CompositeIndex (*MetaCallMapping_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MetaCallMapping::registerNewAttribute;
	auto aClass = class_<OOModel::MetaCallMapping, bases<OOModel::Declaration>>("MetaCallMapping");
		aClass.add_property("value",
			make_function(&OOModel::MetaCallMapping::value, return_internal_reference<>()),
			&OOModel::MetaCallMapping::setValue);
		aClass.def("typeName", make_function((const QString& (
			OOModel::MetaCallMapping::*)())&OOModel::MetaCallMapping::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::MetaCallMapping::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::MetaCallMapping::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::MetaCallMapping::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::MetaCallMapping::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::MetaCallMapping::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::MetaCallMapping::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::MetaCallMapping::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::MetaCallMapping::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("symbolType", &OOModel::MetaCallMapping::symbolType);
		aClass.def("isSubtypeOf", MetaCallMapping_isSubtypeOf1);
		aClass.def("isSubtypeOf", MetaCallMapping_isSubtypeOf2);
		aClass.def("registerNewAttribute", MetaCallMapping_registerNewAttribute1);
		aClass.def("registerNewAttribute", MetaCallMapping_registerNewAttribute2);
}

void initClassMethod() {
	Model::CompositeIndex (*Method_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::Method::registerNewAttribute;
	Model::CompositeIndex (*Method_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Method::registerNewAttribute;
	bool (OOModel::Method::*Method_isSubtypeOf1)(const QString&) const = &OOModel::Method::isSubtypeOf;
	bool (OOModel::Method::*Method_isSubtypeOf2)(int) const = &OOModel::Method::isSubtypeOf;
	auto aClass = class_<OOModel::Method, bases<OOModel::Declaration>>("Method");
		aClass.add_property("items",
			make_function(&OOModel::Method::items, return_internal_reference<>()),
			&OOModel::Method::setItems);
		aClass.add_property("typeArguments",
			make_function(&OOModel::Method::typeArguments, return_internal_reference<>()),
			&OOModel::Method::setTypeArguments);
		aClass.add_property("arguments",
			make_function(&OOModel::Method::arguments, return_internal_reference<>()),
			&OOModel::Method::setArguments);
		aClass.add_property("results",
			make_function(&OOModel::Method::results, return_internal_reference<>()),
			&OOModel::Method::setResults);
		aClass.add_property("memberInitializers",
			make_function(&OOModel::Method::memberInitializers, return_internal_reference<>()),
			&OOModel::Method::setMemberInitializers);
		aClass.add_property("throws",
			make_function(&OOModel::Method::throws, return_internal_reference<>()),
			&OOModel::Method::setThrows);
		aClass.add_property("methodKind",
			&OOModel::Method::methodKind,
			&OOModel::Method::setMethodKind);
		aClass.def("typeName", make_function((const QString& (
			OOModel::Method::*)())&OOModel::Method::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Method::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Method::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&OOModel::Method::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Method::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Method::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Method::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Method::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::Method::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("fullyQualifiedName", &OOModel::Method::fullyQualifiedName);
		aClass.def("isGeneric", &OOModel::Method::isGeneric);
		aClass.def("isStatic", &OOModel::Method::isStatic);
		aClass.def("symbolType", &OOModel::Method::symbolType);
		aClass.def("overrides", &OOModel::Method::overrides);
		aClass.def("isOverridenBy", &OOModel::Method::isOverridenBy);
		aClass.def("callees", &OOModel::Method::callees);
		aClass.def("callers", &OOModel::Method::callers);
		aClass.def("isNewPersistenceUnit", &OOModel::Method::isNewPersistenceUnit);
		aClass.def("registerNewAttribute", Method_registerNewAttribute1);
		aClass.def("registerNewAttribute", Method_registerNewAttribute2);
		aClass.def("isSubtypeOf", Method_isSubtypeOf1);
		aClass.def("isSubtypeOf", Method_isSubtypeOf2);
	scope MethodScope = aClass;
	enum_<OOModel::Method::MethodKind>("MethodKind")
		.value("Default", OOModel::Method::MethodKind::Default)
		.value("Constructor", OOModel::Method::MethodKind::Constructor)
		.value("Destructor", OOModel::Method::MethodKind::Destructor)
		.value("Conversion", OOModel::Method::MethodKind::Conversion)
		.value("OperatorOverload", OOModel::Method::MethodKind::OperatorOverload);
}

void initClassModule() {
	Model::CompositeIndex (*Module_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::Module::registerNewAttribute;
	Model::CompositeIndex (*Module_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Module::registerNewAttribute;
	bool (OOModel::Module::*Module_isSubtypeOf1)(const QString&) const = &OOModel::Module::isSubtypeOf;
	bool (OOModel::Module::*Module_isSubtypeOf2)(int) const = &OOModel::Module::isSubtypeOf;
	auto aClass = class_<OOModel::Module, bases<OOModel::Declaration>>("Module");
		aClass.add_property("modules",
			make_function(&OOModel::Module::modules, return_internal_reference<>()),
			&OOModel::Module::setModules);
		aClass.add_property("classes",
			make_function(&OOModel::Module::classes, return_internal_reference<>()),
			&OOModel::Module::setClasses);
		aClass.add_property("methods",
			make_function(&OOModel::Module::methods, return_internal_reference<>()),
			&OOModel::Module::setMethods);
		aClass.add_property("fields",
			make_function(&OOModel::Module::fields, return_internal_reference<>()),
			&OOModel::Module::setFields);
		aClass.add_property("libraries",
			make_function(&OOModel::Module::libraries, return_internal_reference<>()),
			&OOModel::Module::setLibraries);
		aClass.add_property("kind",
			&OOModel::Module::kind,
			&OOModel::Module::setKind);
		aClass.def("typeName", make_function((const QString& (
			OOModel::Module::*)())&OOModel::Module::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Module::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Module::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&OOModel::Module::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Module::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Module::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Module::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Module::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::Module::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("symbolType", &OOModel::Module::symbolType);
		aClass.def("usedLibraries", &OOModel::Module::usedLibraries);
		aClass.def("isNewPersistenceUnit", &OOModel::Module::isNewPersistenceUnit);
		aClass.def("isTransparentForNameResolution", &OOModel::Module::isTransparentForNameResolution);
		aClass.def("registerNewAttribute", Module_registerNewAttribute1);
		aClass.def("registerNewAttribute", Module_registerNewAttribute2);
		aClass.def("isSubtypeOf", Module_isSubtypeOf1);
		aClass.def("isSubtypeOf", Module_isSubtypeOf2);
	scope ModuleScope = aClass;
	enum_<OOModel::Module::ModuleKind>("ModuleKind")
		.value("Standard", OOModel::Module::ModuleKind::Standard)
		.value("Folder", OOModel::Module::ModuleKind::Folder);
}

void initClassClass() {
	Model::CompositeIndex (*Class_registerNewAttribute1)(const Model::Attribute&) = &OOModel::Class::registerNewAttribute;
	Model::CompositeIndex (*Class_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Class::registerNewAttribute;
	bool (OOModel::Class::*Class_isSubtypeOf1)(const QString&) const = &OOModel::Class::isSubtypeOf;
	bool (OOModel::Class::*Class_isSubtypeOf2)(int) const = &OOModel::Class::isSubtypeOf;
	auto aClass = class_<OOModel::Class, bases<OOModel::Declaration>>("Class");
		aClass.add_property("baseClasses",
			make_function(&OOModel::Class::baseClasses, return_internal_reference<>()),
			&OOModel::Class::setBaseClasses);
		aClass.add_property("friends",
			make_function(&OOModel::Class::friends, return_internal_reference<>()),
			&OOModel::Class::setFriends);
		aClass.add_property("typeArguments",
			make_function(&OOModel::Class::typeArguments, return_internal_reference<>()),
			&OOModel::Class::setTypeArguments);
		aClass.add_property("classes",
			make_function(&OOModel::Class::classes, return_internal_reference<>()),
			&OOModel::Class::setClasses);
		aClass.add_property("methods",
			make_function(&OOModel::Class::methods, return_internal_reference<>()),
			&OOModel::Class::setMethods);
		aClass.add_property("fields",
			make_function(&OOModel::Class::fields, return_internal_reference<>()),
			&OOModel::Class::setFields);
		aClass.add_property("enumerators",
			make_function(&OOModel::Class::enumerators, return_internal_reference<>()),
			&OOModel::Class::setEnumerators);
		aClass.add_property("constructKind",
			&OOModel::Class::constructKind,
			&OOModel::Class::setConstructKind);
		aClass.def("typeName", make_function((const QString& (
			OOModel::Class::*)())&OOModel::Class::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Class::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Class::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&OOModel::Class::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Class::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Class::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Class::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Class::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::Class::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("symbolType", &OOModel::Class::symbolType);
		aClass.def("isGeneric", &OOModel::Class::isGeneric);
		aClass.def("allBaseClasses", &OOModel::Class::allBaseClasses);
		aClass.def("directBaseClasses", &OOModel::Class::directBaseClasses);
		aClass.def("implicitBaseFromProject", make_function(
			&OOModel::Class::implicitBaseFromProject, return_internal_reference<>()));
		aClass.def("expressionToClass", make_function(&OOModel::Class::expressionToClass, return_internal_reference<>()));
		aClass.staticmethod("expressionToClass");
		aClass.def("directSubClasses", &OOModel::Class::directSubClasses);
		aClass.def("isNewPersistenceUnit", &OOModel::Class::isNewPersistenceUnit);
		aClass.def("registerNewAttribute", Class_registerNewAttribute1);
		aClass.def("registerNewAttribute", Class_registerNewAttribute2);
		aClass.def("isSubtypeOf", Class_isSubtypeOf1);
		aClass.def("isSubtypeOf", Class_isSubtypeOf2);
	scope ClassScope = aClass;
	enum_<OOModel::Class::ConstructKind>("ConstructKind")
		.value("Class", OOModel::Class::ConstructKind::Class)
		.value("Interface", OOModel::Class::ConstructKind::Interface)
		.value("Struct", OOModel::Class::ConstructKind::Struct)
		.value("Union", OOModel::Class::ConstructKind::Union)
		.value("Enum", OOModel::Class::ConstructKind::Enum)
		.value("EnumClass", OOModel::Class::ConstructKind::EnumClass)
		.value("Annotation", OOModel::Class::ConstructKind::Annotation);
}

void initClassMetaDefinition() {
	bool (OOModel::MetaDefinition::*MetaDefinition_isSubtypeOf1)(const
		QString&) const = &OOModel::MetaDefinition::isSubtypeOf;
	bool (OOModel::MetaDefinition::*MetaDefinition_isSubtypeOf2)(int) const = &OOModel::MetaDefinition::isSubtypeOf;
	Model::CompositeIndex (*MetaDefinition_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MetaDefinition::registerNewAttribute;
	Model::CompositeIndex (*MetaDefinition_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MetaDefinition::registerNewAttribute;
	auto aClass = class_<OOModel::MetaDefinition, bases<OOModel::Declaration>>("MetaDefinition");
		aClass.add_property("arguments",
			make_function(&OOModel::MetaDefinition::arguments, return_internal_reference<>()),
			&OOModel::MetaDefinition::setArguments);
		aClass.add_property("metaBindings",
			make_function(&OOModel::MetaDefinition::metaBindings, return_internal_reference<>()),
			&OOModel::MetaDefinition::setMetaBindings);
		aClass.add_property("context",
			make_function(&OOModel::MetaDefinition::context, return_internal_reference<>()),
			&OOModel::MetaDefinition::setContext);
		aClass.def("typeName", make_function((const QString& (
			OOModel::MetaDefinition::*)())&OOModel::MetaDefinition::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::MetaDefinition::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::MetaDefinition::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::MetaDefinition::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::MetaDefinition::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::MetaDefinition::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::MetaDefinition::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::MetaDefinition::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::MetaDefinition::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("symbolType", &OOModel::MetaDefinition::symbolType);
		aClass.def("isSubtypeOf", MetaDefinition_isSubtypeOf1);
		aClass.def("isSubtypeOf", MetaDefinition_isSubtypeOf2);
		aClass.def("registerNewAttribute", MetaDefinition_registerNewAttribute1);
		aClass.def("registerNewAttribute", MetaDefinition_registerNewAttribute2);
}

void initClassTypeAlias() {
	Model::CompositeIndex (*TypeAlias_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::TypeAlias::registerNewAttribute;
	Model::CompositeIndex (*TypeAlias_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::TypeAlias::registerNewAttribute;
	bool (OOModel::TypeAlias::*TypeAlias_isSubtypeOf1)(const QString&) const = &OOModel::TypeAlias::isSubtypeOf;
	bool (OOModel::TypeAlias::*TypeAlias_isSubtypeOf2)(int) const = &OOModel::TypeAlias::isSubtypeOf;
	auto aClass = class_<OOModel::TypeAlias, bases<OOModel::Declaration>>("TypeAlias");
		aClass.add_property("typeExpression",
			make_function(&OOModel::TypeAlias::typeExpression, return_internal_reference<>()),
			&OOModel::TypeAlias::setTypeExpression);
		aClass.add_property("typeArguments",
			make_function(&OOModel::TypeAlias::typeArguments, return_internal_reference<>()),
			&OOModel::TypeAlias::setTypeArguments);
		aClass.def("typeName", make_function((const QString& (
			OOModel::TypeAlias::*)())&OOModel::TypeAlias::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::TypeAlias::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::TypeAlias::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&OOModel::TypeAlias::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::TypeAlias::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::TypeAlias::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::TypeAlias::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::TypeAlias::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::TypeAlias::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("symbolType", &OOModel::TypeAlias::symbolType);
		aClass.def("registerNewAttribute", TypeAlias_registerNewAttribute1);
		aClass.def("registerNewAttribute", TypeAlias_registerNewAttribute2);
		aClass.def("isSubtypeOf", TypeAlias_isSubtypeOf1);
		aClass.def("isSubtypeOf", TypeAlias_isSubtypeOf2);
}

void initClassFormalMetaArgument() {
	bool (OOModel::FormalMetaArgument::*FormalMetaArgument_isSubtypeOf1)(
		const QString&) const = &OOModel::FormalMetaArgument::isSubtypeOf;
	bool (OOModel::FormalMetaArgument::*FormalMetaArgument_isSubtypeOf2)(
		int) const = &OOModel::FormalMetaArgument::isSubtypeOf;
	Model::CompositeIndex (*FormalMetaArgument_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FormalMetaArgument::registerNewAttribute;
	Model::CompositeIndex (*FormalMetaArgument_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FormalMetaArgument::registerNewAttribute;
	auto aClass = class_<OOModel::FormalMetaArgument, bases<OOModel::Declaration>>("FormalMetaArgument");
		aClass.def("typeName", make_function((const QString& (
		OOModel::FormalMetaArgument::*)())&OOModel::FormalMetaArgument::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::FormalMetaArgument::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::FormalMetaArgument::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::FormalMetaArgument::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::FormalMetaArgument::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::FormalMetaArgument::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::FormalMetaArgument::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::FormalMetaArgument::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::FormalMetaArgument::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", FormalMetaArgument_isSubtypeOf1);
		aClass.def("isSubtypeOf", FormalMetaArgument_isSubtypeOf2);
		aClass.def("registerNewAttribute", FormalMetaArgument_registerNewAttribute1);
		aClass.def("registerNewAttribute", FormalMetaArgument_registerNewAttribute2);
}

void initClassCommentStatementItem() {
	bool (OOModel::CommentStatementItem::*CommentStatementItem_isSubtypeOf1)(
		const QString&) const = &OOModel::CommentStatementItem::isSubtypeOf;
	bool (OOModel::CommentStatementItem::*CommentStatementItem_isSubtypeOf2)(
		int) const = &OOModel::CommentStatementItem::isSubtypeOf;
	Model::CompositeIndex (*CommentStatementItem_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::CommentStatementItem::registerNewAttribute;
	Model::CompositeIndex (*CommentStatementItem_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::CommentStatementItem::registerNewAttribute;
	auto aClass = class_<OOModel::CommentStatementItem, bases<OOModel::StatementItem>>("CommentStatementItem");
		aClass.add_property("commentNode",
			make_function(&OOModel::CommentStatementItem::commentNode, return_internal_reference<>()),
			&OOModel::CommentStatementItem::setCommentNode);
		aClass.def("typeName", make_function((const QString& (
		OOModel::CommentStatementItem::*)())&OOModel::CommentStatementItem::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::CommentStatementItem::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::CommentStatementItem::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::CommentStatementItem::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::CommentStatementItem::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::CommentStatementItem::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::CommentStatementItem::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::CommentStatementItem::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::CommentStatementItem::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", CommentStatementItem_isSubtypeOf1);
		aClass.def("isSubtypeOf", CommentStatementItem_isSubtypeOf2);
		aClass.def("registerNewAttribute", CommentStatementItem_registerNewAttribute1);
		aClass.def("registerNewAttribute", CommentStatementItem_registerNewAttribute2);
}

void initClassStatement() {
	bool (OOModel::Statement::*Statement_isSubtypeOf1)(const QString&) const = &OOModel::Statement::isSubtypeOf;
	bool (OOModel::Statement::*Statement_isSubtypeOf2)(int) const = &OOModel::Statement::isSubtypeOf;
	Model::CompositeIndex (*Statement_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::Statement::registerNewAttribute;
	Model::CompositeIndex (*Statement_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Statement::registerNewAttribute;
	auto aClass = class_<OOModel::Statement, bases<OOModel::StatementItem>>("Statement");
		aClass.def("typeName", make_function((const QString& (
			OOModel::Statement::*)())&OOModel::Statement::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Statement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Statement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&OOModel::Statement::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Statement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Statement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Statement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Statement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::Statement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", Statement_isSubtypeOf1);
		aClass.def("isSubtypeOf", Statement_isSubtypeOf2);
		aClass.def("registerNewAttribute", Statement_registerNewAttribute1);
		aClass.def("registerNewAttribute", Statement_registerNewAttribute2);
}

void initClassStatementItemList() {
	bool (OOModel::StatementItemList::*StatementItemList_isSubtypeOf1)(
		const QString&) const = &OOModel::StatementItemList::isSubtypeOf;
	bool (OOModel::StatementItemList::*StatementItemList_isSubtypeOf2)(
		int) const = &OOModel::StatementItemList::isSubtypeOf;
	auto aClass = class_<OOModel::StatementItemList, bases<Model::TypedList<OOModel::StatementItem>>>("StatementItemList");
		aClass.def("typeName", make_function((const QString& (
		OOModel::StatementItemList::*)())&OOModel::StatementItemList::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::StatementItemList::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::StatementItemList::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::StatementItemList::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::StatementItemList::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::StatementItemList::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::StatementItemList::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::StatementItemList::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("isSubtypeOf", StatementItemList_isSubtypeOf1);
		aClass.def("isSubtypeOf", StatementItemList_isSubtypeOf2);
}

void initClassFunctionTypeExpression() {
	bool (OOModel::FunctionTypeExpression::*FunctionTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::FunctionTypeExpression::isSubtypeOf;
	bool (OOModel::FunctionTypeExpression::*FunctionTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::FunctionTypeExpression::isSubtypeOf;
	Model::CompositeIndex (*FunctionTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FunctionTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*FunctionTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FunctionTypeExpression::registerNewAttribute;
	auto aClass = class_<OOModel::FunctionTypeExpression, bases<OOModel::TypeExpression>>("FunctionTypeExpression");
		aClass.add_property("arguments",
			make_function(&OOModel::FunctionTypeExpression::arguments, return_internal_reference<>()),
			&OOModel::FunctionTypeExpression::setArguments);
		aClass.add_property("results",
			make_function(&OOModel::FunctionTypeExpression::results, return_internal_reference<>()),
			&OOModel::FunctionTypeExpression::setResults);
		aClass.def("typeName", make_function((const QString& (
		OOModel::FunctionTypeExpression::*)())&OOModel::FunctionTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::FunctionTypeExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::FunctionTypeExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::FunctionTypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::FunctionTypeExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::FunctionTypeExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::FunctionTypeExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::FunctionTypeExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(
			&OOModel::FunctionTypeExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::FunctionTypeExpression::type);
		aClass.def("isSubtypeOf", FunctionTypeExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", FunctionTypeExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", FunctionTypeExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", FunctionTypeExpression_registerNewAttribute2);
}

void initClassTypeQualifierExpression() {
	Model::CompositeIndex (*TypeQualifierExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::TypeQualifierExpression::registerNewAttribute;
	Model::CompositeIndex (*TypeQualifierExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::TypeQualifierExpression::registerNewAttribute;
	bool (OOModel::TypeQualifierExpression::*TypeQualifierExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::TypeQualifierExpression::isSubtypeOf;
	bool (OOModel::TypeQualifierExpression::*TypeQualifierExpression_isSubtypeOf2)(
		int) const = &OOModel::TypeQualifierExpression::isSubtypeOf;
	auto aClass = class_<OOModel::TypeQualifierExpression, bases<OOModel::TypeExpression>>("TypeQualifierExpression");
		aClass.add_property("typeExpression",
			make_function(&OOModel::TypeQualifierExpression::typeExpression, return_internal_reference<>()),
			&OOModel::TypeQualifierExpression::setTypeExpression);
		aClass.add_property("qualifier",
			&OOModel::TypeQualifierExpression::qualifier,
			&OOModel::TypeQualifierExpression::setQualifier);
		aClass.def("typeName", make_function((const QString& (
		OOModel::TypeQualifierExpression::*)())&OOModel::TypeQualifierExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::TypeQualifierExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::TypeQualifierExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::TypeQualifierExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::TypeQualifierExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::TypeQualifierExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::TypeQualifierExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::TypeQualifierExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(
			&OOModel::TypeQualifierExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::TypeQualifierExpression::type);
		aClass.def("registerNewAttribute", TypeQualifierExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", TypeQualifierExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", TypeQualifierExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", TypeQualifierExpression_isSubtypeOf2);
}

void initClassArrayTypeExpression() {
	bool (OOModel::ArrayTypeExpression::*ArrayTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ArrayTypeExpression::isSubtypeOf;
	bool (OOModel::ArrayTypeExpression::*ArrayTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::ArrayTypeExpression::isSubtypeOf;
	Model::CompositeIndex (*ArrayTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ArrayTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*ArrayTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ArrayTypeExpression::registerNewAttribute;
	auto aClass = class_<OOModel::ArrayTypeExpression, bases<OOModel::TypeExpression>>("ArrayTypeExpression");
		aClass.add_property("typeExpression",
			make_function(&OOModel::ArrayTypeExpression::typeExpression, return_internal_reference<>()),
			&OOModel::ArrayTypeExpression::setTypeExpression);
		aClass.add_property("fixedSize",
			make_function(&OOModel::ArrayTypeExpression::fixedSize, return_internal_reference<>()),
			&OOModel::ArrayTypeExpression::setFixedSize);
		aClass.def("typeName", make_function((const QString& (
		OOModel::ArrayTypeExpression::*)())&OOModel::ArrayTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ArrayTypeExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ArrayTypeExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ArrayTypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ArrayTypeExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ArrayTypeExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ArrayTypeExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ArrayTypeExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ArrayTypeExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::ArrayTypeExpression::type);
		aClass.def("isSubtypeOf", ArrayTypeExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", ArrayTypeExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", ArrayTypeExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", ArrayTypeExpression_registerNewAttribute2);
}

void initClassAutoTypeExpression() {
	bool (OOModel::AutoTypeExpression::*AutoTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::AutoTypeExpression::isSubtypeOf;
	bool (OOModel::AutoTypeExpression::*AutoTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::AutoTypeExpression::isSubtypeOf;
	Model::CompositeIndex (*AutoTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::AutoTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*AutoTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::AutoTypeExpression::registerNewAttribute;
	auto aClass = class_<OOModel::AutoTypeExpression, bases<OOModel::TypeExpression>>("AutoTypeExpression");
		aClass.def("typeName", make_function((const QString& (
		OOModel::AutoTypeExpression::*)())&OOModel::AutoTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::AutoTypeExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::AutoTypeExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::AutoTypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::AutoTypeExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::AutoTypeExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::AutoTypeExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::AutoTypeExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::AutoTypeExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::AutoTypeExpression::type);
		aClass.def("isSubtypeOf", AutoTypeExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", AutoTypeExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", AutoTypeExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", AutoTypeExpression_registerNewAttribute2);
}

void initClassReferenceTypeExpression() {
	Model::CompositeIndex (*ReferenceTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ReferenceTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*ReferenceTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ReferenceTypeExpression::registerNewAttribute;
	bool (OOModel::ReferenceTypeExpression::*ReferenceTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ReferenceTypeExpression::isSubtypeOf;
	bool (OOModel::ReferenceTypeExpression::*ReferenceTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::ReferenceTypeExpression::isSubtypeOf;
	auto aClass = class_<OOModel::ReferenceTypeExpression, bases<OOModel::TypeExpression>>("ReferenceTypeExpression");
		aClass.add_property("typeExpression",
			make_function(&OOModel::ReferenceTypeExpression::typeExpression, return_internal_reference<>()),
			&OOModel::ReferenceTypeExpression::setTypeExpression);
		aClass.add_property("isRValueReference",
			&OOModel::ReferenceTypeExpression::isRValueReference,
			&OOModel::ReferenceTypeExpression::setIsRValueReference);
		aClass.def("typeName", make_function((const QString& (
		OOModel::ReferenceTypeExpression::*)())&OOModel::ReferenceTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ReferenceTypeExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ReferenceTypeExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ReferenceTypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ReferenceTypeExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ReferenceTypeExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ReferenceTypeExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ReferenceTypeExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(
			&OOModel::ReferenceTypeExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isRValueReferenceNode", make_function(
			&OOModel::ReferenceTypeExpression::isRValueReferenceNode, return_internal_reference<>()));
		aClass.def("type", &OOModel::ReferenceTypeExpression::type);
		aClass.def("registerNewAttribute", ReferenceTypeExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", ReferenceTypeExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", ReferenceTypeExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", ReferenceTypeExpression_isSubtypeOf2);
}

void initClassPrimitiveTypeExpression() {
	Model::CompositeIndex (*PrimitiveTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::PrimitiveTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*PrimitiveTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::PrimitiveTypeExpression::registerNewAttribute;
	bool (OOModel::PrimitiveTypeExpression::*PrimitiveTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::PrimitiveTypeExpression::isSubtypeOf;
	bool (OOModel::PrimitiveTypeExpression::*PrimitiveTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::PrimitiveTypeExpression::isSubtypeOf;
	auto aClass = class_<OOModel::PrimitiveTypeExpression, bases<OOModel::TypeExpression>>("PrimitiveTypeExpression");
		aClass.add_property("typeValue",
			&OOModel::PrimitiveTypeExpression::typeValue,
			&OOModel::PrimitiveTypeExpression::setTypeValue);
		aClass.def("typeName", make_function((const QString& (
		OOModel::PrimitiveTypeExpression::*)())&OOModel::PrimitiveTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::PrimitiveTypeExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::PrimitiveTypeExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::PrimitiveTypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::PrimitiveTypeExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::PrimitiveTypeExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::PrimitiveTypeExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::PrimitiveTypeExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(
			&OOModel::PrimitiveTypeExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::PrimitiveTypeExpression::type);
		aClass.def("registerNewAttribute", PrimitiveTypeExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", PrimitiveTypeExpression_registerNewAttribute2);
		aClass.def("isSubtypeOf", PrimitiveTypeExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", PrimitiveTypeExpression_isSubtypeOf2);
}

void initClassClassTypeExpression() {
	bool (OOModel::ClassTypeExpression::*ClassTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ClassTypeExpression::isSubtypeOf;
	bool (OOModel::ClassTypeExpression::*ClassTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::ClassTypeExpression::isSubtypeOf;
	Model::CompositeIndex (*ClassTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ClassTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*ClassTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ClassTypeExpression::registerNewAttribute;
	auto aClass = class_<OOModel::ClassTypeExpression, bases<OOModel::TypeExpression>>("ClassTypeExpression");
		aClass.add_property("typeExpression",
			make_function(&OOModel::ClassTypeExpression::typeExpression, return_internal_reference<>()),
			&OOModel::ClassTypeExpression::setTypeExpression);
		aClass.def("typeName", make_function((const QString& (
		OOModel::ClassTypeExpression::*)())&OOModel::ClassTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ClassTypeExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ClassTypeExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ClassTypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ClassTypeExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ClassTypeExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ClassTypeExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ClassTypeExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ClassTypeExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::ClassTypeExpression::type);
		aClass.def("isSubtypeOf", ClassTypeExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", ClassTypeExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", ClassTypeExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", ClassTypeExpression_registerNewAttribute2);
}

void initClassPointerTypeExpression() {
	bool (OOModel::PointerTypeExpression::*PointerTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::PointerTypeExpression::isSubtypeOf;
	bool (OOModel::PointerTypeExpression::*PointerTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::PointerTypeExpression::isSubtypeOf;
	Model::CompositeIndex (*PointerTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::PointerTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*PointerTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::PointerTypeExpression::registerNewAttribute;
	auto aClass = class_<OOModel::PointerTypeExpression, bases<OOModel::TypeExpression>>("PointerTypeExpression");
		aClass.add_property("typeExpression",
			make_function(&OOModel::PointerTypeExpression::typeExpression, return_internal_reference<>()),
			&OOModel::PointerTypeExpression::setTypeExpression);
		aClass.def("typeName", make_function((const QString& (
		OOModel::PointerTypeExpression::*)())&OOModel::PointerTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::PointerTypeExpression::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::PointerTypeExpression::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::PointerTypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::PointerTypeExpression::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::PointerTypeExpression::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::PointerTypeExpression::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::PointerTypeExpression::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::PointerTypeExpression::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("type", &OOModel::PointerTypeExpression::type);
		aClass.def("isSubtypeOf", PointerTypeExpression_isSubtypeOf1);
		aClass.def("isSubtypeOf", PointerTypeExpression_isSubtypeOf2);
		aClass.def("registerNewAttribute", PointerTypeExpression_registerNewAttribute1);
		aClass.def("registerNewAttribute", PointerTypeExpression_registerNewAttribute2);
}

void initClassField() {
	bool (OOModel::Field::*Field_isSubtypeOf1)(const QString&) const = &OOModel::Field::isSubtypeOf;
	bool (OOModel::Field::*Field_isSubtypeOf2)(int) const = &OOModel::Field::isSubtypeOf;
	Model::CompositeIndex (*Field_registerNewAttribute1)(const Model::Attribute&) = &OOModel::Field::registerNewAttribute;
	Model::CompositeIndex (*Field_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Field::registerNewAttribute;
	auto aClass = class_<OOModel::Field, bases<OOModel::VariableDeclaration>>("Field");
		aClass.def("typeName", make_function((const QString& (
			OOModel::Field::*)())&OOModel::Field::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Field::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Field::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&OOModel::Field::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Field::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Field::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Field::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Field::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::Field::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("symbolType", &OOModel::Field::symbolType);
		aClass.def("isSubtypeOf", Field_isSubtypeOf1);
		aClass.def("isSubtypeOf", Field_isSubtypeOf2);
		aClass.def("registerNewAttribute", Field_registerNewAttribute1);
		aClass.def("registerNewAttribute", Field_registerNewAttribute2);
}

void initClassFormalArgument() {
	Model::CompositeIndex (*FormalArgument_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FormalArgument::registerNewAttribute;
	Model::CompositeIndex (*FormalArgument_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FormalArgument::registerNewAttribute;
	bool (OOModel::FormalArgument::*FormalArgument_isSubtypeOf1)(const
		QString&) const = &OOModel::FormalArgument::isSubtypeOf;
	bool (OOModel::FormalArgument::*FormalArgument_isSubtypeOf2)(int) const = &OOModel::FormalArgument::isSubtypeOf;
	auto aClass = class_<OOModel::FormalArgument, bases<OOModel::VariableDeclaration>>("FormalArgument");
		aClass.add_property("direction",
			&OOModel::FormalArgument::direction,
			&OOModel::FormalArgument::setDirection);
		aClass.def("typeName", make_function((const QString& (
			OOModel::FormalArgument::*)())&OOModel::FormalArgument::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::FormalArgument::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::FormalArgument::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::FormalArgument::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::FormalArgument::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::FormalArgument::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::FormalArgument::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::FormalArgument::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::FormalArgument::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isUsedInParentMethod", &OOModel::FormalArgument::isUsedInParentMethod);
		aClass.def("registerNewAttribute", FormalArgument_registerNewAttribute1);
		aClass.def("registerNewAttribute", FormalArgument_registerNewAttribute2);
		aClass.def("isSubtypeOf", FormalArgument_isSubtypeOf1);
		aClass.def("isSubtypeOf", FormalArgument_isSubtypeOf2);
	scope FormalArgumentScope = aClass;
	enum_<OOModel::FormalArgument::Direction>("Direction")
		.value("IN", OOModel::FormalArgument::Direction::IN)
		.value("OUT", OOModel::FormalArgument::Direction::OUT)
		.value("INOUT", OOModel::FormalArgument::Direction::INOUT);
}

void initClassSynchronizedStatement() {
	bool (OOModel::SynchronizedStatement::*SynchronizedStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::SynchronizedStatement::isSubtypeOf;
	bool (OOModel::SynchronizedStatement::*SynchronizedStatement_isSubtypeOf2)(
		int) const = &OOModel::SynchronizedStatement::isSubtypeOf;
	Model::CompositeIndex (*SynchronizedStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::SynchronizedStatement::registerNewAttribute;
	Model::CompositeIndex (*SynchronizedStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::SynchronizedStatement::registerNewAttribute;
	auto aClass = class_<OOModel::SynchronizedStatement, bases<OOModel::Statement>>("SynchronizedStatement");
		aClass.add_property("expression",
			make_function(&OOModel::SynchronizedStatement::expression, return_internal_reference<>()),
			&OOModel::SynchronizedStatement::setExpression);
		aClass.add_property("body",
			make_function(&OOModel::SynchronizedStatement::body, return_internal_reference<>()),
			&OOModel::SynchronizedStatement::setBody);
		aClass.def("typeName", make_function((const QString& (
		OOModel::SynchronizedStatement::*)())&OOModel::SynchronizedStatement::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::SynchronizedStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::SynchronizedStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::SynchronizedStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::SynchronizedStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::SynchronizedStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::SynchronizedStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::SynchronizedStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::SynchronizedStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", SynchronizedStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", SynchronizedStatement_isSubtypeOf2);
		aClass.def("registerNewAttribute", SynchronizedStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", SynchronizedStatement_registerNewAttribute2);
}

void initClassIfStatement() {
	bool (OOModel::IfStatement::*IfStatement_isSubtypeOf1)(const QString&) const = &OOModel::IfStatement::isSubtypeOf;
	bool (OOModel::IfStatement::*IfStatement_isSubtypeOf2)(int) const = &OOModel::IfStatement::isSubtypeOf;
	Model::CompositeIndex (*IfStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::IfStatement::registerNewAttribute;
	Model::CompositeIndex (*IfStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::IfStatement::registerNewAttribute;
	auto aClass = class_<OOModel::IfStatement, bases<OOModel::Statement>>("IfStatement");
		aClass.add_property("condition",
			make_function(&OOModel::IfStatement::condition, return_internal_reference<>()),
			&OOModel::IfStatement::setCondition);
		aClass.add_property("thenBranch",
			make_function(&OOModel::IfStatement::thenBranch, return_internal_reference<>()),
			&OOModel::IfStatement::setThenBranch);
		aClass.add_property("elseBranch",
			make_function(&OOModel::IfStatement::elseBranch, return_internal_reference<>()),
			&OOModel::IfStatement::setElseBranch);
		aClass.def("typeName", make_function((const QString& (
			OOModel::IfStatement::*)())&OOModel::IfStatement::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::IfStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::IfStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::IfStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::IfStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::IfStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::IfStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::IfStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::IfStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", IfStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", IfStatement_isSubtypeOf2);
		aClass.def("registerNewAttribute", IfStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", IfStatement_registerNewAttribute2);
}

void initClassReturnStatement() {
	bool (OOModel::ReturnStatement::*ReturnStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::ReturnStatement::isSubtypeOf;
	bool (OOModel::ReturnStatement::*ReturnStatement_isSubtypeOf2)(int) const = &OOModel::ReturnStatement::isSubtypeOf;
	Model::CompositeIndex (*ReturnStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ReturnStatement::registerNewAttribute;
	Model::CompositeIndex (*ReturnStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ReturnStatement::registerNewAttribute;
	auto aClass = class_<OOModel::ReturnStatement, bases<OOModel::Statement>>("ReturnStatement");
		aClass.add_property("values",
			make_function(&OOModel::ReturnStatement::values, return_internal_reference<>()),
			&OOModel::ReturnStatement::setValues);
		aClass.def("typeName", make_function((const QString& (
			OOModel::ReturnStatement::*)())&OOModel::ReturnStatement::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ReturnStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ReturnStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ReturnStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ReturnStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ReturnStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ReturnStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ReturnStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ReturnStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", ReturnStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", ReturnStatement_isSubtypeOf2);
		aClass.def("registerNewAttribute", ReturnStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", ReturnStatement_registerNewAttribute2);
}

void initClassSwitchStatement() {
	bool (OOModel::SwitchStatement::*SwitchStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::SwitchStatement::isSubtypeOf;
	bool (OOModel::SwitchStatement::*SwitchStatement_isSubtypeOf2)(int) const = &OOModel::SwitchStatement::isSubtypeOf;
	Model::CompositeIndex (*SwitchStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::SwitchStatement::registerNewAttribute;
	Model::CompositeIndex (*SwitchStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::SwitchStatement::registerNewAttribute;
	auto aClass = class_<OOModel::SwitchStatement, bases<OOModel::Statement>>("SwitchStatement");
		aClass.add_property("switchExpression",
			make_function(&OOModel::SwitchStatement::switchExpression, return_internal_reference<>()),
			&OOModel::SwitchStatement::setSwitchExpression);
		aClass.add_property("body",
			make_function(&OOModel::SwitchStatement::body, return_internal_reference<>()),
			&OOModel::SwitchStatement::setBody);
		aClass.def("typeName", make_function((const QString& (
			OOModel::SwitchStatement::*)())&OOModel::SwitchStatement::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::SwitchStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::SwitchStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::SwitchStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::SwitchStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::SwitchStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::SwitchStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::SwitchStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::SwitchStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", SwitchStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", SwitchStatement_isSubtypeOf2);
		aClass.def("registerNewAttribute", SwitchStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", SwitchStatement_registerNewAttribute2);
}

void initClassCaseStatement() {
	bool (OOModel::CaseStatement::*CaseStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::CaseStatement::isSubtypeOf;
	bool (OOModel::CaseStatement::*CaseStatement_isSubtypeOf2)(int) const = &OOModel::CaseStatement::isSubtypeOf;
	Model::CompositeIndex (*CaseStatement_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::CaseStatement::registerNewAttribute;
	Model::CompositeIndex (*CaseStatement_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::CaseStatement::registerNewAttribute;
	auto aClass = class_<OOModel::CaseStatement, bases<OOModel::Statement>>("CaseStatement");
		aClass.add_property("caseExpression",
			make_function(&OOModel::CaseStatement::caseExpression, return_internal_reference<>()),
			&OOModel::CaseStatement::setCaseExpression);
		aClass.add_property("body",
			make_function(&OOModel::CaseStatement::body, return_internal_reference<>()),
			&OOModel::CaseStatement::setBody);
		aClass.def("typeName", make_function((const QString& (
			OOModel::CaseStatement::*)())&OOModel::CaseStatement::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::CaseStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::CaseStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::CaseStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::CaseStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::CaseStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::CaseStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::CaseStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::CaseStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", CaseStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", CaseStatement_isSubtypeOf2);
		aClass.def("registerNewAttribute", CaseStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", CaseStatement_registerNewAttribute2);
}

void initClassBreakStatement() {
	bool (OOModel::BreakStatement::*BreakStatement_isSubtypeOf1)(const
		QString&) const = &OOModel::BreakStatement::isSubtypeOf;
	bool (OOModel::BreakStatement::*BreakStatement_isSubtypeOf2)(int) const = &OOModel::BreakStatement::isSubtypeOf;
	Model::CompositeIndex (*BreakStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::BreakStatement::registerNewAttribute;
	Model::CompositeIndex (*BreakStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::BreakStatement::registerNewAttribute;
	auto aClass = class_<OOModel::BreakStatement, bases<OOModel::Statement>>("BreakStatement");
		aClass.def("typeName", make_function((const QString& (
			OOModel::BreakStatement::*)())&OOModel::BreakStatement::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::BreakStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::BreakStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::BreakStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::BreakStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::BreakStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::BreakStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::BreakStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::BreakStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", BreakStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", BreakStatement_isSubtypeOf2);
		aClass.def("registerNewAttribute", BreakStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", BreakStatement_registerNewAttribute2);
}

void initClassTryCatchFinallyStatement() {
	bool (OOModel::TryCatchFinallyStatement::*TryCatchFinallyStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::TryCatchFinallyStatement::isSubtypeOf;
	bool (OOModel::TryCatchFinallyStatement::*TryCatchFinallyStatement_isSubtypeOf2)(
		int) const = &OOModel::TryCatchFinallyStatement::isSubtypeOf;
	Model::CompositeIndex (*TryCatchFinallyStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::TryCatchFinallyStatement::registerNewAttribute;
	Model::CompositeIndex (*TryCatchFinallyStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::TryCatchFinallyStatement::registerNewAttribute;
	auto aClass = class_<OOModel::TryCatchFinallyStatement, bases<OOModel::Statement>>("TryCatchFinallyStatement");
		aClass.add_property("tryBody",
			make_function(&OOModel::TryCatchFinallyStatement::tryBody, return_internal_reference<>()),
			&OOModel::TryCatchFinallyStatement::setTryBody);
		aClass.add_property("catchClauses",
			make_function(&OOModel::TryCatchFinallyStatement::catchClauses, return_internal_reference<>()),
			&OOModel::TryCatchFinallyStatement::setCatchClauses);
		aClass.add_property("finallyBody",
			make_function(&OOModel::TryCatchFinallyStatement::finallyBody, return_internal_reference<>()),
			&OOModel::TryCatchFinallyStatement::setFinallyBody);
		aClass.def("typeName", make_function((const QString& (
		OOModel::TryCatchFinallyStatement::*)(
			))&OOModel::TryCatchFinallyStatement::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::TryCatchFinallyStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::TryCatchFinallyStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::TryCatchFinallyStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::TryCatchFinallyStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::TryCatchFinallyStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::TryCatchFinallyStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::TryCatchFinallyStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(
			&OOModel::TryCatchFinallyStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", TryCatchFinallyStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", TryCatchFinallyStatement_isSubtypeOf2);
		aClass.def("registerNewAttribute", TryCatchFinallyStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", TryCatchFinallyStatement_registerNewAttribute2);
}

void initClassDeclarationStatement() {
	Model::CompositeIndex (*DeclarationStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::DeclarationStatement::registerNewAttribute;
	Model::CompositeIndex (*DeclarationStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::DeclarationStatement::registerNewAttribute;
	bool (OOModel::DeclarationStatement::*DeclarationStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::DeclarationStatement::isSubtypeOf;
	bool (OOModel::DeclarationStatement::*DeclarationStatement_isSubtypeOf2)(
		int) const = &OOModel::DeclarationStatement::isSubtypeOf;
	auto aClass = class_<OOModel::DeclarationStatement, bases<OOModel::Statement>>("DeclarationStatement");
		aClass.add_property("declaration",
			make_function(&OOModel::DeclarationStatement::declaration, return_internal_reference<>()),
			&OOModel::DeclarationStatement::setDeclaration);
		aClass.def("typeName", make_function((const QString& (
		OOModel::DeclarationStatement::*)())&OOModel::DeclarationStatement::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::DeclarationStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::DeclarationStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::DeclarationStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::DeclarationStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::DeclarationStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::DeclarationStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::DeclarationStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::DeclarationStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("definesSymbol", &OOModel::DeclarationStatement::definesSymbol);
		aClass.def("symbolName", make_function((const QString& (
		OOModel::DeclarationStatement::*)())&OOModel::DeclarationStatement::symbolName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("symbolType", &OOModel::DeclarationStatement::symbolType);
		aClass.def("registerNewAttribute", DeclarationStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", DeclarationStatement_registerNewAttribute2);
		aClass.def("isSubtypeOf", DeclarationStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", DeclarationStatement_isSubtypeOf2);
}

void initClassForEachStatement() {
	Model::CompositeIndex (*ForEachStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ForEachStatement::registerNewAttribute;
	Model::CompositeIndex (*ForEachStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ForEachStatement::registerNewAttribute;
	bool (OOModel::ForEachStatement::*ForEachStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::ForEachStatement::isSubtypeOf;
	bool (OOModel::ForEachStatement::*ForEachStatement_isSubtypeOf2)(int) const = &OOModel::ForEachStatement::isSubtypeOf;
	auto aClass = class_<OOModel::ForEachStatement, bases<OOModel::Statement>>("ForEachStatement");
		aClass.add_property("varName",
			make_function((const QString& (
				OOModel::ForEachStatement::*)())&OOModel::ForEachStatement::varName, return_value_policy<copy_const_reference>()),
			&OOModel::ForEachStatement::setVarName);
		aClass.add_property("varType",
			make_function(&OOModel::ForEachStatement::varType, return_internal_reference<>()),
			&OOModel::ForEachStatement::setVarType);
		aClass.add_property("collection",
			make_function(&OOModel::ForEachStatement::collection, return_internal_reference<>()),
			&OOModel::ForEachStatement::setCollection);
		aClass.add_property("body",
			make_function(&OOModel::ForEachStatement::body, return_internal_reference<>()),
			&OOModel::ForEachStatement::setBody);
		aClass.def("typeName", make_function((const QString& (
			OOModel::ForEachStatement::*)())&OOModel::ForEachStatement::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ForEachStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ForEachStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ForEachStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ForEachStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ForEachStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ForEachStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ForEachStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ForEachStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("varNameNode", make_function(&OOModel::ForEachStatement::varNameNode, return_internal_reference<>()));
		aClass.def("definesSymbol", &OOModel::ForEachStatement::definesSymbol);
		aClass.def("symbolName", make_function((const QString& (
		OOModel::ForEachStatement::*)())&OOModel::ForEachStatement::symbolName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("symbolType", &OOModel::ForEachStatement::symbolType);
		aClass.def("registerNewAttribute", ForEachStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", ForEachStatement_registerNewAttribute2);
		aClass.def("isSubtypeOf", ForEachStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", ForEachStatement_isSubtypeOf2);
}

void initClassAssertStatement() {
	Model::CompositeIndex (*AssertStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::AssertStatement::registerNewAttribute;
	Model::CompositeIndex (*AssertStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::AssertStatement::registerNewAttribute;
	bool (OOModel::AssertStatement::*AssertStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::AssertStatement::isSubtypeOf;
	bool (OOModel::AssertStatement::*AssertStatement_isSubtypeOf2)(int) const = &OOModel::AssertStatement::isSubtypeOf;
	auto aClass = class_<OOModel::AssertStatement, bases<OOModel::Statement>>("AssertStatement");
		aClass.add_property("expression",
			make_function(&OOModel::AssertStatement::expression, return_internal_reference<>()),
			&OOModel::AssertStatement::setExpression);
		aClass.add_property("assertKind",
			&OOModel::AssertStatement::assertKind,
			&OOModel::AssertStatement::setAssertKind);
		aClass.def("typeName", make_function((const QString& (
			OOModel::AssertStatement::*)())&OOModel::AssertStatement::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::AssertStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::AssertStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::AssertStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::AssertStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::AssertStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::AssertStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::AssertStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::AssertStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("registerNewAttribute", AssertStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", AssertStatement_registerNewAttribute2);
		aClass.def("isSubtypeOf", AssertStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", AssertStatement_isSubtypeOf2);
	scope AssertStatementScope = aClass;
	enum_<OOModel::AssertStatement::AssertKind>("AssertKind")
		.value("Runtime", OOModel::AssertStatement::AssertKind::Runtime)
		.value("Static", OOModel::AssertStatement::AssertKind::Static);
}

void initClassExpressionStatement() {
	Model::CompositeIndex (*ExpressionStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ExpressionStatement::registerNewAttribute;
	Model::CompositeIndex (*ExpressionStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ExpressionStatement::registerNewAttribute;
	bool (OOModel::ExpressionStatement::*ExpressionStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::ExpressionStatement::isSubtypeOf;
	bool (OOModel::ExpressionStatement::*ExpressionStatement_isSubtypeOf2)(
		int) const = &OOModel::ExpressionStatement::isSubtypeOf;
	auto aClass = class_<OOModel::ExpressionStatement, bases<OOModel::Statement>>("ExpressionStatement");
		aClass.add_property("expression",
			make_function(&OOModel::ExpressionStatement::expression, return_internal_reference<>()),
			&OOModel::ExpressionStatement::setExpression);
		aClass.def("typeName", make_function((const QString& (
		OOModel::ExpressionStatement::*)())&OOModel::ExpressionStatement::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ExpressionStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ExpressionStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ExpressionStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ExpressionStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ExpressionStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ExpressionStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ExpressionStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ExpressionStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("definesSymbol", &OOModel::ExpressionStatement::definesSymbol);
		aClass.def("symbolName", make_function((const QString& (
		OOModel::ExpressionStatement::*)())&OOModel::ExpressionStatement::symbolName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("symbolType", &OOModel::ExpressionStatement::symbolType);
		aClass.def("registerNewAttribute", ExpressionStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", ExpressionStatement_registerNewAttribute2);
		aClass.def("isSubtypeOf", ExpressionStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", ExpressionStatement_isSubtypeOf2);
}

void initClassContinueStatement() {
	bool (OOModel::ContinueStatement::*ContinueStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::ContinueStatement::isSubtypeOf;
	bool (OOModel::ContinueStatement::*ContinueStatement_isSubtypeOf2)(
		int) const = &OOModel::ContinueStatement::isSubtypeOf;
	Model::CompositeIndex (*ContinueStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ContinueStatement::registerNewAttribute;
	Model::CompositeIndex (*ContinueStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ContinueStatement::registerNewAttribute;
	auto aClass = class_<OOModel::ContinueStatement, bases<OOModel::Statement>>("ContinueStatement");
		aClass.def("typeName", make_function((const QString& (
		OOModel::ContinueStatement::*)())&OOModel::ContinueStatement::typeName,
			 return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::ContinueStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::ContinueStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::ContinueStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::ContinueStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::ContinueStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::ContinueStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::ContinueStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::ContinueStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", ContinueStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", ContinueStatement_isSubtypeOf2);
		aClass.def("registerNewAttribute", ContinueStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", ContinueStatement_registerNewAttribute2);
}

void initClassLoopStatement() {
	Model::CompositeIndex (*LoopStatement_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::LoopStatement::registerNewAttribute;
	Model::CompositeIndex (*LoopStatement_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::LoopStatement::registerNewAttribute;
	bool (OOModel::LoopStatement::*LoopStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::LoopStatement::isSubtypeOf;
	bool (OOModel::LoopStatement::*LoopStatement_isSubtypeOf2)(int) const = &OOModel::LoopStatement::isSubtypeOf;
	auto aClass = class_<OOModel::LoopStatement, bases<OOModel::Statement>>("LoopStatement");
		aClass.add_property("condition",
			make_function(&OOModel::LoopStatement::condition, return_internal_reference<>()),
			&OOModel::LoopStatement::setCondition);
		aClass.add_property("initStep",
			make_function(&OOModel::LoopStatement::initStep, return_internal_reference<>()),
			&OOModel::LoopStatement::setInitStep);
		aClass.add_property("updateStep",
			make_function(&OOModel::LoopStatement::updateStep, return_internal_reference<>()),
			&OOModel::LoopStatement::setUpdateStep);
		aClass.add_property("body",
			make_function(&OOModel::LoopStatement::body, return_internal_reference<>()),
			&OOModel::LoopStatement::setBody);
		aClass.add_property("loopKind",
			&OOModel::LoopStatement::loopKind,
			&OOModel::LoopStatement::setLoopKind);
		aClass.def("typeName", make_function((const QString& (
			OOModel::LoopStatement::*)())&OOModel::LoopStatement::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::LoopStatement::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::LoopStatement::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(
			&OOModel::LoopStatement::typeNameStatic, return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::LoopStatement::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::LoopStatement::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::LoopStatement::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::LoopStatement::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::LoopStatement::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("registerNewAttribute", LoopStatement_registerNewAttribute1);
		aClass.def("registerNewAttribute", LoopStatement_registerNewAttribute2);
		aClass.def("isSubtypeOf", LoopStatement_isSubtypeOf1);
		aClass.def("isSubtypeOf", LoopStatement_isSubtypeOf2);
	scope LoopStatementScope = aClass;
	enum_<OOModel::LoopStatement::LoopKind>("LoopKind")
		.value("PreCheck", OOModel::LoopStatement::LoopKind::PreCheck)
		.value("PostCheck", OOModel::LoopStatement::LoopKind::PostCheck);
}

void initClassBlock() {
	bool (OOModel::Block::*Block_isSubtypeOf1)(const QString&) const = &OOModel::Block::isSubtypeOf;
	bool (OOModel::Block::*Block_isSubtypeOf2)(int) const = &OOModel::Block::isSubtypeOf;
	Model::CompositeIndex (*Block_registerNewAttribute1)(const Model::Attribute&) = &OOModel::Block::registerNewAttribute;
	Model::CompositeIndex (*Block_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Block::registerNewAttribute;
	auto aClass = class_<OOModel::Block, bases<OOModel::Statement>>("Block");
		aClass.add_property("items",
			make_function(&OOModel::Block::items, return_internal_reference<>()),
			&OOModel::Block::setItems);
		aClass.def("typeName", make_function((const QString& (
			OOModel::Block::*)())&OOModel::Block::typeName, return_value_policy<copy_const_reference>()));
		aClass.def("typeId", &OOModel::Block::typeId);
		aClass.def("hierarchyTypeIds", &OOModel::Block::hierarchyTypeIds);
		aClass.def("typeNameStatic", make_function(&OOModel::Block::typeNameStatic,
			 return_value_policy<copy_const_reference>()));
		aClass.staticmethod("typeNameStatic");
		aClass.def("typeIdStatic", &OOModel::Block::typeIdStatic);
		aClass.staticmethod("typeIdStatic");
		aClass.def("initType", &OOModel::Block::initType);
		aClass.staticmethod("initType");
		aClass.def("clone", make_function(&OOModel::Block::clone, return_internal_reference<>()));
		aClass.def("createDefaultInstance", make_function(
			&OOModel::Block::createDefaultInstance, return_internal_reference<>()));
		aClass.staticmethod("createDefaultInstance");
		aClass.def("getMetaData", make_function(&OOModel::Block::getMetaData, return_internal_reference<>()));
		aClass.staticmethod("getMetaData");
		aClass.def("isSubtypeOf", Block_isSubtypeOf1);
		aClass.def("isSubtypeOf", Block_isSubtypeOf2);
		aClass.def("registerNewAttribute", Block_registerNewAttribute1);
		aClass.def("registerNewAttribute", Block_registerNewAttribute2);
}

void initTypedListWrappers() {
	using MetaCallMappings = Model::TypedList<OOModel::MetaCallMapping>;
	auto MetaCallMappingsDef = class_<Model::TypedList<OOModel::MetaCallMapping>, bases<Model::List>>("MetaCallMappings");
		MetaCallMappingsDef.def("__len__", &MetaCallMappings::size);
		MetaCallMappingsDef.def("__iter__", iterator<MetaCallMappings, return_internal_reference<>>());

	using FormalMetaArguments = Model::TypedList<OOModel::FormalMetaArgument>;
	auto FormalMetaArgumentsDef = class_<Model::TypedList<OOModel::FormalMetaArgument>,
		 bases<Model::List>>("FormalMetaArguments");
		FormalMetaArgumentsDef.def("__len__", &FormalMetaArguments::size);
		FormalMetaArgumentsDef.def("__iter__", iterator<FormalMetaArguments, return_internal_reference<>>());

	using Expressions = Model::TypedList<OOModel::Expression>;
	auto ExpressionsDef = class_<Model::TypedList<OOModel::Expression>, bases<Model::List>>("Expressions");
		ExpressionsDef.def("__len__", &Expressions::size);
		ExpressionsDef.def("__iter__", iterator<Expressions, return_internal_reference<>>());

	using FormalTypeArguments = Model::TypedList<OOModel::FormalTypeArgument>;
	auto FormalTypeArgumentsDef = class_<Model::TypedList<OOModel::FormalTypeArgument>,
		 bases<Model::List>>("FormalTypeArguments");
		FormalTypeArgumentsDef.def("__len__", &FormalTypeArguments::size);
		FormalTypeArgumentsDef.def("__iter__", iterator<FormalTypeArguments, return_internal_reference<>>());

	using FormalArguments = Model::TypedList<OOModel::FormalArgument>;
	auto FormalArgumentsDef = class_<Model::TypedList<OOModel::FormalArgument>, bases<Model::List>>("FormalArguments");
		FormalArgumentsDef.def("__len__", &FormalArguments::size);
		FormalArgumentsDef.def("__iter__", iterator<FormalArguments, return_internal_reference<>>());

	using Classs = Model::TypedList<OOModel::Class>;
	auto ClasssDef = class_<Model::TypedList<OOModel::Class>, bases<Model::List>>("Classs");
		ClasssDef.def("__len__", &Classs::size);
		ClasssDef.def("__iter__", iterator<Classs, return_internal_reference<>>());

	using MetaBindings = Model::TypedList<OOModel::MetaBinding>;
	auto MetaBindingsDef = class_<Model::TypedList<OOModel::MetaBinding>, bases<Model::List>>("MetaBindings");
		MetaBindingsDef.def("__len__", &MetaBindings::size);
		MetaBindingsDef.def("__iter__", iterator<MetaBindings, return_internal_reference<>>());

	using CatchClauses = Model::TypedList<OOModel::CatchClause>;
	auto CatchClausesDef = class_<Model::TypedList<OOModel::CatchClause>, bases<Model::List>>("CatchClauses");
		CatchClausesDef.def("__len__", &CatchClauses::size);
		CatchClausesDef.def("__iter__", iterator<CatchClauses, return_internal_reference<>>());

	using Modules = Model::TypedList<OOModel::Module>;
	auto ModulesDef = class_<Model::TypedList<OOModel::Module>, bases<Model::List>>("Modules");
		ModulesDef.def("__len__", &Modules::size);
		ModulesDef.def("__iter__", iterator<Modules, return_internal_reference<>>());

	using Declarations = Model::TypedList<OOModel::Declaration>;
	auto DeclarationsDef = class_<Model::TypedList<OOModel::Declaration>, bases<Model::List>>("Declarations");
		DeclarationsDef.def("__len__", &Declarations::size);
		DeclarationsDef.def("__iter__", iterator<Declarations, return_internal_reference<>>());

	using MemberInitializers = Model::TypedList<OOModel::MemberInitializer>;
	auto MemberInitializersDef = class_<Model::TypedList<OOModel::MemberInitializer>,
		 bases<Model::List>>("MemberInitializers");
		MemberInitializersDef.def("__len__", &MemberInitializers::size);
		MemberInitializersDef.def("__iter__", iterator<MemberInitializers, return_internal_reference<>>());

	using FormalResults = Model::TypedList<OOModel::FormalResult>;
	auto FormalResultsDef = class_<Model::TypedList<OOModel::FormalResult>, bases<Model::List>>("FormalResults");
		FormalResultsDef.def("__len__", &FormalResults::size);
		FormalResultsDef.def("__iter__", iterator<FormalResults, return_internal_reference<>>());

	using Texts = Model::TypedList<Model::Text>;
	auto TextsDef = class_<Model::TypedList<Model::Text>, bases<Model::List>>("Texts");
		TextsDef.def("__len__", &Texts::size);
		TextsDef.def("__iter__", iterator<Texts, return_internal_reference<>>());

	using Methods = Model::TypedList<OOModel::Method>;
	auto MethodsDef = class_<Model::TypedList<OOModel::Method>, bases<Model::List>>("Methods");
		MethodsDef.def("__len__", &Methods::size);
		MethodsDef.def("__iter__", iterator<Methods, return_internal_reference<>>());

	using Fields = Model::TypedList<OOModel::Field>;
	auto FieldsDef = class_<Model::TypedList<OOModel::Field>, bases<Model::List>>("Fields");
		FieldsDef.def("__len__", &Fields::size);
		FieldsDef.def("__iter__", iterator<Fields, return_internal_reference<>>());

	using Enumerators = Model::TypedList<OOModel::Enumerator>;
	auto EnumeratorsDef = class_<Model::TypedList<OOModel::Enumerator>, bases<Model::List>>("Enumerators");
		EnumeratorsDef.def("__len__", &Enumerators::size);
		EnumeratorsDef.def("__iter__", iterator<Enumerators, return_internal_reference<>>());

	using Projects = Model::TypedList<OOModel::Project>;
	auto ProjectsDef = class_<Model::TypedList<OOModel::Project>, bases<Model::List>>("Projects");
		ProjectsDef.def("__len__", &Projects::size);
		ProjectsDef.def("__iter__", iterator<Projects, return_internal_reference<>>());

	using UsedLibrarys = Model::TypedList<Model::UsedLibrary>;
	auto UsedLibrarysDef = class_<Model::TypedList<Model::UsedLibrary>, bases<Model::List>>("UsedLibrarys");
		UsedLibrarysDef.def("__len__", &UsedLibrarys::size);
		UsedLibrarysDef.def("__iter__", iterator<UsedLibrarys, return_internal_reference<>>());

}


BOOST_PYTHON_MODULE(AstApi) {

	initClassNode();
	initClassCompositeNode();
	initClassModifier();
	initClassReference();
	initClassList();
	initClassBoolean();
	initClassCharacter();
	initClassText();
	initClassFloat();
	initClassInteger();
	initClassExpression();
	initClassDeclaration();
	initClassMemberInitializer();
	initClassEnumerator();
	initClassCatchClause();
	initClassFormalResult();
	initClassStatementItem();
	initClassFormalTypeArgument();
	initClassUsedLibrary();
	initClassOOReference();
	initClassTypedListOfStatementItem();
	initClassNameText();
	initClassCommaExpression();
	initClassUnaryOperation();
	initClassMetaCallExpression();
	initClassUnfinishedOperator();
	initClassNullLiteral();
	initClassFloatLiteral();
	initClassTypeTraitExpression();
	initClassSuperExpression();
	initClassConditionalExpression();
	initClassTypeNameOperator();
	initClassThisExpression();
	initClassBinaryOperation();
	initClassEmptyExpression();
	initClassMethodCallExpression();
	initClassDeleteExpression();
	initClassLambdaExpression();
	initClassIntegerLiteral();
	initClassVariableDeclarationExpression();
	initClassStringLiteral();
	initClassInstanceOfExpression();
	initClassBooleanLiteral();
	initClassTypeExpression();
	initClassReferenceExpression();
	initClassArrayInitializer();
	initClassThrowExpression();
	initClassErrorExpression();
	initClassAssignmentExpression();
	initClassGlobalScopeExpression();
	initClassCharacterLiteral();
	initClassCastExpression();
	initClassNewExpression();
	initClassProject();
	initClassExplicitTemplateInstantiation();
	initClassMetaBinding();
	initClassNameImport();
	initClassVariableDeclaration();
	initClassMetaCallMapping();
	initClassMethod();
	initClassModule();
	initClassClass();
	initClassMetaDefinition();
	initClassTypeAlias();
	initClassFormalMetaArgument();
	initClassCommentStatementItem();
	initClassStatement();
	initClassStatementItemList();
	initClassFunctionTypeExpression();
	initClassTypeQualifierExpression();
	initClassArrayTypeExpression();
	initClassAutoTypeExpression();
	initClassReferenceTypeExpression();
	initClassPrimitiveTypeExpression();
	initClassClassTypeExpression();
	initClassPointerTypeExpression();
	initClassField();
	initClassFormalArgument();
	initClassSynchronizedStatement();
	initClassIfStatement();
	initClassReturnStatement();
	initClassSwitchStatement();
	initClassCaseStatement();
	initClassBreakStatement();
	initClassTryCatchFinallyStatement();
	initClassDeclarationStatement();
	initClassForEachStatement();
	initClassAssertStatement();
	initClassExpressionStatement();
	initClassContinueStatement();
	initClassLoopStatement();
	initClassBlock();
	initTypedListWrappers();
}


} /* namespace InformationScripting */
