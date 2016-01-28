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

namespace InformationScripting {

using namespace boost::python;

BOOST_PYTHON_MODULE(AstApi) {

{
	Model::Node* (*Node_createNewNode1)(const QString&, Model::Node*,
		 Model::PersistentStore&, bool) = &Model::Node::createNewNode;
	Model::Node* (*Node_createNewNode2)(const QString&, Model::Node*) = &Model::Node::createNewNode;
	bool (Model::Node::*Node_isSubtypeOf1)(const QString&) const = &Model::Node::isSubtypeOf;
	bool (Model::Node::*Node_isSubtypeOf2)(int) const = &Model::Node::isSubtypeOf;
	scope Nodescope = class_<Model::Node, boost::noncopyable>("Node", no_init)
		.add_property("parent",
			make_function(&Model::Node::parent, return_internal_reference<>()),
			&Model::Node::setParent)
		.def("typeName", make_function((const QString& (Model::Node::*)(
			))&Model::Node::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::Node::typeId)
		.def("hierarchyTypeIds", &Model::Node::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::Node::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::Node::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::Node::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::Node::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&Model::Node::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("manager", make_function(&Model::Node::manager, return_internal_reference<>()))
		.def("root", make_function(&Model::Node::root, return_internal_reference<>()))
		.def("children", &Model::Node::children)
		.def("definesSymbol", &Model::Node::definesSymbol)
		.def("isTransparentForNameResolution", &Model::Node::isTransparentForNameResolution)
		.def("symbolName", make_function((const QString& (Model::Node::*)(
			))&Model::Node::symbolName, return_value_policy<copy_const_reference>()))
		.def("symbolType", &Model::Node::symbolType)
		.def("findSymbols", &Model::Node::findSymbols)
		.def("childrenInScope", &Model::Node::childrenInScope)
		.def("symbolMatches", &Model::Node::symbolMatches)
		.def("revision", &Model::Node::revision)
		.def("incrementRevision", &Model::Node::incrementRevision)
		.def("addToRevision", &Model::Node::addToRevision)
		.def("isPartiallyLoaded", &Model::Node::isPartiallyLoaded)
		.def("hasPartiallyLoadedChildren", &Model::Node::hasPartiallyLoadedChildren)
		.def("accessLock", make_function(&Model::Node::accessLock, return_internal_reference<>()))
		.def("lowestCommonAncestor", make_function(&Model::Node::lowestCommonAncestor, return_internal_reference<>()))
		.def("isAncestorOf", &Model::Node::isAncestorOf)
		.def("childToSubnode", make_function(&Model::Node::childToSubnode, return_internal_reference<>()))
		.def("execute", &Model::Node::execute)
		.def("save", &Model::Node::save)
		.def("load", &Model::Node::load)
		.def("isNewPersistenceUnit", &Model::Node::isNewPersistenceUnit)
		.def("persistentUnitNode", make_function(&Model::Node::persistentUnitNode, return_internal_reference<>()))
		.def("isTypeRegistered", &Model::Node::isTypeRegistered)
		.staticmethod("isTypeRegistered")
		.def("isModifyable", &Model::Node::isModifyable)
		.def("replaceChild", &Model::Node::replaceChild)
		.def("beginModification", &Model::Node::beginModification)
		.def("endModification", &Model::Node::endModification)
		.def("usedLibraries", &Model::Node::usedLibraries)
		.def("toDebugString", &Model::Node::toDebugString)
		.def("createNewNode", make_function(Node_createNewNode1, return_internal_reference<>()))
		.def("createNewNode", make_function(Node_createNewNode2, return_internal_reference<>()))
		.def("isSubtypeOf", Node_isSubtypeOf1)
		.def("isSubtypeOf", Node_isSubtypeOf2);
	enum_<Model::Node::SymbolType>("SymbolType")
		.value("UNSPECIFIED", Model::Node::SymbolType::UNSPECIFIED)
		.value("METHOD", Model::Node::SymbolType::METHOD)
		.value("CONTAINER", Model::Node::SymbolType::CONTAINER)
		.value("VARIABLE", Model::Node::SymbolType::VARIABLE)
		.value("ANY_SYMBOL", Model::Node::SymbolType::ANY_SYMBOL);
	enum_<Model::Node::FindSymbolDirection>("FindSymbolDirection")
		.value("SEARCH_UP", Model::Node::FindSymbolDirection::SEARCH_UP)
		.value("SEARCH_DOWN", Model::Node::FindSymbolDirection::SEARCH_DOWN)
		.value("SEARCH_HERE", Model::Node::FindSymbolDirection::SEARCH_HERE);
}

{
	Model::CompositeIndex (*CompositeNode_registerNewAttribute1)(const
		Model::Attribute&) = &Model::CompositeNode::registerNewAttribute;
	Model::Node* (Model::CompositeNode::*CompositeNode_get1)(const QString&) const = &Model::CompositeNode::get;
	Model::Node* (Model::CompositeNode::*CompositeNode_get2)(const
		Model::CompositeIndex&) const = &Model::CompositeNode::get;
	void (Model::CompositeNode::*CompositeNode_remove1)(QString) = &Model::CompositeNode::remove;
	void (Model::CompositeNode::*CompositeNode_remove2)(Model::Node*) = &Model::CompositeNode::remove;
	void (Model::CompositeNode::*CompositeNode_remove3)(const Model::CompositeIndex&) = &Model::CompositeNode::remove;
	bool (Model::CompositeNode::*CompositeNode_isSubtypeOf1)(const QString&) const = &Model::CompositeNode::isSubtypeOf;
	bool (Model::CompositeNode::*CompositeNode_isSubtypeOf2)(int) const = &Model::CompositeNode::isSubtypeOf;
	Model::CompositeIndex (Model::CompositeNode::*CompositeNode_indexOf1)(
		const QString&) const = &Model::CompositeNode::indexOf;
	Model::CompositeIndex (Model::CompositeNode::*CompositeNode_indexOf2)(
		Model::Node*) const = &Model::CompositeNode::indexOf;
	class_<Model::CompositeNode, bases<Model::Node>>("CompositeNode")
		.add_property("comment",
			make_function(&Model::CompositeNode::comment, return_internal_reference<>()),
			&Model::CompositeNode::setComment)
		.def("typeName", make_function((const QString& (Model::CompositeNode::*)(
			))&Model::CompositeNode::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::CompositeNode::typeId)
		.def("hierarchyTypeIds", &Model::CompositeNode::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::CompositeNode::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::CompositeNode::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::CompositeNode::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::CompositeNode::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&Model::CompositeNode::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("children", &Model::CompositeNode::children)
		.def("replaceChild", &Model::CompositeNode::replaceChild)
		.def("set", &Model::CompositeNode::set)
		.def("save", &Model::CompositeNode::save)
		.def("load", &Model::CompositeNode::load)
		.def("hasAttribute", &Model::CompositeNode::hasAttribute)
		.def("getAllAttributes", &Model::CompositeNode::getAllAttributes)
		.def("getMetaData", make_function(&Model::CompositeNode::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("registerExtensionId", &Model::CompositeNode::registerExtensionId)
		.staticmethod("registerExtensionId")
		.def("meta", make_function((const Model::AttributeChain& (
			Model::CompositeNode::*)())&Model::CompositeNode::meta, return_value_policy<copy_const_reference>()))
		.def("registerNewAttribute", CompositeNode_registerNewAttribute1)
		.def("get", make_function(CompositeNode_get1, return_internal_reference<>()))
		.def("get", make_function(CompositeNode_get2, return_internal_reference<>()))
		.def("remove", CompositeNode_remove1)
		.def("remove", CompositeNode_remove2)
		.def("remove", CompositeNode_remove3)
		.def("isSubtypeOf", CompositeNode_isSubtypeOf1)
		.def("isSubtypeOf", CompositeNode_isSubtypeOf2)
		.def("indexOf", CompositeNode_indexOf1)
		.def("indexOf", CompositeNode_indexOf2);
}

{
	bool (OOModel::Modifier::*Modifier_isSubtypeOf1)(const QString&) const = &OOModel::Modifier::isSubtypeOf;
	bool (OOModel::Modifier::*Modifier_isSubtypeOf2)(int) const = &OOModel::Modifier::isSubtypeOf;
	scope Modifierscope = class_<OOModel::Modifier, bases<Model::Node>>("Modifier")
		.def("typeName", make_function((const QString& (OOModel::Modifier::*)(
			))&OOModel::Modifier::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Modifier::typeId)
		.def("hierarchyTypeIds", &OOModel::Modifier::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Modifier::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Modifier::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Modifier::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Modifier::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&OOModel::Modifier::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("get", &OOModel::Modifier::get)
		.def("isSet", &OOModel::Modifier::isSet)
		.def("set", &OOModel::Modifier::set)
		.def("clear", &OOModel::Modifier::clear)
		.def("save", &OOModel::Modifier::save)
		.def("load", &OOModel::Modifier::load)
		.def("isSubtypeOf", Modifier_isSubtypeOf1)
		.def("isSubtypeOf", Modifier_isSubtypeOf2);
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

{
	bool (Model::Reference::*Reference_isSubtypeOf1)(const QString&) const = &Model::Reference::isSubtypeOf;
	bool (Model::Reference::*Reference_isSubtypeOf2)(int) const = &Model::Reference::isSubtypeOf;
	class_<Model::Reference, bases<Model::Node>>("Reference")
		.add_property("name",
			make_function((const QString& (Model::Reference::*)(
				))&Model::Reference::name, return_value_policy<copy_const_reference>()),
			&Model::Reference::setName)
		.def("typeName", make_function((const QString& (Model::Reference::*)(
			))&Model::Reference::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::Reference::typeId)
		.def("hierarchyTypeIds", &Model::Reference::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::Reference::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::Reference::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::Reference::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::Reference::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&Model::Reference::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("target", make_function(&Model::Reference::target, return_internal_reference<>()))
		.def("save", &Model::Reference::save)
		.def("load", &Model::Reference::load)
		.def("resolve", &Model::Reference::resolve)
		.def("isResolved", &Model::Reference::isResolved)
		.def("computeTarget", make_function(&Model::Reference::computeTarget, return_internal_reference<>()))
		.def("unresolveAll", &Model::Reference::unresolveAll)
		.staticmethod("unresolveAll")
		.def("unresolveNames", &Model::Reference::unresolveNames)
		.staticmethod("unresolveNames")
		.def("unresolveAfterNewSubTree", &Model::Reference::unresolveAfterNewSubTree)
		.staticmethod("unresolveAfterNewSubTree")
		.def("addUnresolutionSteps", &Model::Reference::addUnresolutionSteps)
		.staticmethod("addUnresolutionSteps")
		.def("unresolveIfNameIntroduced", &Model::Reference::unresolveIfNameIntroduced)
		.staticmethod("unresolveIfNameIntroduced")
		.def("resolvePending", &Model::Reference::resolvePending)
		.staticmethod("resolvePending")
		.def("isSubtypeOf", Reference_isSubtypeOf1)
		.def("isSubtypeOf", Reference_isSubtypeOf2);
}

{
	void (Model::List::*List_remove1)(Model::Node*) = &Model::List::remove;
	void (Model::List::*List_remove2)(int) = &Model::List::remove;
	bool (Model::List::*List_isSubtypeOf1)(const QString&) const = &Model::List::isSubtypeOf;
	bool (Model::List::*List_isSubtypeOf2)(int) const = &Model::List::isSubtypeOf;
	class_<Model::List, bases<Model::Node>>("List")
		.def("typeName", make_function((const QString& (Model::List::*)(
			))&Model::List::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::List::typeId)
		.def("hierarchyTypeIds", &Model::List::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::List::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::List::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::List::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::List::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&Model::List::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("save", &Model::List::save)
		.def("load", &Model::List::load)
		.def("children", &Model::List::children)
		.def("size", &Model::List::size)
		.def("isEmpty", &Model::List::isEmpty)
		.def("indexOf", &Model::List::indexOf)
		.def("indexToSubnode", &Model::List::indexToSubnode)
		.def("contains", &Model::List::contains)
		.def("append", &Model::List::append)
		.def("prepend", &Model::List::prepend)
		.def("insert", &Model::List::insert)
		.def("paste", &Model::List::paste)
		.def("clear", &Model::List::clear)
		.def("nodes", make_function((const QVector<Model::Node*>& (
			Model::List::*)())&Model::List::nodes, return_value_policy<copy_const_reference>()))
		.def("isTransparentForNameResolution", &Model::List::isTransparentForNameResolution)
		.def("findSymbols", &Model::List::findSymbols)
		.def("replaceChild", &Model::List::replaceChild)
		.def("createDefaultElement", make_function(&Model::List::createDefaultElement, return_internal_reference<>()))
		.def("lowerTypeBoundForElements", &Model::List::lowerTypeBoundForElements)
		.def("remove", List_remove1)
		.def("remove", List_remove2)
		.def("isSubtypeOf", List_isSubtypeOf1)
		.def("isSubtypeOf", List_isSubtypeOf2);
}

{
	bool (Model::Boolean::*Boolean_isSubtypeOf1)(const QString&) const = &Model::Boolean::isSubtypeOf;
	bool (Model::Boolean::*Boolean_isSubtypeOf2)(int) const = &Model::Boolean::isSubtypeOf;
	class_<Model::Boolean, bases<Model::Node>>("Boolean")
		.def("typeName", make_function((const QString& (Model::Boolean::*)(
			))&Model::Boolean::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::Boolean::typeId)
		.def("hierarchyTypeIds", &Model::Boolean::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::Boolean::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::Boolean::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::Boolean::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::Boolean::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&Model::Boolean::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("get", &Model::Boolean::get)
		.def("set", &Model::Boolean::set)
		.def("save", &Model::Boolean::save)
		.def("load", &Model::Boolean::load)
		.def("isSubtypeOf", Boolean_isSubtypeOf1)
		.def("isSubtypeOf", Boolean_isSubtypeOf2);
}

{
	bool (Model::Character::*Character_isSubtypeOf1)(const QString&) const = &Model::Character::isSubtypeOf;
	bool (Model::Character::*Character_isSubtypeOf2)(int) const = &Model::Character::isSubtypeOf;
	class_<Model::Character, bases<Model::Node>>("Character")
		.def("typeName", make_function((const QString& (Model::Character::*)(
			))&Model::Character::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::Character::typeId)
		.def("hierarchyTypeIds", &Model::Character::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::Character::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::Character::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::Character::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::Character::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&Model::Character::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("get", &Model::Character::get)
		.def("set", &Model::Character::set)
		.def("save", &Model::Character::save)
		.def("load", &Model::Character::load)
		.def("isSubtypeOf", Character_isSubtypeOf1)
		.def("isSubtypeOf", Character_isSubtypeOf2);
}

{
	bool (Model::Text::*Text_isSubtypeOf1)(const QString&) const = &Model::Text::isSubtypeOf;
	bool (Model::Text::*Text_isSubtypeOf2)(int) const = &Model::Text::isSubtypeOf;
	class_<Model::Text, bases<Model::Node>>("Text")
		.def("typeName", make_function((const QString& (Model::Text::*)(
			))&Model::Text::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::Text::typeId)
		.def("hierarchyTypeIds", &Model::Text::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::Text::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::Text::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::Text::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::Text::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&Model::Text::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("get", make_function((const QString& (Model::Text::*)(
			))&Model::Text::get, return_value_policy<copy_const_reference>()))
		.def("operator const QString &", make_function((const QString&
			(Model::Text::*)())&Model::Text::operator const QString &, return_value_policy<copy_const_reference>()))
		.def("set", &Model::Text::set)
		.def("save", &Model::Text::save)
		.def("load", &Model::Text::load)
		.def("isSubtypeOf", Text_isSubtypeOf1)
		.def("isSubtypeOf", Text_isSubtypeOf2);
}

{
	bool (Model::Float::*Float_isSubtypeOf1)(const QString&) const = &Model::Float::isSubtypeOf;
	bool (Model::Float::*Float_isSubtypeOf2)(int) const = &Model::Float::isSubtypeOf;
	class_<Model::Float, bases<Model::Node>>("Float")
		.def("typeName", make_function((const QString& (Model::Float::*)(
			))&Model::Float::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::Float::typeId)
		.def("hierarchyTypeIds", &Model::Float::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::Float::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::Float::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::Float::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::Float::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&Model::Float::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("get", &Model::Float::get)
		.def("set", &Model::Float::set)
		.def("save", &Model::Float::save)
		.def("load", &Model::Float::load)
		.def("isSubtypeOf", Float_isSubtypeOf1)
		.def("isSubtypeOf", Float_isSubtypeOf2);
}

{
	bool (Model::Integer::*Integer_isSubtypeOf1)(const QString&) const = &Model::Integer::isSubtypeOf;
	bool (Model::Integer::*Integer_isSubtypeOf2)(int) const = &Model::Integer::isSubtypeOf;
	class_<Model::Integer, bases<Model::Node>>("Integer")
		.def("typeName", make_function((const QString& (Model::Integer::*)(
			))&Model::Integer::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::Integer::typeId)
		.def("hierarchyTypeIds", &Model::Integer::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::Integer::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::Integer::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::Integer::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::Integer::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&Model::Integer::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("get", &Model::Integer::get)
		.def("set", &Model::Integer::set)
		.def("save", &Model::Integer::save)
		.def("load", &Model::Integer::load)
		.def("isSubtypeOf", Integer_isSubtypeOf1)
		.def("isSubtypeOf", Integer_isSubtypeOf2);
}

{
	Model::CompositeIndex (*Expression_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::Expression::registerNewAttribute;
	Model::CompositeIndex (*Expression_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Expression::registerNewAttribute;
	bool (OOModel::Expression::*Expression_isSubtypeOf1)(const QString&) const = &OOModel::Expression::isSubtypeOf;
	bool (OOModel::Expression::*Expression_isSubtypeOf2)(int) const = &OOModel::Expression::isSubtypeOf;
	class_<OOModel::Expression, bases<Model::CompositeNode>>("Expression")
		.def("typeName", make_function((const QString& (OOModel::Expression::*)(
			))&OOModel::Expression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Expression::typeId)
		.def("hierarchyTypeIds", &OOModel::Expression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Expression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Expression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Expression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Expression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::Expression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::Expression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::Expression::type)
		.def("topMostExpressionParent", make_function(
			&OOModel::Expression::topMostExpressionParent, return_internal_reference<>()))
		.def("registerNewAttribute", Expression_registerNewAttribute1)
		.def("registerNewAttribute", Expression_registerNewAttribute2)
		.def("isSubtypeOf", Expression_isSubtypeOf1)
		.def("isSubtypeOf", Expression_isSubtypeOf2);
}

{
	Model::CompositeIndex (*Declaration_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::Declaration::registerNewAttribute;
	Model::CompositeIndex (*Declaration_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::Declaration::registerNewAttribute;
	bool (OOModel::Declaration::*Declaration_isSubtypeOf1)(const QString&) const = &OOModel::Declaration::isSubtypeOf;
	bool (OOModel::Declaration::*Declaration_isSubtypeOf2)(int) const = &OOModel::Declaration::isSubtypeOf;
	class_<OOModel::Declaration, bases<Model::CompositeNode>>("Declaration")
		.add_property("name",
			make_function((const QString& (OOModel::Declaration::*)(
				))&OOModel::Declaration::name, return_value_policy<copy_const_reference>()),
			&OOModel::Declaration::setName)
		.add_property("annotations",
			make_function(&OOModel::Declaration::annotations, return_internal_reference<>()),
			&OOModel::Declaration::setAnnotations)
		.add_property("modifiers",
			make_function(&OOModel::Declaration::modifiers, return_internal_reference<>()),
			&OOModel::Declaration::setModifiers)
		.add_property("subDeclarations",
			make_function(&OOModel::Declaration::subDeclarations, return_internal_reference<>()),
			&OOModel::Declaration::setSubDeclarations)
		.add_property("metaCalls",
			make_function(&OOModel::Declaration::metaCalls, return_internal_reference<>()),
			&OOModel::Declaration::setMetaCalls)
		.def("typeName", make_function((const QString& (OOModel::Declaration::*)(
			))&OOModel::Declaration::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Declaration::typeId)
		.def("hierarchyTypeIds", &OOModel::Declaration::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Declaration::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Declaration::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Declaration::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Declaration::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::Declaration::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::Declaration::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("nameNode", make_function(&OOModel::Declaration::nameNode, return_internal_reference<>()))
		.def("definesSymbol", &OOModel::Declaration::definesSymbol)
		.def("symbolName", make_function((const QString& (
			OOModel::Declaration::*)())&OOModel::Declaration::symbolName, return_value_policy<copy_const_reference>()))
		.def("symbolType", &OOModel::Declaration::symbolType)
		.def("registerNewAttribute", Declaration_registerNewAttribute1)
		.def("registerNewAttribute", Declaration_registerNewAttribute2)
		.def("isSubtypeOf", Declaration_isSubtypeOf1)
		.def("isSubtypeOf", Declaration_isSubtypeOf2);
}

{
	bool (OOModel::MemberInitializer::*MemberInitializer_isSubtypeOf1)(
		const QString&) const = &OOModel::MemberInitializer::isSubtypeOf;
	bool (OOModel::MemberInitializer::*MemberInitializer_isSubtypeOf2)(
		int) const = &OOModel::MemberInitializer::isSubtypeOf;
	Model::CompositeIndex (*MemberInitializer_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MemberInitializer::registerNewAttribute;
	Model::CompositeIndex (*MemberInitializer_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MemberInitializer::registerNewAttribute;
	class_<OOModel::MemberInitializer, bases<Model::CompositeNode>>("MemberInitializer")
		.add_property("arguments",
			make_function(&OOModel::MemberInitializer::arguments, return_internal_reference<>()),
			&OOModel::MemberInitializer::setArguments)
		.add_property("memberReference",
			make_function(&OOModel::MemberInitializer::memberReference, return_internal_reference<>()),
			&OOModel::MemberInitializer::setMemberReference)
		.def("typeName", make_function((const QString& (
			OOModel::MemberInitializer::*)())&OOModel::MemberInitializer::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::MemberInitializer::typeId)
		.def("hierarchyTypeIds", &OOModel::MemberInitializer::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::MemberInitializer::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::MemberInitializer::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::MemberInitializer::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::MemberInitializer::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::MemberInitializer::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::MemberInitializer::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", MemberInitializer_isSubtypeOf1)
		.def("isSubtypeOf", MemberInitializer_isSubtypeOf2)
		.def("registerNewAttribute", MemberInitializer_registerNewAttribute1)
		.def("registerNewAttribute", MemberInitializer_registerNewAttribute2);
}

{
	Model::CompositeIndex (*Enumerator_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::Enumerator::registerNewAttribute;
	Model::CompositeIndex (*Enumerator_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Enumerator::registerNewAttribute;
	bool (OOModel::Enumerator::*Enumerator_isSubtypeOf1)(const QString&) const = &OOModel::Enumerator::isSubtypeOf;
	bool (OOModel::Enumerator::*Enumerator_isSubtypeOf2)(int) const = &OOModel::Enumerator::isSubtypeOf;
	class_<OOModel::Enumerator, bases<Model::CompositeNode>>("Enumerator")
		.add_property("name",
			make_function((const QString& (OOModel::Enumerator::*)(
				))&OOModel::Enumerator::name, return_value_policy<copy_const_reference>()),
			&OOModel::Enumerator::setName)
		.add_property("value",
			make_function(&OOModel::Enumerator::value, return_internal_reference<>()),
			&OOModel::Enumerator::setValue)
		.def("typeName", make_function((const QString& (OOModel::Enumerator::*)(
			))&OOModel::Enumerator::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Enumerator::typeId)
		.def("hierarchyTypeIds", &OOModel::Enumerator::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Enumerator::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Enumerator::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Enumerator::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Enumerator::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::Enumerator::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::Enumerator::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("nameNode", make_function(&OOModel::Enumerator::nameNode, return_internal_reference<>()))
		.def("definesSymbol", &OOModel::Enumerator::definesSymbol)
		.def("symbolName", make_function((const QString& (
			OOModel::Enumerator::*)())&OOModel::Enumerator::symbolName, return_value_policy<copy_const_reference>()))
		.def("symbolType", &OOModel::Enumerator::symbolType)
		.def("registerNewAttribute", Enumerator_registerNewAttribute1)
		.def("registerNewAttribute", Enumerator_registerNewAttribute2)
		.def("isSubtypeOf", Enumerator_isSubtypeOf1)
		.def("isSubtypeOf", Enumerator_isSubtypeOf2);
}

{
	bool (OOModel::CatchClause::*CatchClause_isSubtypeOf1)(const QString&) const = &OOModel::CatchClause::isSubtypeOf;
	bool (OOModel::CatchClause::*CatchClause_isSubtypeOf2)(int) const = &OOModel::CatchClause::isSubtypeOf;
	Model::CompositeIndex (*CatchClause_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::CatchClause::registerNewAttribute;
	Model::CompositeIndex (*CatchClause_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::CatchClause::registerNewAttribute;
	class_<OOModel::CatchClause, bases<Model::CompositeNode>>("CatchClause")
		.add_property("exceptionToCatch",
			make_function(&OOModel::CatchClause::exceptionToCatch, return_internal_reference<>()),
			&OOModel::CatchClause::setExceptionToCatch)
		.add_property("body",
			make_function(&OOModel::CatchClause::body, return_internal_reference<>()),
			&OOModel::CatchClause::setBody)
		.def("typeName", make_function((const QString& (OOModel::CatchClause::*)(
			))&OOModel::CatchClause::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::CatchClause::typeId)
		.def("hierarchyTypeIds", &OOModel::CatchClause::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::CatchClause::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::CatchClause::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::CatchClause::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::CatchClause::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::CatchClause::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::CatchClause::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("findSymbols", &OOModel::CatchClause::findSymbols)
		.def("isSubtypeOf", CatchClause_isSubtypeOf1)
		.def("isSubtypeOf", CatchClause_isSubtypeOf2)
		.def("registerNewAttribute", CatchClause_registerNewAttribute1)
		.def("registerNewAttribute", CatchClause_registerNewAttribute2);
}

{
	Model::CompositeIndex (*FormalResult_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FormalResult::registerNewAttribute;
	Model::CompositeIndex (*FormalResult_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FormalResult::registerNewAttribute;
	bool (OOModel::FormalResult::*FormalResult_isSubtypeOf1)(const QString&) const = &OOModel::FormalResult::isSubtypeOf;
	bool (OOModel::FormalResult::*FormalResult_isSubtypeOf2)(int) const = &OOModel::FormalResult::isSubtypeOf;
	class_<OOModel::FormalResult, bases<Model::CompositeNode>>("FormalResult")
		.add_property("name",
			make_function((const QString& (OOModel::FormalResult::*)(
				))&OOModel::FormalResult::name, return_value_policy<copy_const_reference>()),
			&OOModel::FormalResult::setName)
		.add_property("typeExpression",
			make_function(&OOModel::FormalResult::typeExpression, return_internal_reference<>()),
			&OOModel::FormalResult::setTypeExpression)
		.def("typeName", make_function((const QString& (OOModel::FormalResult::*)(
			))&OOModel::FormalResult::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::FormalResult::typeId)
		.def("hierarchyTypeIds", &OOModel::FormalResult::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::FormalResult::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::FormalResult::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::FormalResult::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::FormalResult::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::FormalResult::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::FormalResult::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("nameNode", make_function(&OOModel::FormalResult::nameNode, return_internal_reference<>()))
		.def("definesSymbol", &OOModel::FormalResult::definesSymbol)
		.def("symbolName", make_function((const QString& (
			OOModel::FormalResult::*)())&OOModel::FormalResult::symbolName, return_value_policy<copy_const_reference>()))
		.def("symbolType", &OOModel::FormalResult::symbolType)
		.def("registerNewAttribute", FormalResult_registerNewAttribute1)
		.def("registerNewAttribute", FormalResult_registerNewAttribute2)
		.def("isSubtypeOf", FormalResult_isSubtypeOf1)
		.def("isSubtypeOf", FormalResult_isSubtypeOf2);
}

{
	bool (OOModel::StatementItem::*StatementItem_isSubtypeOf1)(
		const QString&) const = &OOModel::StatementItem::isSubtypeOf;
	bool (OOModel::StatementItem::*StatementItem_isSubtypeOf2)(int) const = &OOModel::StatementItem::isSubtypeOf;
	Model::CompositeIndex (*StatementItem_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::StatementItem::registerNewAttribute;
	Model::CompositeIndex (*StatementItem_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::StatementItem::registerNewAttribute;
	class_<OOModel::StatementItem, bases<Model::CompositeNode>>("StatementItem")
		.def("typeName", make_function((const QString& (
			OOModel::StatementItem::*)())&OOModel::StatementItem::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::StatementItem::typeId)
		.def("hierarchyTypeIds", &OOModel::StatementItem::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::StatementItem::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::StatementItem::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::StatementItem::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::StatementItem::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::StatementItem::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::StatementItem::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", StatementItem_isSubtypeOf1)
		.def("isSubtypeOf", StatementItem_isSubtypeOf2)
		.def("registerNewAttribute", StatementItem_registerNewAttribute1)
		.def("registerNewAttribute", StatementItem_registerNewAttribute2);
}

{
	Model::CompositeIndex (*FormalTypeArgument_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FormalTypeArgument::registerNewAttribute;
	Model::CompositeIndex (*FormalTypeArgument_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FormalTypeArgument::registerNewAttribute;
	bool (OOModel::FormalTypeArgument::*FormalTypeArgument_isSubtypeOf1)(
		const QString&) const = &OOModel::FormalTypeArgument::isSubtypeOf;
	bool (OOModel::FormalTypeArgument::*FormalTypeArgument_isSubtypeOf2)(
		int) const = &OOModel::FormalTypeArgument::isSubtypeOf;
	class_<OOModel::FormalTypeArgument, bases<Model::CompositeNode>>("FormalTypeArgument")
		.add_property("name",
			make_function((const QString& (OOModel::FormalTypeArgument::*)(
				))&OOModel::FormalTypeArgument::name, return_value_policy<copy_const_reference>()),
			&OOModel::FormalTypeArgument::setName)
		.add_property("subTypeOfExpression",
			make_function(&OOModel::FormalTypeArgument::subTypeOfExpression, return_internal_reference<>()),
			&OOModel::FormalTypeArgument::setSubTypeOfExpression)
		.add_property("superTypeOfExpression",
			make_function(&OOModel::FormalTypeArgument::superTypeOfExpression, return_internal_reference<>()),
			&OOModel::FormalTypeArgument::setSuperTypeOfExpression)
		.add_property("specializationExpression",
			make_function(&OOModel::FormalTypeArgument::specializationExpression, return_internal_reference<>()),
			&OOModel::FormalTypeArgument::setSpecializationExpression)
		.add_property("defaultType",
			make_function(&OOModel::FormalTypeArgument::defaultType, return_internal_reference<>()),
			&OOModel::FormalTypeArgument::setDefaultType)
		.def("typeName", make_function((const QString& (
		OOModel::FormalTypeArgument::*)())&OOModel::FormalTypeArgument::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::FormalTypeArgument::typeId)
		.def("hierarchyTypeIds", &OOModel::FormalTypeArgument::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::FormalTypeArgument::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::FormalTypeArgument::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::FormalTypeArgument::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::FormalTypeArgument::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::FormalTypeArgument::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::FormalTypeArgument::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("nameNode", make_function(&OOModel::FormalTypeArgument::nameNode, return_internal_reference<>()))
		.def("definesSymbol", &OOModel::FormalTypeArgument::definesSymbol)
		.def("symbolName", make_function((const QString& (
		OOModel::FormalTypeArgument::*)())&OOModel::FormalTypeArgument::symbolName,
			 return_value_policy<copy_const_reference>()))
		.def("symbolType", &OOModel::FormalTypeArgument::symbolType)
		.def("registerNewAttribute", FormalTypeArgument_registerNewAttribute1)
		.def("registerNewAttribute", FormalTypeArgument_registerNewAttribute2)
		.def("isSubtypeOf", FormalTypeArgument_isSubtypeOf1)
		.def("isSubtypeOf", FormalTypeArgument_isSubtypeOf2);
}

{
	Model::CompositeIndex (*UsedLibrary_registerNewAttribute1)(
		const Model::Attribute&) = &Model::UsedLibrary::registerNewAttribute;
	Model::CompositeIndex (*UsedLibrary_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &Model::UsedLibrary::registerNewAttribute;
	bool (Model::UsedLibrary::*UsedLibrary_isSubtypeOf1)(const QString&) const = &Model::UsedLibrary::isSubtypeOf;
	bool (Model::UsedLibrary::*UsedLibrary_isSubtypeOf2)(int) const = &Model::UsedLibrary::isSubtypeOf;
	class_<Model::UsedLibrary, bases<Model::CompositeNode>>("UsedLibrary")
		.add_property("name",
			make_function((const QString& (Model::UsedLibrary::*)(
				))&Model::UsedLibrary::name, return_value_policy<copy_const_reference>()),
			&Model::UsedLibrary::setName)
		.def("typeName", make_function((const QString& (Model::UsedLibrary::*)(
			))&Model::UsedLibrary::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::UsedLibrary::typeId)
		.def("hierarchyTypeIds", &Model::UsedLibrary::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::UsedLibrary::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::UsedLibrary::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::UsedLibrary::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::UsedLibrary::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&Model::UsedLibrary::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&Model::UsedLibrary::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("nameNode", make_function(&Model::UsedLibrary::nameNode, return_internal_reference<>()))
		.def("usedLibraries", &Model::UsedLibrary::usedLibraries)
		.def("libraryRoot", make_function(&Model::UsedLibrary::libraryRoot, return_internal_reference<>()))
		.def("libraryManager", make_function(&Model::UsedLibrary::libraryManager, return_internal_reference<>()))
		.def("loadLibrary", &Model::UsedLibrary::loadLibrary)
		.def("registerNewAttribute", UsedLibrary_registerNewAttribute1)
		.def("registerNewAttribute", UsedLibrary_registerNewAttribute2)
		.def("isSubtypeOf", UsedLibrary_isSubtypeOf1)
		.def("isSubtypeOf", UsedLibrary_isSubtypeOf2);
}

{
	bool (OOModel::OOReference::*OOReference_isSubtypeOf1)(const QString&) const = &OOModel::OOReference::isSubtypeOf;
	bool (OOModel::OOReference::*OOReference_isSubtypeOf2)(int) const = &OOModel::OOReference::isSubtypeOf;
	class_<OOModel::OOReference, bases<Model::Reference>>("OOReference")
		.def("typeName", make_function((const QString& (OOModel::OOReference::*)(
			))&OOModel::OOReference::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::OOReference::typeId)
		.def("hierarchyTypeIds", &OOModel::OOReference::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::OOReference::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::OOReference::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::OOReference::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::OOReference::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::OOReference::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("computeTarget", make_function(&OOModel::OOReference::computeTarget, return_internal_reference<>()))
		.def("unresolveOOReferencesAfterSubTree", &OOModel::OOReference::unresolveOOReferencesAfterSubTree)
		.staticmethod("unresolveOOReferencesAfterSubTree")
		.def("isSubtypeOf", OOReference_isSubtypeOf1)
		.def("isSubtypeOf", OOReference_isSubtypeOf2);
}

{
	using TypedListOfStatementItem = Model::TypedList<OOModel::StatementItem>;
	bool (Model::TypedList<OOModel::StatementItem>::*TypedListOfStatementItem_isSubtypeOf1)(
		const QString&) const = &TypedListOfStatementItem::isSubtypeOf;
	bool (Model::TypedList<OOModel::StatementItem>::*TypedListOfStatementItem_isSubtypeOf2)(
		int) const = &TypedListOfStatementItem::isSubtypeOf;
	class_<Model::TypedList<OOModel::StatementItem>, bases<Model::List>>("TypedListOfStatementItem")
		.def("typeName", make_function((const QString& (
			TypedListOfStatementItem::*)())&TypedListOfStatementItem::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &TypedListOfStatementItem::typeId)
		.def("hierarchyTypeIds", &TypedListOfStatementItem::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&TypedListOfStatementItem::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &TypedListOfStatementItem::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &TypedListOfStatementItem::initType)
		.staticmethod("initType")
		.def("clone", make_function(&TypedListOfStatementItem::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&TypedListOfStatementItem::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("first", make_function(&TypedListOfStatementItem::first, return_internal_reference<>()))
		.def("last", make_function(&TypedListOfStatementItem::last, return_internal_reference<>()))
		.def("at", make_function(&TypedListOfStatementItem::at, return_internal_reference<>()))
		.def("replaceChild", &TypedListOfStatementItem::replaceChild)
		.def("createDefaultElement", make_function(
			&TypedListOfStatementItem::createDefaultElement, return_internal_reference<>()))
		.def("lowerTypeBoundForElements", &TypedListOfStatementItem::lowerTypeBoundForElements)
		.def("isSubtypeOf", TypedListOfStatementItem_isSubtypeOf1)
		.def("isSubtypeOf", TypedListOfStatementItem_isSubtypeOf2);
}

{
	bool (Model::NameText::*NameText_isSubtypeOf1)(const QString&) const = &Model::NameText::isSubtypeOf;
	bool (Model::NameText::*NameText_isSubtypeOf2)(int) const = &Model::NameText::isSubtypeOf;
	class_<Model::NameText, bases<Model::Text>>("NameText")
		.def("typeName", make_function((const QString& (Model::NameText::*)(
			))&Model::NameText::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &Model::NameText::typeId)
		.def("hierarchyTypeIds", &Model::NameText::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&Model::NameText::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &Model::NameText::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &Model::NameText::initType)
		.staticmethod("initType")
		.def("clone", make_function(&Model::NameText::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&Model::NameText::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("isSubtypeOf", NameText_isSubtypeOf1)
		.def("isSubtypeOf", NameText_isSubtypeOf2);
}

{
	Model::CompositeIndex (*CommaExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::CommaExpression::registerNewAttribute;
	Model::CompositeIndex (*CommaExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::CommaExpression::registerNewAttribute;
	bool (OOModel::CommaExpression::*CommaExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::CommaExpression::isSubtypeOf;
	bool (OOModel::CommaExpression::*CommaExpression_isSubtypeOf2)(int) const = &OOModel::CommaExpression::isSubtypeOf;
	class_<OOModel::CommaExpression, bases<OOModel::Expression>>("CommaExpression")
		.add_property("left",
			make_function(&OOModel::CommaExpression::left, return_internal_reference<>()),
			&OOModel::CommaExpression::setLeft)
		.add_property("right",
			make_function(&OOModel::CommaExpression::right, return_internal_reference<>()),
			&OOModel::CommaExpression::setRight)
		.def("typeName", make_function((const QString& (
			OOModel::CommaExpression::*)())&OOModel::CommaExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::CommaExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::CommaExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::CommaExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::CommaExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::CommaExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::CommaExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::CommaExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::CommaExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("allSubOperands", &OOModel::CommaExpression::allSubOperands)
		.def("type", &OOModel::CommaExpression::type)
		.def("findSymbols", &OOModel::CommaExpression::findSymbols)
		.def("registerNewAttribute", CommaExpression_registerNewAttribute1)
		.def("registerNewAttribute", CommaExpression_registerNewAttribute2)
		.def("isSubtypeOf", CommaExpression_isSubtypeOf1)
		.def("isSubtypeOf", CommaExpression_isSubtypeOf2);
}

{
	Model::CompositeIndex (*UnaryOperation_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::UnaryOperation::registerNewAttribute;
	Model::CompositeIndex (*UnaryOperation_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::UnaryOperation::registerNewAttribute;
	bool (OOModel::UnaryOperation::*UnaryOperation_isSubtypeOf1)(const
		QString&) const = &OOModel::UnaryOperation::isSubtypeOf;
	bool (OOModel::UnaryOperation::*UnaryOperation_isSubtypeOf2)(int) const = &OOModel::UnaryOperation::isSubtypeOf;
	scope UnaryOperationscope = class_<OOModel::UnaryOperation, bases<OOModel::Expression>>("UnaryOperation")
		.add_property("operand",
			make_function(&OOModel::UnaryOperation::operand, return_internal_reference<>()),
			&OOModel::UnaryOperation::setOperand)
		.add_property("op",
			&OOModel::UnaryOperation::op,
			&OOModel::UnaryOperation::setOp)
		.def("typeName", make_function((const QString& (
			OOModel::UnaryOperation::*)())&OOModel::UnaryOperation::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::UnaryOperation::typeId)
		.def("hierarchyTypeIds", &OOModel::UnaryOperation::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::UnaryOperation::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::UnaryOperation::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::UnaryOperation::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::UnaryOperation::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::UnaryOperation::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::UnaryOperation::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::UnaryOperation::type)
		.def("registerNewAttribute", UnaryOperation_registerNewAttribute1)
		.def("registerNewAttribute", UnaryOperation_registerNewAttribute2)
		.def("isSubtypeOf", UnaryOperation_isSubtypeOf1)
		.def("isSubtypeOf", UnaryOperation_isSubtypeOf2);
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

{
	Model::CompositeIndex (*MetaCallExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MetaCallExpression::registerNewAttribute;
	Model::CompositeIndex (*MetaCallExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MetaCallExpression::registerNewAttribute;
	bool (OOModel::MetaCallExpression::*MetaCallExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::MetaCallExpression::isSubtypeOf;
	bool (OOModel::MetaCallExpression::*MetaCallExpression_isSubtypeOf2)(
		int) const = &OOModel::MetaCallExpression::isSubtypeOf;
	class_<OOModel::MetaCallExpression, bases<OOModel::Expression>>("MetaCallExpression")
		.add_property("callee",
			make_function(&OOModel::MetaCallExpression::callee, return_internal_reference<>()),
			&OOModel::MetaCallExpression::setCallee)
		.add_property("arguments",
			make_function(&OOModel::MetaCallExpression::arguments, return_internal_reference<>()),
			&OOModel::MetaCallExpression::setArguments)
		.def("typeName", make_function((const QString& (
		OOModel::MetaCallExpression::*)())&OOModel::MetaCallExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::MetaCallExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::MetaCallExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::MetaCallExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::MetaCallExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::MetaCallExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::MetaCallExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::MetaCallExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::MetaCallExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("metaDefinition", make_function(&OOModel::MetaCallExpression::metaDefinition, return_internal_reference<>()))
		.def("generatedTree", make_function(&OOModel::MetaCallExpression::generatedTree, return_internal_reference<>()))
		.def("registerNewAttribute", MetaCallExpression_registerNewAttribute1)
		.def("registerNewAttribute", MetaCallExpression_registerNewAttribute2)
		.def("isSubtypeOf", MetaCallExpression_isSubtypeOf1)
		.def("isSubtypeOf", MetaCallExpression_isSubtypeOf2);
}

{
	bool (OOModel::UnfinishedOperator::*UnfinishedOperator_isSubtypeOf1)(
		const QString&) const = &OOModel::UnfinishedOperator::isSubtypeOf;
	bool (OOModel::UnfinishedOperator::*UnfinishedOperator_isSubtypeOf2)(
		int) const = &OOModel::UnfinishedOperator::isSubtypeOf;
	Model::CompositeIndex (*UnfinishedOperator_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::UnfinishedOperator::registerNewAttribute;
	Model::CompositeIndex (*UnfinishedOperator_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::UnfinishedOperator::registerNewAttribute;
	class_<OOModel::UnfinishedOperator, bases<OOModel::Expression>>("UnfinishedOperator")
		.add_property("delimiters",
			make_function(&OOModel::UnfinishedOperator::delimiters, return_internal_reference<>()),
			&OOModel::UnfinishedOperator::setDelimiters)
		.add_property("operands",
			make_function(&OOModel::UnfinishedOperator::operands, return_internal_reference<>()),
			&OOModel::UnfinishedOperator::setOperands)
		.def("typeName", make_function((const QString& (
		OOModel::UnfinishedOperator::*)())&OOModel::UnfinishedOperator::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::UnfinishedOperator::typeId)
		.def("hierarchyTypeIds", &OOModel::UnfinishedOperator::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::UnfinishedOperator::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::UnfinishedOperator::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::UnfinishedOperator::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::UnfinishedOperator::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::UnfinishedOperator::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::UnfinishedOperator::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::UnfinishedOperator::type)
		.def("isSubtypeOf", UnfinishedOperator_isSubtypeOf1)
		.def("isSubtypeOf", UnfinishedOperator_isSubtypeOf2)
		.def("registerNewAttribute", UnfinishedOperator_registerNewAttribute1)
		.def("registerNewAttribute", UnfinishedOperator_registerNewAttribute2);
}

{
	bool (OOModel::NullLiteral::*NullLiteral_isSubtypeOf1)(const QString&) const = &OOModel::NullLiteral::isSubtypeOf;
	bool (OOModel::NullLiteral::*NullLiteral_isSubtypeOf2)(int) const = &OOModel::NullLiteral::isSubtypeOf;
	Model::CompositeIndex (*NullLiteral_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::NullLiteral::registerNewAttribute;
	Model::CompositeIndex (*NullLiteral_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::NullLiteral::registerNewAttribute;
	class_<OOModel::NullLiteral, bases<OOModel::Expression>>("NullLiteral")
		.def("typeName", make_function((const QString& (OOModel::NullLiteral::*)(
			))&OOModel::NullLiteral::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::NullLiteral::typeId)
		.def("hierarchyTypeIds", &OOModel::NullLiteral::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::NullLiteral::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::NullLiteral::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::NullLiteral::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::NullLiteral::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::NullLiteral::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::NullLiteral::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::NullLiteral::type)
		.def("isSubtypeOf", NullLiteral_isSubtypeOf1)
		.def("isSubtypeOf", NullLiteral_isSubtypeOf2)
		.def("registerNewAttribute", NullLiteral_registerNewAttribute1)
		.def("registerNewAttribute", NullLiteral_registerNewAttribute2);
}

{
	Model::CompositeIndex (*FloatLiteral_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FloatLiteral::registerNewAttribute;
	Model::CompositeIndex (*FloatLiteral_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FloatLiteral::registerNewAttribute;
	bool (OOModel::FloatLiteral::*FloatLiteral_isSubtypeOf1)(const QString&) const = &OOModel::FloatLiteral::isSubtypeOf;
	bool (OOModel::FloatLiteral::*FloatLiteral_isSubtypeOf2)(int) const = &OOModel::FloatLiteral::isSubtypeOf;
	class_<OOModel::FloatLiteral, bases<OOModel::Expression>>("FloatLiteral")
		.add_property("value",
			make_function((const QString& (OOModel::FloatLiteral::*)(
				))&OOModel::FloatLiteral::value, return_value_policy<copy_const_reference>()),
			&OOModel::FloatLiteral::setValue)
		.def("typeName", make_function((const QString& (OOModel::FloatLiteral::*)(
			))&OOModel::FloatLiteral::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::FloatLiteral::typeId)
		.def("hierarchyTypeIds", &OOModel::FloatLiteral::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::FloatLiteral::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::FloatLiteral::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::FloatLiteral::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::FloatLiteral::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::FloatLiteral::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::FloatLiteral::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("valueNode", make_function(&OOModel::FloatLiteral::valueNode, return_internal_reference<>()))
		.def("type", &OOModel::FloatLiteral::type)
		.def("registerNewAttribute", FloatLiteral_registerNewAttribute1)
		.def("registerNewAttribute", FloatLiteral_registerNewAttribute2)
		.def("isSubtypeOf", FloatLiteral_isSubtypeOf1)
		.def("isSubtypeOf", FloatLiteral_isSubtypeOf2);
}

{
	Model::CompositeIndex (*TypeTraitExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::TypeTraitExpression::registerNewAttribute;
	Model::CompositeIndex (*TypeTraitExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::TypeTraitExpression::registerNewAttribute;
	bool (OOModel::TypeTraitExpression::*TypeTraitExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::TypeTraitExpression::isSubtypeOf;
	bool (OOModel::TypeTraitExpression::*TypeTraitExpression_isSubtypeOf2)(
		int) const = &OOModel::TypeTraitExpression::isSubtypeOf;
	scope TypeTraitExpressionscope = class_<OOModel::TypeTraitExpression,
		 bases<OOModel::Expression>>("TypeTraitExpression")
		.add_property("operand",
			make_function(&OOModel::TypeTraitExpression::operand, return_internal_reference<>()),
			&OOModel::TypeTraitExpression::setOperand)
		.add_property("typeTraitKind",
			&OOModel::TypeTraitExpression::typeTraitKind,
			&OOModel::TypeTraitExpression::setTypeTraitKind)
		.def("typeName", make_function((const QString& (
		OOModel::TypeTraitExpression::*)())&OOModel::TypeTraitExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::TypeTraitExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::TypeTraitExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::TypeTraitExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::TypeTraitExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::TypeTraitExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::TypeTraitExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::TypeTraitExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::TypeTraitExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::TypeTraitExpression::type)
		.def("registerNewAttribute", TypeTraitExpression_registerNewAttribute1)
		.def("registerNewAttribute", TypeTraitExpression_registerNewAttribute2)
		.def("isSubtypeOf", TypeTraitExpression_isSubtypeOf1)
		.def("isSubtypeOf", TypeTraitExpression_isSubtypeOf2);
	enum_<OOModel::TypeTraitExpression::TypeTraitKind>("TypeTraitKind")
		.value("SizeOf", OOModel::TypeTraitExpression::TypeTraitKind::SizeOf)
		.value("AlignOf", OOModel::TypeTraitExpression::TypeTraitKind::AlignOf)
		.value("TypeId", OOModel::TypeTraitExpression::TypeTraitKind::TypeId);
}

{
	bool (OOModel::SuperExpression::*SuperExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::SuperExpression::isSubtypeOf;
	bool (OOModel::SuperExpression::*SuperExpression_isSubtypeOf2)(int) const = &OOModel::SuperExpression::isSubtypeOf;
	Model::CompositeIndex (*SuperExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::SuperExpression::registerNewAttribute;
	Model::CompositeIndex (*SuperExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::SuperExpression::registerNewAttribute;
	class_<OOModel::SuperExpression, bases<OOModel::Expression>>("SuperExpression")
		.def("typeName", make_function((const QString& (
			OOModel::SuperExpression::*)())&OOModel::SuperExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::SuperExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::SuperExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::SuperExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::SuperExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::SuperExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::SuperExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::SuperExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::SuperExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::SuperExpression::type)
		.def("isSubtypeOf", SuperExpression_isSubtypeOf1)
		.def("isSubtypeOf", SuperExpression_isSubtypeOf2)
		.def("registerNewAttribute", SuperExpression_registerNewAttribute1)
		.def("registerNewAttribute", SuperExpression_registerNewAttribute2);
}

{
	bool (OOModel::ConditionalExpression::*ConditionalExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ConditionalExpression::isSubtypeOf;
	bool (OOModel::ConditionalExpression::*ConditionalExpression_isSubtypeOf2)(
		int) const = &OOModel::ConditionalExpression::isSubtypeOf;
	Model::CompositeIndex (*ConditionalExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ConditionalExpression::registerNewAttribute;
	Model::CompositeIndex (*ConditionalExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ConditionalExpression::registerNewAttribute;
	class_<OOModel::ConditionalExpression, bases<OOModel::Expression>>("ConditionalExpression")
		.add_property("condition",
			make_function(&OOModel::ConditionalExpression::condition, return_internal_reference<>()),
			&OOModel::ConditionalExpression::setCondition)
		.add_property("trueExpression",
			make_function(&OOModel::ConditionalExpression::trueExpression, return_internal_reference<>()),
			&OOModel::ConditionalExpression::setTrueExpression)
		.add_property("falseExpression",
			make_function(&OOModel::ConditionalExpression::falseExpression, return_internal_reference<>()),
			&OOModel::ConditionalExpression::setFalseExpression)
		.def("typeName", make_function((const QString& (
		OOModel::ConditionalExpression::*)())&OOModel::ConditionalExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ConditionalExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::ConditionalExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ConditionalExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ConditionalExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ConditionalExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ConditionalExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ConditionalExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ConditionalExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::ConditionalExpression::type)
		.def("isSubtypeOf", ConditionalExpression_isSubtypeOf1)
		.def("isSubtypeOf", ConditionalExpression_isSubtypeOf2)
		.def("registerNewAttribute", ConditionalExpression_registerNewAttribute1)
		.def("registerNewAttribute", ConditionalExpression_registerNewAttribute2);
}

{
	bool (OOModel::TypeNameOperator::*TypeNameOperator_isSubtypeOf1)(
		const QString&) const = &OOModel::TypeNameOperator::isSubtypeOf;
	bool (OOModel::TypeNameOperator::*TypeNameOperator_isSubtypeOf2)(int) const = &OOModel::TypeNameOperator::isSubtypeOf;
	Model::CompositeIndex (*TypeNameOperator_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::TypeNameOperator::registerNewAttribute;
	Model::CompositeIndex (*TypeNameOperator_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::TypeNameOperator::registerNewAttribute;
	class_<OOModel::TypeNameOperator, bases<OOModel::Expression>>("TypeNameOperator")
		.add_property("typeExpression",
			make_function(&OOModel::TypeNameOperator::typeExpression, return_internal_reference<>()),
			&OOModel::TypeNameOperator::setTypeExpression)
		.def("typeName", make_function((const QString& (
			OOModel::TypeNameOperator::*)())&OOModel::TypeNameOperator::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::TypeNameOperator::typeId)
		.def("hierarchyTypeIds", &OOModel::TypeNameOperator::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::TypeNameOperator::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::TypeNameOperator::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::TypeNameOperator::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::TypeNameOperator::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::TypeNameOperator::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::TypeNameOperator::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::TypeNameOperator::type)
		.def("isSubtypeOf", TypeNameOperator_isSubtypeOf1)
		.def("isSubtypeOf", TypeNameOperator_isSubtypeOf2)
		.def("registerNewAttribute", TypeNameOperator_registerNewAttribute1)
		.def("registerNewAttribute", TypeNameOperator_registerNewAttribute2);
}

{
	bool (OOModel::ThisExpression::*ThisExpression_isSubtypeOf1)(const
		QString&) const = &OOModel::ThisExpression::isSubtypeOf;
	bool (OOModel::ThisExpression::*ThisExpression_isSubtypeOf2)(int) const = &OOModel::ThisExpression::isSubtypeOf;
	Model::CompositeIndex (*ThisExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ThisExpression::registerNewAttribute;
	Model::CompositeIndex (*ThisExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ThisExpression::registerNewAttribute;
	class_<OOModel::ThisExpression, bases<OOModel::Expression>>("ThisExpression")
		.def("typeName", make_function((const QString& (
			OOModel::ThisExpression::*)())&OOModel::ThisExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ThisExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::ThisExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ThisExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ThisExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ThisExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ThisExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ThisExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ThisExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::ThisExpression::type)
		.def("isSubtypeOf", ThisExpression_isSubtypeOf1)
		.def("isSubtypeOf", ThisExpression_isSubtypeOf2)
		.def("registerNewAttribute", ThisExpression_registerNewAttribute1)
		.def("registerNewAttribute", ThisExpression_registerNewAttribute2);
}

{
	Model::CompositeIndex (*BinaryOperation_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::BinaryOperation::registerNewAttribute;
	Model::CompositeIndex (*BinaryOperation_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::BinaryOperation::registerNewAttribute;
	bool (OOModel::BinaryOperation::*BinaryOperation_isSubtypeOf1)(
		const QString&) const = &OOModel::BinaryOperation::isSubtypeOf;
	bool (OOModel::BinaryOperation::*BinaryOperation_isSubtypeOf2)(int) const = &OOModel::BinaryOperation::isSubtypeOf;
	scope BinaryOperationscope = class_<OOModel::BinaryOperation, bases<OOModel::Expression>>("BinaryOperation")
		.add_property("left",
			make_function(&OOModel::BinaryOperation::left, return_internal_reference<>()),
			&OOModel::BinaryOperation::setLeft)
		.add_property("right",
			make_function(&OOModel::BinaryOperation::right, return_internal_reference<>()),
			&OOModel::BinaryOperation::setRight)
		.add_property("op",
			&OOModel::BinaryOperation::op,
			&OOModel::BinaryOperation::setOp)
		.def("typeName", make_function((const QString& (
			OOModel::BinaryOperation::*)())&OOModel::BinaryOperation::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::BinaryOperation::typeId)
		.def("hierarchyTypeIds", &OOModel::BinaryOperation::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::BinaryOperation::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::BinaryOperation::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::BinaryOperation::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::BinaryOperation::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::BinaryOperation::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::BinaryOperation::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::BinaryOperation::type)
		.def("registerNewAttribute", BinaryOperation_registerNewAttribute1)
		.def("registerNewAttribute", BinaryOperation_registerNewAttribute2)
		.def("isSubtypeOf", BinaryOperation_isSubtypeOf1)
		.def("isSubtypeOf", BinaryOperation_isSubtypeOf2);
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

{
	bool (OOModel::EmptyExpression::*EmptyExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::EmptyExpression::isSubtypeOf;
	bool (OOModel::EmptyExpression::*EmptyExpression_isSubtypeOf2)(int) const = &OOModel::EmptyExpression::isSubtypeOf;
	Model::CompositeIndex (*EmptyExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::EmptyExpression::registerNewAttribute;
	Model::CompositeIndex (*EmptyExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::EmptyExpression::registerNewAttribute;
	class_<OOModel::EmptyExpression, bases<OOModel::Expression>>("EmptyExpression")
		.def("typeName", make_function((const QString& (
			OOModel::EmptyExpression::*)())&OOModel::EmptyExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::EmptyExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::EmptyExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::EmptyExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::EmptyExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::EmptyExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::EmptyExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::EmptyExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::EmptyExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::EmptyExpression::type)
		.def("isSubtypeOf", EmptyExpression_isSubtypeOf1)
		.def("isSubtypeOf", EmptyExpression_isSubtypeOf2)
		.def("registerNewAttribute", EmptyExpression_registerNewAttribute1)
		.def("registerNewAttribute", EmptyExpression_registerNewAttribute2);
}

{
	Model::CompositeIndex (*MethodCallExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MethodCallExpression::registerNewAttribute;
	Model::CompositeIndex (*MethodCallExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MethodCallExpression::registerNewAttribute;
	bool (OOModel::MethodCallExpression::*MethodCallExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::MethodCallExpression::isSubtypeOf;
	bool (OOModel::MethodCallExpression::*MethodCallExpression_isSubtypeOf2)(
		int) const = &OOModel::MethodCallExpression::isSubtypeOf;
	OOModel::Method* (OOModel::MethodCallExpression::*MethodCallExpression_methodDefinition1)(
		) = &OOModel::MethodCallExpression::methodDefinition;
	scope MethodCallExpressionscope = class_<OOModel::MethodCallExpression,
		 bases<OOModel::Expression>>("MethodCallExpression")
		.add_property("callee",
			make_function(&OOModel::MethodCallExpression::callee, return_internal_reference<>()),
			&OOModel::MethodCallExpression::setCallee)
		.add_property("arguments",
			make_function(&OOModel::MethodCallExpression::arguments, return_internal_reference<>()),
			&OOModel::MethodCallExpression::setArguments)
		.add_property("methodCallKind",
			&OOModel::MethodCallExpression::methodCallKind,
			&OOModel::MethodCallExpression::setMethodCallKind)
		.def("typeName", make_function((const QString& (
		OOModel::MethodCallExpression::*)())&OOModel::MethodCallExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::MethodCallExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::MethodCallExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::MethodCallExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::MethodCallExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::MethodCallExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::MethodCallExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::MethodCallExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::MethodCallExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::MethodCallExpression::type)
		.def("registerNewAttribute", MethodCallExpression_registerNewAttribute1)
		.def("registerNewAttribute", MethodCallExpression_registerNewAttribute2)
		.def("isSubtypeOf", MethodCallExpression_isSubtypeOf1)
		.def("isSubtypeOf", MethodCallExpression_isSubtypeOf2)
		.def("methodDefinition", make_function(MethodCallExpression_methodDefinition1, return_internal_reference<>()));
	enum_<OOModel::MethodCallExpression::MethodCallKind>("MethodCallKind")
		.value("Call", OOModel::MethodCallExpression::MethodCallKind::Call)
		.value("Construct", OOModel::MethodCallExpression::MethodCallKind::Construct);
}

{
	Model::CompositeIndex (*DeleteExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::DeleteExpression::registerNewAttribute;
	Model::CompositeIndex (*DeleteExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::DeleteExpression::registerNewAttribute;
	bool (OOModel::DeleteExpression::*DeleteExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::DeleteExpression::isSubtypeOf;
	bool (OOModel::DeleteExpression::*DeleteExpression_isSubtypeOf2)(int) const = &OOModel::DeleteExpression::isSubtypeOf;
	class_<OOModel::DeleteExpression, bases<OOModel::Expression>>("DeleteExpression")
		.add_property("expr",
			make_function(&OOModel::DeleteExpression::expr, return_internal_reference<>()),
			&OOModel::DeleteExpression::setExpr)
		.add_property("isArray",
			&OOModel::DeleteExpression::isArray,
			&OOModel::DeleteExpression::setIsArray)
		.def("typeName", make_function((const QString& (
			OOModel::DeleteExpression::*)())&OOModel::DeleteExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::DeleteExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::DeleteExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::DeleteExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::DeleteExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::DeleteExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::DeleteExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::DeleteExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::DeleteExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isArrayNode", make_function(&OOModel::DeleteExpression::isArrayNode, return_internal_reference<>()))
		.def("type", &OOModel::DeleteExpression::type)
		.def("registerNewAttribute", DeleteExpression_registerNewAttribute1)
		.def("registerNewAttribute", DeleteExpression_registerNewAttribute2)
		.def("isSubtypeOf", DeleteExpression_isSubtypeOf1)
		.def("isSubtypeOf", DeleteExpression_isSubtypeOf2);
}

{
	Model::CompositeIndex (*LambdaExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::LambdaExpression::registerNewAttribute;
	Model::CompositeIndex (*LambdaExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::LambdaExpression::registerNewAttribute;
	bool (OOModel::LambdaExpression::*LambdaExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::LambdaExpression::isSubtypeOf;
	bool (OOModel::LambdaExpression::*LambdaExpression_isSubtypeOf2)(int) const = &OOModel::LambdaExpression::isSubtypeOf;
	scope LambdaExpressionscope = class_<OOModel::LambdaExpression, bases<OOModel::Expression>>("LambdaExpression")
		.add_property("arguments",
			make_function(&OOModel::LambdaExpression::arguments, return_internal_reference<>()),
			&OOModel::LambdaExpression::setArguments)
		.add_property("results",
			make_function(&OOModel::LambdaExpression::results, return_internal_reference<>()),
			&OOModel::LambdaExpression::setResults)
		.add_property("captures",
			make_function(&OOModel::LambdaExpression::captures, return_internal_reference<>()),
			&OOModel::LambdaExpression::setCaptures)
		.add_property("body",
			make_function(&OOModel::LambdaExpression::body, return_internal_reference<>()),
			&OOModel::LambdaExpression::setBody)
		.add_property("defaultCaptureType",
			&OOModel::LambdaExpression::defaultCaptureType,
			&OOModel::LambdaExpression::setDefaultCaptureType)
		.def("typeName", make_function((const QString& (
			OOModel::LambdaExpression::*)())&OOModel::LambdaExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::LambdaExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::LambdaExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::LambdaExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::LambdaExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::LambdaExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::LambdaExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::LambdaExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::LambdaExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::LambdaExpression::type)
		.def("registerNewAttribute", LambdaExpression_registerNewAttribute1)
		.def("registerNewAttribute", LambdaExpression_registerNewAttribute2)
		.def("isSubtypeOf", LambdaExpression_isSubtypeOf1)
		.def("isSubtypeOf", LambdaExpression_isSubtypeOf2);
	enum_<OOModel::LambdaExpression::DefaultCaptureType>("DefaultCaptureType")
		.value("None", OOModel::LambdaExpression::DefaultCaptureType::None)
		.value("Value", OOModel::LambdaExpression::DefaultCaptureType::Value)
		.value("Reference", OOModel::LambdaExpression::DefaultCaptureType::Reference);
}

{
	Model::CompositeIndex (*IntegerLiteral_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::IntegerLiteral::registerNewAttribute;
	Model::CompositeIndex (*IntegerLiteral_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::IntegerLiteral::registerNewAttribute;
	bool (OOModel::IntegerLiteral::*IntegerLiteral_isSubtypeOf1)(const
		QString&) const = &OOModel::IntegerLiteral::isSubtypeOf;
	bool (OOModel::IntegerLiteral::*IntegerLiteral_isSubtypeOf2)(int) const = &OOModel::IntegerLiteral::isSubtypeOf;
	class_<OOModel::IntegerLiteral, bases<OOModel::Expression>>("IntegerLiteral")
		.add_property("value",
			make_function((const QString& (OOModel::IntegerLiteral::*)(
				))&OOModel::IntegerLiteral::value, return_value_policy<copy_const_reference>()),
			&OOModel::IntegerLiteral::setValue)
		.def("typeName", make_function((const QString& (
			OOModel::IntegerLiteral::*)())&OOModel::IntegerLiteral::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::IntegerLiteral::typeId)
		.def("hierarchyTypeIds", &OOModel::IntegerLiteral::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::IntegerLiteral::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::IntegerLiteral::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::IntegerLiteral::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::IntegerLiteral::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::IntegerLiteral::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::IntegerLiteral::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("valueNode", make_function(&OOModel::IntegerLiteral::valueNode, return_internal_reference<>()))
		.def("valueAsInt", &OOModel::IntegerLiteral::valueAsInt)
		.def("type", &OOModel::IntegerLiteral::type)
		.def("registerNewAttribute", IntegerLiteral_registerNewAttribute1)
		.def("registerNewAttribute", IntegerLiteral_registerNewAttribute2)
		.def("isSubtypeOf", IntegerLiteral_isSubtypeOf1)
		.def("isSubtypeOf", IntegerLiteral_isSubtypeOf2);
}

{
	Model::CompositeIndex (*VariableDeclarationExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::VariableDeclarationExpression::registerNewAttribute;
	Model::CompositeIndex (*VariableDeclarationExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::VariableDeclarationExpression::registerNewAttribute;
	bool (OOModel::VariableDeclarationExpression::*VariableDeclarationExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::VariableDeclarationExpression::isSubtypeOf;
	bool (OOModel::VariableDeclarationExpression::*VariableDeclarationExpression_isSubtypeOf2)(
		int) const = &OOModel::VariableDeclarationExpression::isSubtypeOf;
	class_<OOModel::VariableDeclarationExpression, bases<OOModel::Expression>>("VariableDeclarationExpression")
		.add_property("decl",
			make_function(&OOModel::VariableDeclarationExpression::decl, return_internal_reference<>()),
			&OOModel::VariableDeclarationExpression::setDecl)
		.def("typeName", make_function((const QString& (
		OOModel::VariableDeclarationExpression::*)(
			))&OOModel::VariableDeclarationExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::VariableDeclarationExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::VariableDeclarationExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(
			&OOModel::VariableDeclarationExpression::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::VariableDeclarationExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::VariableDeclarationExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::VariableDeclarationExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::VariableDeclarationExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::VariableDeclarationExpression::getMetaData,
			 return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("definesSymbol", &OOModel::VariableDeclarationExpression::definesSymbol)
		.def("symbolName", make_function((const QString& (
		OOModel::VariableDeclarationExpression::*)(
			))&OOModel::VariableDeclarationExpression::symbolName, return_value_policy<copy_const_reference>()))
		.def("symbolType", &OOModel::VariableDeclarationExpression::symbolType)
		.def("type", &OOModel::VariableDeclarationExpression::type)
		.def("registerNewAttribute", VariableDeclarationExpression_registerNewAttribute1)
		.def("registerNewAttribute", VariableDeclarationExpression_registerNewAttribute2)
		.def("isSubtypeOf", VariableDeclarationExpression_isSubtypeOf1)
		.def("isSubtypeOf", VariableDeclarationExpression_isSubtypeOf2);
}

{
	Model::CompositeIndex (*StringLiteral_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::StringLiteral::registerNewAttribute;
	Model::CompositeIndex (*StringLiteral_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::StringLiteral::registerNewAttribute;
	bool (OOModel::StringLiteral::*StringLiteral_isSubtypeOf1)(
		const QString&) const = &OOModel::StringLiteral::isSubtypeOf;
	bool (OOModel::StringLiteral::*StringLiteral_isSubtypeOf2)(int) const = &OOModel::StringLiteral::isSubtypeOf;
	class_<OOModel::StringLiteral, bases<OOModel::Expression>>("StringLiteral")
		.add_property("value",
			make_function((const QString& (OOModel::StringLiteral::*)(
				))&OOModel::StringLiteral::value, return_value_policy<copy_const_reference>()),
			&OOModel::StringLiteral::setValue)
		.def("typeName", make_function((const QString& (
			OOModel::StringLiteral::*)())&OOModel::StringLiteral::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::StringLiteral::typeId)
		.def("hierarchyTypeIds", &OOModel::StringLiteral::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::StringLiteral::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::StringLiteral::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::StringLiteral::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::StringLiteral::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::StringLiteral::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::StringLiteral::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("valueNode", make_function(&OOModel::StringLiteral::valueNode, return_internal_reference<>()))
		.def("type", &OOModel::StringLiteral::type)
		.def("registerNewAttribute", StringLiteral_registerNewAttribute1)
		.def("registerNewAttribute", StringLiteral_registerNewAttribute2)
		.def("isSubtypeOf", StringLiteral_isSubtypeOf1)
		.def("isSubtypeOf", StringLiteral_isSubtypeOf2);
}

{
	bool (OOModel::InstanceOfExpression::*InstanceOfExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::InstanceOfExpression::isSubtypeOf;
	bool (OOModel::InstanceOfExpression::*InstanceOfExpression_isSubtypeOf2)(
		int) const = &OOModel::InstanceOfExpression::isSubtypeOf;
	Model::CompositeIndex (*InstanceOfExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::InstanceOfExpression::registerNewAttribute;
	Model::CompositeIndex (*InstanceOfExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::InstanceOfExpression::registerNewAttribute;
	class_<OOModel::InstanceOfExpression, bases<OOModel::Expression>>("InstanceOfExpression")
		.add_property("expr",
			make_function(&OOModel::InstanceOfExpression::expr, return_internal_reference<>()),
			&OOModel::InstanceOfExpression::setExpr)
		.add_property("typeExpression",
			make_function(&OOModel::InstanceOfExpression::typeExpression, return_internal_reference<>()),
			&OOModel::InstanceOfExpression::setTypeExpression)
		.def("typeName", make_function((const QString& (
		OOModel::InstanceOfExpression::*)())&OOModel::InstanceOfExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::InstanceOfExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::InstanceOfExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::InstanceOfExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::InstanceOfExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::InstanceOfExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::InstanceOfExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::InstanceOfExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::InstanceOfExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::InstanceOfExpression::type)
		.def("isSubtypeOf", InstanceOfExpression_isSubtypeOf1)
		.def("isSubtypeOf", InstanceOfExpression_isSubtypeOf2)
		.def("registerNewAttribute", InstanceOfExpression_registerNewAttribute1)
		.def("registerNewAttribute", InstanceOfExpression_registerNewAttribute2);
}

{
	Model::CompositeIndex (*BooleanLiteral_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::BooleanLiteral::registerNewAttribute;
	Model::CompositeIndex (*BooleanLiteral_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::BooleanLiteral::registerNewAttribute;
	bool (OOModel::BooleanLiteral::*BooleanLiteral_isSubtypeOf1)(const
		QString&) const = &OOModel::BooleanLiteral::isSubtypeOf;
	bool (OOModel::BooleanLiteral::*BooleanLiteral_isSubtypeOf2)(int) const = &OOModel::BooleanLiteral::isSubtypeOf;
	class_<OOModel::BooleanLiteral, bases<OOModel::Expression>>("BooleanLiteral")
		.add_property("value",
			&OOModel::BooleanLiteral::value,
			&OOModel::BooleanLiteral::setValue)
		.def("typeName", make_function((const QString& (
			OOModel::BooleanLiteral::*)())&OOModel::BooleanLiteral::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::BooleanLiteral::typeId)
		.def("hierarchyTypeIds", &OOModel::BooleanLiteral::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::BooleanLiteral::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::BooleanLiteral::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::BooleanLiteral::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::BooleanLiteral::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::BooleanLiteral::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::BooleanLiteral::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("valueNode", make_function(&OOModel::BooleanLiteral::valueNode, return_internal_reference<>()))
		.def("type", &OOModel::BooleanLiteral::type)
		.def("registerNewAttribute", BooleanLiteral_registerNewAttribute1)
		.def("registerNewAttribute", BooleanLiteral_registerNewAttribute2)
		.def("isSubtypeOf", BooleanLiteral_isSubtypeOf1)
		.def("isSubtypeOf", BooleanLiteral_isSubtypeOf2);
}

{
	bool (OOModel::TypeExpression::*TypeExpression_isSubtypeOf1)(const
		QString&) const = &OOModel::TypeExpression::isSubtypeOf;
	bool (OOModel::TypeExpression::*TypeExpression_isSubtypeOf2)(int) const = &OOModel::TypeExpression::isSubtypeOf;
	Model::CompositeIndex (*TypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::TypeExpression::registerNewAttribute;
	Model::CompositeIndex (*TypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::TypeExpression::registerNewAttribute;
	class_<OOModel::TypeExpression, bases<OOModel::Expression>>("TypeExpression")
		.def("typeName", make_function((const QString& (
			OOModel::TypeExpression::*)())&OOModel::TypeExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::TypeExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::TypeExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::TypeExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::TypeExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::TypeExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::TypeExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::TypeExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::TypeExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", TypeExpression_isSubtypeOf1)
		.def("isSubtypeOf", TypeExpression_isSubtypeOf2)
		.def("registerNewAttribute", TypeExpression_registerNewAttribute1)
		.def("registerNewAttribute", TypeExpression_registerNewAttribute2);
}

{
	Model::CompositeIndex (*ReferenceExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ReferenceExpression::registerNewAttribute;
	Model::CompositeIndex (*ReferenceExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ReferenceExpression::registerNewAttribute;
	bool (OOModel::ReferenceExpression::*ReferenceExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ReferenceExpression::isSubtypeOf;
	bool (OOModel::ReferenceExpression::*ReferenceExpression_isSubtypeOf2)(
		int) const = &OOModel::ReferenceExpression::isSubtypeOf;
	scope ReferenceExpressionscope = class_<OOModel::ReferenceExpression,
		 bases<OOModel::Expression>>("ReferenceExpression")
		.add_property("prefix",
			make_function(&OOModel::ReferenceExpression::prefix, return_internal_reference<>()),
			&OOModel::ReferenceExpression::setPrefix)
		.add_property("ref",
			make_function(&OOModel::ReferenceExpression::ref, return_internal_reference<>()),
			&OOModel::ReferenceExpression::setRef)
		.add_property("typeArguments",
			make_function(&OOModel::ReferenceExpression::typeArguments, return_internal_reference<>()),
			&OOModel::ReferenceExpression::setTypeArguments)
		.add_property("memberKind",
			&OOModel::ReferenceExpression::memberKind,
			&OOModel::ReferenceExpression::setMemberKind)
		.add_property("name",
			make_function((const QString& (OOModel::ReferenceExpression::*)(
				))&OOModel::ReferenceExpression::name, return_value_policy<copy_const_reference>()),
			&OOModel::ReferenceExpression::setName)
		.def("typeName", make_function((const QString& (
		OOModel::ReferenceExpression::*)())&OOModel::ReferenceExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ReferenceExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::ReferenceExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ReferenceExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ReferenceExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ReferenceExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ReferenceExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ReferenceExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ReferenceExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("target", make_function(&OOModel::ReferenceExpression::target, return_internal_reference<>()))
		.def("type", &OOModel::ReferenceExpression::type)
		.def("registerNewAttribute", ReferenceExpression_registerNewAttribute1)
		.def("registerNewAttribute", ReferenceExpression_registerNewAttribute2)
		.def("isSubtypeOf", ReferenceExpression_isSubtypeOf1)
		.def("isSubtypeOf", ReferenceExpression_isSubtypeOf2);
	enum_<OOModel::ReferenceExpression::MemberKind>("MemberKind")
		.value("Dot", OOModel::ReferenceExpression::MemberKind::Dot)
		.value("Pointer", OOModel::ReferenceExpression::MemberKind::Pointer)
		.value("Static", OOModel::ReferenceExpression::MemberKind::Static)
		.value("Template", OOModel::ReferenceExpression::MemberKind::Template);
}

{
	bool (OOModel::ArrayInitializer::*ArrayInitializer_isSubtypeOf1)(
		const QString&) const = &OOModel::ArrayInitializer::isSubtypeOf;
	bool (OOModel::ArrayInitializer::*ArrayInitializer_isSubtypeOf2)(int) const = &OOModel::ArrayInitializer::isSubtypeOf;
	Model::CompositeIndex (*ArrayInitializer_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ArrayInitializer::registerNewAttribute;
	Model::CompositeIndex (*ArrayInitializer_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ArrayInitializer::registerNewAttribute;
	class_<OOModel::ArrayInitializer, bases<OOModel::Expression>>("ArrayInitializer")
		.add_property("values",
			make_function(&OOModel::ArrayInitializer::values, return_internal_reference<>()),
			&OOModel::ArrayInitializer::setValues)
		.def("typeName", make_function((const QString& (
			OOModel::ArrayInitializer::*)())&OOModel::ArrayInitializer::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ArrayInitializer::typeId)
		.def("hierarchyTypeIds", &OOModel::ArrayInitializer::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ArrayInitializer::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ArrayInitializer::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ArrayInitializer::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ArrayInitializer::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ArrayInitializer::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ArrayInitializer::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::ArrayInitializer::type)
		.def("isSubtypeOf", ArrayInitializer_isSubtypeOf1)
		.def("isSubtypeOf", ArrayInitializer_isSubtypeOf2)
		.def("registerNewAttribute", ArrayInitializer_registerNewAttribute1)
		.def("registerNewAttribute", ArrayInitializer_registerNewAttribute2);
}

{
	bool (OOModel::ThrowExpression::*ThrowExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ThrowExpression::isSubtypeOf;
	bool (OOModel::ThrowExpression::*ThrowExpression_isSubtypeOf2)(int) const = &OOModel::ThrowExpression::isSubtypeOf;
	Model::CompositeIndex (*ThrowExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ThrowExpression::registerNewAttribute;
	Model::CompositeIndex (*ThrowExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ThrowExpression::registerNewAttribute;
	class_<OOModel::ThrowExpression, bases<OOModel::Expression>>("ThrowExpression")
		.add_property("expr",
			make_function(&OOModel::ThrowExpression::expr, return_internal_reference<>()),
			&OOModel::ThrowExpression::setExpr)
		.def("typeName", make_function((const QString& (
			OOModel::ThrowExpression::*)())&OOModel::ThrowExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ThrowExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::ThrowExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ThrowExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ThrowExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ThrowExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ThrowExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ThrowExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ThrowExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::ThrowExpression::type)
		.def("isSubtypeOf", ThrowExpression_isSubtypeOf1)
		.def("isSubtypeOf", ThrowExpression_isSubtypeOf2)
		.def("registerNewAttribute", ThrowExpression_registerNewAttribute1)
		.def("registerNewAttribute", ThrowExpression_registerNewAttribute2);
}

{
	Model::CompositeIndex (*ErrorExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ErrorExpression::registerNewAttribute;
	Model::CompositeIndex (*ErrorExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ErrorExpression::registerNewAttribute;
	bool (OOModel::ErrorExpression::*ErrorExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ErrorExpression::isSubtypeOf;
	bool (OOModel::ErrorExpression::*ErrorExpression_isSubtypeOf2)(int) const = &OOModel::ErrorExpression::isSubtypeOf;
	class_<OOModel::ErrorExpression, bases<OOModel::Expression>>("ErrorExpression")
		.add_property("prefix",
			make_function((const QString& (OOModel::ErrorExpression::*)(
				))&OOModel::ErrorExpression::prefix, return_value_policy<copy_const_reference>()),
			&OOModel::ErrorExpression::setPrefix)
		.add_property("arg",
			make_function(&OOModel::ErrorExpression::arg, return_internal_reference<>()),
			&OOModel::ErrorExpression::setArg)
		.add_property("postfix",
			make_function((const QString& (OOModel::ErrorExpression::*)(
				))&OOModel::ErrorExpression::postfix, return_value_policy<copy_const_reference>()),
			&OOModel::ErrorExpression::setPostfix)
		.def("typeName", make_function((const QString& (
			OOModel::ErrorExpression::*)())&OOModel::ErrorExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ErrorExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::ErrorExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ErrorExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ErrorExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ErrorExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ErrorExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ErrorExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ErrorExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("prefixNode", make_function(&OOModel::ErrorExpression::prefixNode, return_internal_reference<>()))
		.def("postfixNode", make_function(&OOModel::ErrorExpression::postfixNode, return_internal_reference<>()))
		.def("type", &OOModel::ErrorExpression::type)
		.def("registerNewAttribute", ErrorExpression_registerNewAttribute1)
		.def("registerNewAttribute", ErrorExpression_registerNewAttribute2)
		.def("isSubtypeOf", ErrorExpression_isSubtypeOf1)
		.def("isSubtypeOf", ErrorExpression_isSubtypeOf2);
}

{
	Model::CompositeIndex (*AssignmentExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::AssignmentExpression::registerNewAttribute;
	Model::CompositeIndex (*AssignmentExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::AssignmentExpression::registerNewAttribute;
	bool (OOModel::AssignmentExpression::*AssignmentExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::AssignmentExpression::isSubtypeOf;
	bool (OOModel::AssignmentExpression::*AssignmentExpression_isSubtypeOf2)(
		int) const = &OOModel::AssignmentExpression::isSubtypeOf;
	scope AssignmentExpressionscope = class_<OOModel::AssignmentExpression,
		 bases<OOModel::Expression>>("AssignmentExpression")
		.add_property("left",
			make_function(&OOModel::AssignmentExpression::left, return_internal_reference<>()),
			&OOModel::AssignmentExpression::setLeft)
		.add_property("right",
			make_function(&OOModel::AssignmentExpression::right, return_internal_reference<>()),
			&OOModel::AssignmentExpression::setRight)
		.add_property("op",
			&OOModel::AssignmentExpression::op,
			&OOModel::AssignmentExpression::setOp)
		.def("typeName", make_function((const QString& (
		OOModel::AssignmentExpression::*)())&OOModel::AssignmentExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::AssignmentExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::AssignmentExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::AssignmentExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::AssignmentExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::AssignmentExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::AssignmentExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::AssignmentExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::AssignmentExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::AssignmentExpression::type)
		.def("registerNewAttribute", AssignmentExpression_registerNewAttribute1)
		.def("registerNewAttribute", AssignmentExpression_registerNewAttribute2)
		.def("isSubtypeOf", AssignmentExpression_isSubtypeOf1)
		.def("isSubtypeOf", AssignmentExpression_isSubtypeOf2);
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

{
	bool (OOModel::GlobalScopeExpression::*GlobalScopeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::GlobalScopeExpression::isSubtypeOf;
	bool (OOModel::GlobalScopeExpression::*GlobalScopeExpression_isSubtypeOf2)(
		int) const = &OOModel::GlobalScopeExpression::isSubtypeOf;
	Model::CompositeIndex (*GlobalScopeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::GlobalScopeExpression::registerNewAttribute;
	Model::CompositeIndex (*GlobalScopeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::GlobalScopeExpression::registerNewAttribute;
	class_<OOModel::GlobalScopeExpression, bases<OOModel::Expression>>("GlobalScopeExpression")
		.def("typeName", make_function((const QString& (
		OOModel::GlobalScopeExpression::*)())&OOModel::GlobalScopeExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::GlobalScopeExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::GlobalScopeExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::GlobalScopeExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::GlobalScopeExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::GlobalScopeExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::GlobalScopeExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::GlobalScopeExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::GlobalScopeExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::GlobalScopeExpression::type)
		.def("isSubtypeOf", GlobalScopeExpression_isSubtypeOf1)
		.def("isSubtypeOf", GlobalScopeExpression_isSubtypeOf2)
		.def("registerNewAttribute", GlobalScopeExpression_registerNewAttribute1)
		.def("registerNewAttribute", GlobalScopeExpression_registerNewAttribute2);
}

{
	Model::CompositeIndex (*CharacterLiteral_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::CharacterLiteral::registerNewAttribute;
	Model::CompositeIndex (*CharacterLiteral_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::CharacterLiteral::registerNewAttribute;
	bool (OOModel::CharacterLiteral::*CharacterLiteral_isSubtypeOf1)(
		const QString&) const = &OOModel::CharacterLiteral::isSubtypeOf;
	bool (OOModel::CharacterLiteral::*CharacterLiteral_isSubtypeOf2)(int) const = &OOModel::CharacterLiteral::isSubtypeOf;
	class_<OOModel::CharacterLiteral, bases<OOModel::Expression>>("CharacterLiteral")
		.add_property("value",
			make_function((const QString& (
				OOModel::CharacterLiteral::*)())&OOModel::CharacterLiteral::value, return_value_policy<copy_const_reference>()),
			&OOModel::CharacterLiteral::setValue)
		.def("typeName", make_function((const QString& (
			OOModel::CharacterLiteral::*)())&OOModel::CharacterLiteral::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::CharacterLiteral::typeId)
		.def("hierarchyTypeIds", &OOModel::CharacterLiteral::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::CharacterLiteral::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::CharacterLiteral::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::CharacterLiteral::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::CharacterLiteral::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::CharacterLiteral::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::CharacterLiteral::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("valueNode", make_function(&OOModel::CharacterLiteral::valueNode, return_internal_reference<>()))
		.def("type", &OOModel::CharacterLiteral::type)
		.def("registerNewAttribute", CharacterLiteral_registerNewAttribute1)
		.def("registerNewAttribute", CharacterLiteral_registerNewAttribute2)
		.def("isSubtypeOf", CharacterLiteral_isSubtypeOf1)
		.def("isSubtypeOf", CharacterLiteral_isSubtypeOf2);
}

{
	Model::CompositeIndex (*CastExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::CastExpression::registerNewAttribute;
	Model::CompositeIndex (*CastExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::CastExpression::registerNewAttribute;
	bool (OOModel::CastExpression::*CastExpression_isSubtypeOf1)(const
		QString&) const = &OOModel::CastExpression::isSubtypeOf;
	bool (OOModel::CastExpression::*CastExpression_isSubtypeOf2)(int) const = &OOModel::CastExpression::isSubtypeOf;
	scope CastExpressionscope = class_<OOModel::CastExpression, bases<OOModel::Expression>>("CastExpression")
		.add_property("expr",
			make_function(&OOModel::CastExpression::expr, return_internal_reference<>()),
			&OOModel::CastExpression::setExpr)
		.add_property("type",
			&OOModel::CastExpression::type,
			&OOModel::CastExpression::setType)
		.add_property("castKind",
			&OOModel::CastExpression::castKind,
			&OOModel::CastExpression::setCastKind)
		.def("typeName", make_function((const QString& (
			OOModel::CastExpression::*)())&OOModel::CastExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::CastExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::CastExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::CastExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::CastExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::CastExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::CastExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::CastExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::CastExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("castType", make_function(&OOModel::CastExpression::castType, return_internal_reference<>()))
		.def("registerNewAttribute", CastExpression_registerNewAttribute1)
		.def("registerNewAttribute", CastExpression_registerNewAttribute2)
		.def("isSubtypeOf", CastExpression_isSubtypeOf1)
		.def("isSubtypeOf", CastExpression_isSubtypeOf2);
	enum_<OOModel::CastExpression::CastKind>("CastKind")
		.value("Default", OOModel::CastExpression::CastKind::Default)
		.value("ConstCast", OOModel::CastExpression::CastKind::ConstCast)
		.value("DynamicCast", OOModel::CastExpression::CastKind::DynamicCast)
		.value("ReinterpretCast", OOModel::CastExpression::CastKind::ReinterpretCast)
		.value("StaticCast", OOModel::CastExpression::CastKind::StaticCast)
		.value("FunctionalCast", OOModel::CastExpression::CastKind::FunctionalCast);
}

{
	bool (OOModel::NewExpression::*NewExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::NewExpression::isSubtypeOf;
	bool (OOModel::NewExpression::*NewExpression_isSubtypeOf2)(int) const = &OOModel::NewExpression::isSubtypeOf;
	Model::CompositeIndex (*NewExpression_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::NewExpression::registerNewAttribute;
	Model::CompositeIndex (*NewExpression_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::NewExpression::registerNewAttribute;
	class_<OOModel::NewExpression, bases<OOModel::Expression>>("NewExpression")
		.add_property("newType",
			make_function(&OOModel::NewExpression::newType, return_internal_reference<>()),
			&OOModel::NewExpression::setNewType)
		.add_property("dimensions",
			make_function(&OOModel::NewExpression::dimensions, return_internal_reference<>()),
			&OOModel::NewExpression::setDimensions)
		.add_property("initializer",
			make_function(&OOModel::NewExpression::initializer, return_internal_reference<>()),
			&OOModel::NewExpression::setInitializer)
		.def("typeName", make_function((const QString& (
			OOModel::NewExpression::*)())&OOModel::NewExpression::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::NewExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::NewExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::NewExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::NewExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::NewExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::NewExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::NewExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::NewExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::NewExpression::type)
		.def("isSubtypeOf", NewExpression_isSubtypeOf1)
		.def("isSubtypeOf", NewExpression_isSubtypeOf2)
		.def("registerNewAttribute", NewExpression_registerNewAttribute1)
		.def("registerNewAttribute", NewExpression_registerNewAttribute2);
}

{
	Model::CompositeIndex (*Project_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::Project::registerNewAttribute;
	Model::CompositeIndex (*Project_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::Project::registerNewAttribute;
	bool (OOModel::Project::*Project_isSubtypeOf1)(const QString&) const = &OOModel::Project::isSubtypeOf;
	bool (OOModel::Project::*Project_isSubtypeOf2)(int) const = &OOModel::Project::isSubtypeOf;
	class_<OOModel::Project, bases<OOModel::Declaration>>("Project")
		.add_property("projects",
			make_function(&OOModel::Project::projects, return_internal_reference<>()),
			&OOModel::Project::setProjects)
		.add_property("modules",
			make_function(&OOModel::Project::modules, return_internal_reference<>()),
			&OOModel::Project::setModules)
		.add_property("classes",
			make_function(&OOModel::Project::classes, return_internal_reference<>()),
			&OOModel::Project::setClasses)
		.add_property("methods",
			make_function(&OOModel::Project::methods, return_internal_reference<>()),
			&OOModel::Project::setMethods)
		.add_property("fields",
			make_function(&OOModel::Project::fields, return_internal_reference<>()),
			&OOModel::Project::setFields)
		.add_property("libraries",
			make_function(&OOModel::Project::libraries, return_internal_reference<>()),
			&OOModel::Project::setLibraries)
		.add_property("implicitBaseType",
			make_function(&OOModel::Project::implicitBaseType, return_internal_reference<>()),
			&OOModel::Project::setImplicitBaseType)
		.add_property("implicitEnumType",
			make_function(&OOModel::Project::implicitEnumType, return_internal_reference<>()),
			&OOModel::Project::setImplicitEnumType)
		.add_property("implicitArrayType",
			make_function(&OOModel::Project::implicitArrayType, return_internal_reference<>()),
			&OOModel::Project::setImplicitArrayType)
		.def("typeName", make_function((const QString& (OOModel::Project::*)(
			))&OOModel::Project::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Project::typeId)
		.def("hierarchyTypeIds", &OOModel::Project::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Project::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Project::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Project::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Project::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&OOModel::Project::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::Project::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isTransparentForNameResolution", &OOModel::Project::isTransparentForNameResolution)
		.def("symbolType", &OOModel::Project::symbolType)
		.def("usedLibraries", &OOModel::Project::usedLibraries)
		.def("isNewPersistenceUnit", &OOModel::Project::isNewPersistenceUnit)
		.def("registerNewAttribute", Project_registerNewAttribute1)
		.def("registerNewAttribute", Project_registerNewAttribute2)
		.def("isSubtypeOf", Project_isSubtypeOf1)
		.def("isSubtypeOf", Project_isSubtypeOf2);
}

{
	bool (OOModel::ExplicitTemplateInstantiation::*ExplicitTemplateInstantiation_isSubtypeOf1)(
		const QString&) const = &OOModel::ExplicitTemplateInstantiation::isSubtypeOf;
	bool (OOModel::ExplicitTemplateInstantiation::*ExplicitTemplateInstantiation_isSubtypeOf2)(
		int) const = &OOModel::ExplicitTemplateInstantiation::isSubtypeOf;
	Model::CompositeIndex (*ExplicitTemplateInstantiation_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ExplicitTemplateInstantiation::registerNewAttribute;
	Model::CompositeIndex (*ExplicitTemplateInstantiation_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ExplicitTemplateInstantiation::registerNewAttribute;
	class_<OOModel::ExplicitTemplateInstantiation, bases<OOModel::Declaration>>("ExplicitTemplateInstantiation")
		.add_property("instantiatedClass",
			make_function(&OOModel::ExplicitTemplateInstantiation::instantiatedClass, return_internal_reference<>()),
			&OOModel::ExplicitTemplateInstantiation::setInstantiatedClass)
		.def("typeName", make_function((const QString& (
		OOModel::ExplicitTemplateInstantiation::*)(
			))&OOModel::ExplicitTemplateInstantiation::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ExplicitTemplateInstantiation::typeId)
		.def("hierarchyTypeIds", &OOModel::ExplicitTemplateInstantiation::hierarchyTypeIds)
		.def("typeNameStatic", make_function(
			&OOModel::ExplicitTemplateInstantiation::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ExplicitTemplateInstantiation::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ExplicitTemplateInstantiation::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ExplicitTemplateInstantiation::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ExplicitTemplateInstantiation::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ExplicitTemplateInstantiation::getMetaData,
			 return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("symbolType", &OOModel::ExplicitTemplateInstantiation::symbolType)
		.def("isSubtypeOf", ExplicitTemplateInstantiation_isSubtypeOf1)
		.def("isSubtypeOf", ExplicitTemplateInstantiation_isSubtypeOf2)
		.def("registerNewAttribute", ExplicitTemplateInstantiation_registerNewAttribute1)
		.def("registerNewAttribute", ExplicitTemplateInstantiation_registerNewAttribute2);
}

{
	bool (OOModel::MetaBinding::*MetaBinding_isSubtypeOf1)(const QString&) const = &OOModel::MetaBinding::isSubtypeOf;
	bool (OOModel::MetaBinding::*MetaBinding_isSubtypeOf2)(int) const = &OOModel::MetaBinding::isSubtypeOf;
	Model::CompositeIndex (*MetaBinding_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MetaBinding::registerNewAttribute;
	Model::CompositeIndex (*MetaBinding_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MetaBinding::registerNewAttribute;
	class_<OOModel::MetaBinding, bases<OOModel::Declaration>>("MetaBinding")
		.add_property("input",
			make_function(&OOModel::MetaBinding::input, return_internal_reference<>()),
			&OOModel::MetaBinding::setInput)
		.add_property("mappings",
			make_function(&OOModel::MetaBinding::mappings, return_internal_reference<>()),
			&OOModel::MetaBinding::setMappings)
		.def("typeName", make_function((const QString& (OOModel::MetaBinding::*)(
			))&OOModel::MetaBinding::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::MetaBinding::typeId)
		.def("hierarchyTypeIds", &OOModel::MetaBinding::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::MetaBinding::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::MetaBinding::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::MetaBinding::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::MetaBinding::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::MetaBinding::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::MetaBinding::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("symbolType", &OOModel::MetaBinding::symbolType)
		.def("isSubtypeOf", MetaBinding_isSubtypeOf1)
		.def("isSubtypeOf", MetaBinding_isSubtypeOf2)
		.def("registerNewAttribute", MetaBinding_registerNewAttribute1)
		.def("registerNewAttribute", MetaBinding_registerNewAttribute2);
}

{
	Model::CompositeIndex (*NameImport_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::NameImport::registerNewAttribute;
	Model::CompositeIndex (*NameImport_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::NameImport::registerNewAttribute;
	bool (OOModel::NameImport::*NameImport_isSubtypeOf1)(const QString&) const = &OOModel::NameImport::isSubtypeOf;
	bool (OOModel::NameImport::*NameImport_isSubtypeOf2)(int) const = &OOModel::NameImport::isSubtypeOf;
	class_<OOModel::NameImport, bases<OOModel::Declaration>>("NameImport")
		.add_property("importedName",
			make_function(&OOModel::NameImport::importedName, return_internal_reference<>()),
			&OOModel::NameImport::setImportedName)
		.add_property("importAll",
			&OOModel::NameImport::importAll,
			&OOModel::NameImport::setImportAll)
		.def("typeName", make_function((const QString& (OOModel::NameImport::*)(
			))&OOModel::NameImport::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::NameImport::typeId)
		.def("hierarchyTypeIds", &OOModel::NameImport::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::NameImport::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::NameImport::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::NameImport::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::NameImport::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::NameImport::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::NameImport::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("importAllNode", make_function(&OOModel::NameImport::importAllNode, return_internal_reference<>()))
		.def("definesSymbol", &OOModel::NameImport::definesSymbol)
		.def("findSymbols", &OOModel::NameImport::findSymbols)
		.def("registerNewAttribute", NameImport_registerNewAttribute1)
		.def("registerNewAttribute", NameImport_registerNewAttribute2)
		.def("isSubtypeOf", NameImport_isSubtypeOf1)
		.def("isSubtypeOf", NameImport_isSubtypeOf2);
}

{
	bool (OOModel::VariableDeclaration::*VariableDeclaration_isSubtypeOf1)(
		const QString&) const = &OOModel::VariableDeclaration::isSubtypeOf;
	bool (OOModel::VariableDeclaration::*VariableDeclaration_isSubtypeOf2)(
		int) const = &OOModel::VariableDeclaration::isSubtypeOf;
	Model::CompositeIndex (*VariableDeclaration_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::VariableDeclaration::registerNewAttribute;
	Model::CompositeIndex (*VariableDeclaration_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::VariableDeclaration::registerNewAttribute;
	class_<OOModel::VariableDeclaration, bases<OOModel::Declaration>>("VariableDeclaration")
		.add_property("typeExpression",
			make_function(&OOModel::VariableDeclaration::typeExpression, return_internal_reference<>()),
			&OOModel::VariableDeclaration::setTypeExpression)
		.add_property("initialValue",
			make_function(&OOModel::VariableDeclaration::initialValue, return_internal_reference<>()),
			&OOModel::VariableDeclaration::setInitialValue)
		.def("typeName", make_function((const QString& (
		OOModel::VariableDeclaration::*)())&OOModel::VariableDeclaration::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::VariableDeclaration::typeId)
		.def("hierarchyTypeIds", &OOModel::VariableDeclaration::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::VariableDeclaration::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::VariableDeclaration::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::VariableDeclaration::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::VariableDeclaration::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::VariableDeclaration::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::VariableDeclaration::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("symbolType", &OOModel::VariableDeclaration::symbolType)
		.def("isSubtypeOf", VariableDeclaration_isSubtypeOf1)
		.def("isSubtypeOf", VariableDeclaration_isSubtypeOf2)
		.def("registerNewAttribute", VariableDeclaration_registerNewAttribute1)
		.def("registerNewAttribute", VariableDeclaration_registerNewAttribute2);
}

{
	bool (OOModel::MetaCallMapping::*MetaCallMapping_isSubtypeOf1)(
		const QString&) const = &OOModel::MetaCallMapping::isSubtypeOf;
	bool (OOModel::MetaCallMapping::*MetaCallMapping_isSubtypeOf2)(int) const = &OOModel::MetaCallMapping::isSubtypeOf;
	Model::CompositeIndex (*MetaCallMapping_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MetaCallMapping::registerNewAttribute;
	Model::CompositeIndex (*MetaCallMapping_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MetaCallMapping::registerNewAttribute;
	class_<OOModel::MetaCallMapping, bases<OOModel::Declaration>>("MetaCallMapping")
		.add_property("value",
			make_function(&OOModel::MetaCallMapping::value, return_internal_reference<>()),
			&OOModel::MetaCallMapping::setValue)
		.def("typeName", make_function((const QString& (
			OOModel::MetaCallMapping::*)())&OOModel::MetaCallMapping::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::MetaCallMapping::typeId)
		.def("hierarchyTypeIds", &OOModel::MetaCallMapping::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::MetaCallMapping::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::MetaCallMapping::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::MetaCallMapping::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::MetaCallMapping::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::MetaCallMapping::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::MetaCallMapping::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("symbolType", &OOModel::MetaCallMapping::symbolType)
		.def("isSubtypeOf", MetaCallMapping_isSubtypeOf1)
		.def("isSubtypeOf", MetaCallMapping_isSubtypeOf2)
		.def("registerNewAttribute", MetaCallMapping_registerNewAttribute1)
		.def("registerNewAttribute", MetaCallMapping_registerNewAttribute2);
}

{
	Model::CompositeIndex (*Method_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::Method::registerNewAttribute;
	Model::CompositeIndex (*Method_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Method::registerNewAttribute;
	bool (OOModel::Method::*Method_isSubtypeOf1)(const QString&) const = &OOModel::Method::isSubtypeOf;
	bool (OOModel::Method::*Method_isSubtypeOf2)(int) const = &OOModel::Method::isSubtypeOf;
	scope Methodscope = class_<OOModel::Method, bases<OOModel::Declaration>>("Method")
		.add_property("items",
			make_function(&OOModel::Method::items, return_internal_reference<>()),
			&OOModel::Method::setItems)
		.add_property("typeArguments",
			make_function(&OOModel::Method::typeArguments, return_internal_reference<>()),
			&OOModel::Method::setTypeArguments)
		.add_property("arguments",
			make_function(&OOModel::Method::arguments, return_internal_reference<>()),
			&OOModel::Method::setArguments)
		.add_property("results",
			make_function(&OOModel::Method::results, return_internal_reference<>()),
			&OOModel::Method::setResults)
		.add_property("memberInitializers",
			make_function(&OOModel::Method::memberInitializers, return_internal_reference<>()),
			&OOModel::Method::setMemberInitializers)
		.add_property("throws",
			make_function(&OOModel::Method::throws, return_internal_reference<>()),
			&OOModel::Method::setThrows)
		.add_property("methodKind",
			&OOModel::Method::methodKind,
			&OOModel::Method::setMethodKind)
		.def("typeName", make_function((const QString& (OOModel::Method::*)(
			))&OOModel::Method::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Method::typeId)
		.def("hierarchyTypeIds", &OOModel::Method::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Method::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Method::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Method::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Method::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&OOModel::Method::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::Method::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("fullyQualifiedName", &OOModel::Method::fullyQualifiedName)
		.def("isGeneric", &OOModel::Method::isGeneric)
		.def("isStatic", &OOModel::Method::isStatic)
		.def("symbolType", &OOModel::Method::symbolType)
		.def("findSymbols", &OOModel::Method::findSymbols)
		.def("overrides", &OOModel::Method::overrides)
		.def("isOverridenBy", &OOModel::Method::isOverridenBy)
		.def("callees", &OOModel::Method::callees)
		.def("callers", &OOModel::Method::callers)
		.def("isNewPersistenceUnit", &OOModel::Method::isNewPersistenceUnit)
		.def("registerNewAttribute", Method_registerNewAttribute1)
		.def("registerNewAttribute", Method_registerNewAttribute2)
		.def("isSubtypeOf", Method_isSubtypeOf1)
		.def("isSubtypeOf", Method_isSubtypeOf2);
	enum_<OOModel::Method::MethodKind>("MethodKind")
		.value("Default", OOModel::Method::MethodKind::Default)
		.value("Constructor", OOModel::Method::MethodKind::Constructor)
		.value("Destructor", OOModel::Method::MethodKind::Destructor)
		.value("Conversion", OOModel::Method::MethodKind::Conversion)
		.value("OperatorOverload", OOModel::Method::MethodKind::OperatorOverload);
}

{
	Model::CompositeIndex (*Module_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::Module::registerNewAttribute;
	Model::CompositeIndex (*Module_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Module::registerNewAttribute;
	bool (OOModel::Module::*Module_isSubtypeOf1)(const QString&) const = &OOModel::Module::isSubtypeOf;
	bool (OOModel::Module::*Module_isSubtypeOf2)(int) const = &OOModel::Module::isSubtypeOf;
	scope Modulescope = class_<OOModel::Module, bases<OOModel::Declaration>>("Module")
		.add_property("modules",
			make_function(&OOModel::Module::modules, return_internal_reference<>()),
			&OOModel::Module::setModules)
		.add_property("classes",
			make_function(&OOModel::Module::classes, return_internal_reference<>()),
			&OOModel::Module::setClasses)
		.add_property("methods",
			make_function(&OOModel::Module::methods, return_internal_reference<>()),
			&OOModel::Module::setMethods)
		.add_property("fields",
			make_function(&OOModel::Module::fields, return_internal_reference<>()),
			&OOModel::Module::setFields)
		.add_property("libraries",
			make_function(&OOModel::Module::libraries, return_internal_reference<>()),
			&OOModel::Module::setLibraries)
		.add_property("kind",
			&OOModel::Module::kind,
			&OOModel::Module::setKind)
		.def("typeName", make_function((const QString& (OOModel::Module::*)(
			))&OOModel::Module::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Module::typeId)
		.def("hierarchyTypeIds", &OOModel::Module::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Module::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Module::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Module::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Module::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&OOModel::Module::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::Module::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("symbolType", &OOModel::Module::symbolType)
		.def("usedLibraries", &OOModel::Module::usedLibraries)
		.def("isNewPersistenceUnit", &OOModel::Module::isNewPersistenceUnit)
		.def("isTransparentForNameResolution", &OOModel::Module::isTransparentForNameResolution)
		.def("registerNewAttribute", Module_registerNewAttribute1)
		.def("registerNewAttribute", Module_registerNewAttribute2)
		.def("isSubtypeOf", Module_isSubtypeOf1)
		.def("isSubtypeOf", Module_isSubtypeOf2);
	enum_<OOModel::Module::ModuleKind>("ModuleKind")
		.value("Standard", OOModel::Module::ModuleKind::Standard)
		.value("Folder", OOModel::Module::ModuleKind::Folder);
}

{
	Model::CompositeIndex (*Class_registerNewAttribute1)(const Model::Attribute&) = &OOModel::Class::registerNewAttribute;
	Model::CompositeIndex (*Class_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Class::registerNewAttribute;
	bool (OOModel::Class::*Class_isSubtypeOf1)(const QString&) const = &OOModel::Class::isSubtypeOf;
	bool (OOModel::Class::*Class_isSubtypeOf2)(int) const = &OOModel::Class::isSubtypeOf;
	scope Classscope = class_<OOModel::Class, bases<OOModel::Declaration>>("Class")
		.add_property("baseClasses",
			make_function(&OOModel::Class::baseClasses, return_internal_reference<>()),
			&OOModel::Class::setBaseClasses)
		.add_property("friends",
			make_function(&OOModel::Class::friends, return_internal_reference<>()),
			&OOModel::Class::setFriends)
		.add_property("typeArguments",
			make_function(&OOModel::Class::typeArguments, return_internal_reference<>()),
			&OOModel::Class::setTypeArguments)
		.add_property("classes",
			make_function(&OOModel::Class::classes, return_internal_reference<>()),
			&OOModel::Class::setClasses)
		.add_property("methods",
			make_function(&OOModel::Class::methods, return_internal_reference<>()),
			&OOModel::Class::setMethods)
		.add_property("fields",
			make_function(&OOModel::Class::fields, return_internal_reference<>()),
			&OOModel::Class::setFields)
		.add_property("enumerators",
			make_function(&OOModel::Class::enumerators, return_internal_reference<>()),
			&OOModel::Class::setEnumerators)
		.add_property("constructKind",
			&OOModel::Class::constructKind,
			&OOModel::Class::setConstructKind)
		.def("typeName", make_function((const QString& (OOModel::Class::*)(
			))&OOModel::Class::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Class::typeId)
		.def("hierarchyTypeIds", &OOModel::Class::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Class::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Class::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Class::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Class::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&OOModel::Class::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::Class::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("findSymbols", &OOModel::Class::findSymbols)
		.def("symbolType", &OOModel::Class::symbolType)
		.def("isGeneric", &OOModel::Class::isGeneric)
		.def("allBaseClasses", &OOModel::Class::allBaseClasses)
		.def("directBaseClasses", &OOModel::Class::directBaseClasses)
		.def("implicitBaseFromProject", make_function(
			&OOModel::Class::implicitBaseFromProject, return_internal_reference<>()))
		.def("expressionToClass", make_function(&OOModel::Class::expressionToClass, return_internal_reference<>()))
		.staticmethod("expressionToClass")
		.def("directSubClasses", &OOModel::Class::directSubClasses)
		.def("isNewPersistenceUnit", &OOModel::Class::isNewPersistenceUnit)
		.def("registerNewAttribute", Class_registerNewAttribute1)
		.def("registerNewAttribute", Class_registerNewAttribute2)
		.def("isSubtypeOf", Class_isSubtypeOf1)
		.def("isSubtypeOf", Class_isSubtypeOf2);
	enum_<OOModel::Class::ConstructKind>("ConstructKind")
		.value("Class", OOModel::Class::ConstructKind::Class)
		.value("Interface", OOModel::Class::ConstructKind::Interface)
		.value("Struct", OOModel::Class::ConstructKind::Struct)
		.value("Union", OOModel::Class::ConstructKind::Union)
		.value("Enum", OOModel::Class::ConstructKind::Enum)
		.value("Annotation", OOModel::Class::ConstructKind::Annotation);
}

{
	bool (OOModel::MetaDefinition::*MetaDefinition_isSubtypeOf1)(const
		QString&) const = &OOModel::MetaDefinition::isSubtypeOf;
	bool (OOModel::MetaDefinition::*MetaDefinition_isSubtypeOf2)(int) const = &OOModel::MetaDefinition::isSubtypeOf;
	Model::CompositeIndex (*MetaDefinition_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::MetaDefinition::registerNewAttribute;
	Model::CompositeIndex (*MetaDefinition_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::MetaDefinition::registerNewAttribute;
	class_<OOModel::MetaDefinition, bases<OOModel::Declaration>>("MetaDefinition")
		.add_property("arguments",
			make_function(&OOModel::MetaDefinition::arguments, return_internal_reference<>()),
			&OOModel::MetaDefinition::setArguments)
		.add_property("metaBindings",
			make_function(&OOModel::MetaDefinition::metaBindings, return_internal_reference<>()),
			&OOModel::MetaDefinition::setMetaBindings)
		.add_property("context",
			make_function(&OOModel::MetaDefinition::context, return_internal_reference<>()),
			&OOModel::MetaDefinition::setContext)
		.def("typeName", make_function((const QString& (
			OOModel::MetaDefinition::*)())&OOModel::MetaDefinition::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::MetaDefinition::typeId)
		.def("hierarchyTypeIds", &OOModel::MetaDefinition::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::MetaDefinition::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::MetaDefinition::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::MetaDefinition::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::MetaDefinition::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::MetaDefinition::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::MetaDefinition::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("symbolType", &OOModel::MetaDefinition::symbolType)
		.def("isSubtypeOf", MetaDefinition_isSubtypeOf1)
		.def("isSubtypeOf", MetaDefinition_isSubtypeOf2)
		.def("registerNewAttribute", MetaDefinition_registerNewAttribute1)
		.def("registerNewAttribute", MetaDefinition_registerNewAttribute2);
}

{
	Model::CompositeIndex (*TypeAlias_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::TypeAlias::registerNewAttribute;
	Model::CompositeIndex (*TypeAlias_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::TypeAlias::registerNewAttribute;
	bool (OOModel::TypeAlias::*TypeAlias_isSubtypeOf1)(const QString&) const = &OOModel::TypeAlias::isSubtypeOf;
	bool (OOModel::TypeAlias::*TypeAlias_isSubtypeOf2)(int) const = &OOModel::TypeAlias::isSubtypeOf;
	class_<OOModel::TypeAlias, bases<OOModel::Declaration>>("TypeAlias")
		.add_property("typeExpression",
			make_function(&OOModel::TypeAlias::typeExpression, return_internal_reference<>()),
			&OOModel::TypeAlias::setTypeExpression)
		.add_property("typeArguments",
			make_function(&OOModel::TypeAlias::typeArguments, return_internal_reference<>()),
			&OOModel::TypeAlias::setTypeArguments)
		.def("typeName", make_function((const QString& (OOModel::TypeAlias::*)(
			))&OOModel::TypeAlias::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::TypeAlias::typeId)
		.def("hierarchyTypeIds", &OOModel::TypeAlias::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::TypeAlias::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::TypeAlias::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::TypeAlias::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::TypeAlias::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::TypeAlias::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::TypeAlias::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("symbolType", &OOModel::TypeAlias::symbolType)
		.def("findSymbols", &OOModel::TypeAlias::findSymbols)
		.def("registerNewAttribute", TypeAlias_registerNewAttribute1)
		.def("registerNewAttribute", TypeAlias_registerNewAttribute2)
		.def("isSubtypeOf", TypeAlias_isSubtypeOf1)
		.def("isSubtypeOf", TypeAlias_isSubtypeOf2);
}

{
	bool (OOModel::FormalMetaArgument::*FormalMetaArgument_isSubtypeOf1)(
		const QString&) const = &OOModel::FormalMetaArgument::isSubtypeOf;
	bool (OOModel::FormalMetaArgument::*FormalMetaArgument_isSubtypeOf2)(
		int) const = &OOModel::FormalMetaArgument::isSubtypeOf;
	Model::CompositeIndex (*FormalMetaArgument_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FormalMetaArgument::registerNewAttribute;
	Model::CompositeIndex (*FormalMetaArgument_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FormalMetaArgument::registerNewAttribute;
	class_<OOModel::FormalMetaArgument, bases<OOModel::Declaration>>("FormalMetaArgument")
		.def("typeName", make_function((const QString& (
		OOModel::FormalMetaArgument::*)())&OOModel::FormalMetaArgument::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::FormalMetaArgument::typeId)
		.def("hierarchyTypeIds", &OOModel::FormalMetaArgument::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::FormalMetaArgument::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::FormalMetaArgument::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::FormalMetaArgument::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::FormalMetaArgument::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::FormalMetaArgument::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::FormalMetaArgument::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", FormalMetaArgument_isSubtypeOf1)
		.def("isSubtypeOf", FormalMetaArgument_isSubtypeOf2)
		.def("registerNewAttribute", FormalMetaArgument_registerNewAttribute1)
		.def("registerNewAttribute", FormalMetaArgument_registerNewAttribute2);
}

{
	bool (OOModel::CommentStatementItem::*CommentStatementItem_isSubtypeOf1)(
		const QString&) const = &OOModel::CommentStatementItem::isSubtypeOf;
	bool (OOModel::CommentStatementItem::*CommentStatementItem_isSubtypeOf2)(
		int) const = &OOModel::CommentStatementItem::isSubtypeOf;
	Model::CompositeIndex (*CommentStatementItem_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::CommentStatementItem::registerNewAttribute;
	Model::CompositeIndex (*CommentStatementItem_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::CommentStatementItem::registerNewAttribute;
	class_<OOModel::CommentStatementItem, bases<OOModel::StatementItem>>("CommentStatementItem")
		.add_property("commentNode",
			make_function(&OOModel::CommentStatementItem::commentNode, return_internal_reference<>()),
			&OOModel::CommentStatementItem::setCommentNode)
		.def("typeName", make_function((const QString& (
		OOModel::CommentStatementItem::*)())&OOModel::CommentStatementItem::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::CommentStatementItem::typeId)
		.def("hierarchyTypeIds", &OOModel::CommentStatementItem::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::CommentStatementItem::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::CommentStatementItem::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::CommentStatementItem::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::CommentStatementItem::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::CommentStatementItem::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::CommentStatementItem::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", CommentStatementItem_isSubtypeOf1)
		.def("isSubtypeOf", CommentStatementItem_isSubtypeOf2)
		.def("registerNewAttribute", CommentStatementItem_registerNewAttribute1)
		.def("registerNewAttribute", CommentStatementItem_registerNewAttribute2);
}

{
	bool (OOModel::Statement::*Statement_isSubtypeOf1)(const QString&) const = &OOModel::Statement::isSubtypeOf;
	bool (OOModel::Statement::*Statement_isSubtypeOf2)(int) const = &OOModel::Statement::isSubtypeOf;
	Model::CompositeIndex (*Statement_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::Statement::registerNewAttribute;
	Model::CompositeIndex (*Statement_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Statement::registerNewAttribute;
	class_<OOModel::Statement, bases<OOModel::StatementItem>>("Statement")
		.def("typeName", make_function((const QString& (OOModel::Statement::*)(
			))&OOModel::Statement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Statement::typeId)
		.def("hierarchyTypeIds", &OOModel::Statement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Statement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Statement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Statement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Statement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::Statement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::Statement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", Statement_isSubtypeOf1)
		.def("isSubtypeOf", Statement_isSubtypeOf2)
		.def("registerNewAttribute", Statement_registerNewAttribute1)
		.def("registerNewAttribute", Statement_registerNewAttribute2);
}

{
	bool (OOModel::StatementItemList::*StatementItemList_isSubtypeOf1)(
		const QString&) const = &OOModel::StatementItemList::isSubtypeOf;
	bool (OOModel::StatementItemList::*StatementItemList_isSubtypeOf2)(
		int) const = &OOModel::StatementItemList::isSubtypeOf;
	class_<OOModel::StatementItemList, bases<Model::TypedList<OOModel::StatementItem>>>("StatementItemList")
		.def("typeName", make_function((const QString& (
			OOModel::StatementItemList::*)())&OOModel::StatementItemList::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::StatementItemList::typeId)
		.def("hierarchyTypeIds", &OOModel::StatementItemList::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::StatementItemList::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::StatementItemList::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::StatementItemList::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::StatementItemList::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::StatementItemList::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("findSymbols", &OOModel::StatementItemList::findSymbols)
		.def("isSubtypeOf", StatementItemList_isSubtypeOf1)
		.def("isSubtypeOf", StatementItemList_isSubtypeOf2);
}

{
	bool (OOModel::FunctionTypeExpression::*FunctionTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::FunctionTypeExpression::isSubtypeOf;
	bool (OOModel::FunctionTypeExpression::*FunctionTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::FunctionTypeExpression::isSubtypeOf;
	Model::CompositeIndex (*FunctionTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FunctionTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*FunctionTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FunctionTypeExpression::registerNewAttribute;
	class_<OOModel::FunctionTypeExpression, bases<OOModel::TypeExpression>>("FunctionTypeExpression")
		.add_property("arguments",
			make_function(&OOModel::FunctionTypeExpression::arguments, return_internal_reference<>()),
			&OOModel::FunctionTypeExpression::setArguments)
		.add_property("results",
			make_function(&OOModel::FunctionTypeExpression::results, return_internal_reference<>()),
			&OOModel::FunctionTypeExpression::setResults)
		.def("typeName", make_function((const QString& (
		OOModel::FunctionTypeExpression::*)())&OOModel::FunctionTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::FunctionTypeExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::FunctionTypeExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(
			&OOModel::FunctionTypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::FunctionTypeExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::FunctionTypeExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::FunctionTypeExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::FunctionTypeExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::FunctionTypeExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::FunctionTypeExpression::type)
		.def("isSubtypeOf", FunctionTypeExpression_isSubtypeOf1)
		.def("isSubtypeOf", FunctionTypeExpression_isSubtypeOf2)
		.def("registerNewAttribute", FunctionTypeExpression_registerNewAttribute1)
		.def("registerNewAttribute", FunctionTypeExpression_registerNewAttribute2);
}

{
	Model::CompositeIndex (*TypeQualifierExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::TypeQualifierExpression::registerNewAttribute;
	Model::CompositeIndex (*TypeQualifierExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::TypeQualifierExpression::registerNewAttribute;
	bool (OOModel::TypeQualifierExpression::*TypeQualifierExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::TypeQualifierExpression::isSubtypeOf;
	bool (OOModel::TypeQualifierExpression::*TypeQualifierExpression_isSubtypeOf2)(
		int) const = &OOModel::TypeQualifierExpression::isSubtypeOf;
	class_<OOModel::TypeQualifierExpression, bases<OOModel::TypeExpression>>("TypeQualifierExpression")
		.add_property("typeExpression",
			make_function(&OOModel::TypeQualifierExpression::typeExpression, return_internal_reference<>()),
			&OOModel::TypeQualifierExpression::setTypeExpression)
		.add_property("qualifier",
			&OOModel::TypeQualifierExpression::qualifier,
			&OOModel::TypeQualifierExpression::setQualifier)
		.def("typeName", make_function((const QString& (
		OOModel::TypeQualifierExpression::*)())&OOModel::TypeQualifierExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::TypeQualifierExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::TypeQualifierExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(
			&OOModel::TypeQualifierExpression::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::TypeQualifierExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::TypeQualifierExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::TypeQualifierExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::TypeQualifierExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::TypeQualifierExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::TypeQualifierExpression::type)
		.def("registerNewAttribute", TypeQualifierExpression_registerNewAttribute1)
		.def("registerNewAttribute", TypeQualifierExpression_registerNewAttribute2)
		.def("isSubtypeOf", TypeQualifierExpression_isSubtypeOf1)
		.def("isSubtypeOf", TypeQualifierExpression_isSubtypeOf2);
}

{
	bool (OOModel::ArrayTypeExpression::*ArrayTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ArrayTypeExpression::isSubtypeOf;
	bool (OOModel::ArrayTypeExpression::*ArrayTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::ArrayTypeExpression::isSubtypeOf;
	Model::CompositeIndex (*ArrayTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ArrayTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*ArrayTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ArrayTypeExpression::registerNewAttribute;
	class_<OOModel::ArrayTypeExpression, bases<OOModel::TypeExpression>>("ArrayTypeExpression")
		.add_property("typeExpression",
			make_function(&OOModel::ArrayTypeExpression::typeExpression, return_internal_reference<>()),
			&OOModel::ArrayTypeExpression::setTypeExpression)
		.add_property("fixedSize",
			make_function(&OOModel::ArrayTypeExpression::fixedSize, return_internal_reference<>()),
			&OOModel::ArrayTypeExpression::setFixedSize)
		.def("typeName", make_function((const QString& (
		OOModel::ArrayTypeExpression::*)())&OOModel::ArrayTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ArrayTypeExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::ArrayTypeExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ArrayTypeExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ArrayTypeExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ArrayTypeExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ArrayTypeExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ArrayTypeExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ArrayTypeExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::ArrayTypeExpression::type)
		.def("isSubtypeOf", ArrayTypeExpression_isSubtypeOf1)
		.def("isSubtypeOf", ArrayTypeExpression_isSubtypeOf2)
		.def("registerNewAttribute", ArrayTypeExpression_registerNewAttribute1)
		.def("registerNewAttribute", ArrayTypeExpression_registerNewAttribute2);
}

{
	bool (OOModel::AutoTypeExpression::*AutoTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::AutoTypeExpression::isSubtypeOf;
	bool (OOModel::AutoTypeExpression::*AutoTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::AutoTypeExpression::isSubtypeOf;
	Model::CompositeIndex (*AutoTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::AutoTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*AutoTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::AutoTypeExpression::registerNewAttribute;
	class_<OOModel::AutoTypeExpression, bases<OOModel::TypeExpression>>("AutoTypeExpression")
		.def("typeName", make_function((const QString& (
		OOModel::AutoTypeExpression::*)())&OOModel::AutoTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::AutoTypeExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::AutoTypeExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::AutoTypeExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::AutoTypeExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::AutoTypeExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::AutoTypeExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::AutoTypeExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::AutoTypeExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::AutoTypeExpression::type)
		.def("isSubtypeOf", AutoTypeExpression_isSubtypeOf1)
		.def("isSubtypeOf", AutoTypeExpression_isSubtypeOf2)
		.def("registerNewAttribute", AutoTypeExpression_registerNewAttribute1)
		.def("registerNewAttribute", AutoTypeExpression_registerNewAttribute2);
}

{
	Model::CompositeIndex (*ReferenceTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ReferenceTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*ReferenceTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ReferenceTypeExpression::registerNewAttribute;
	bool (OOModel::ReferenceTypeExpression::*ReferenceTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ReferenceTypeExpression::isSubtypeOf;
	bool (OOModel::ReferenceTypeExpression::*ReferenceTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::ReferenceTypeExpression::isSubtypeOf;
	class_<OOModel::ReferenceTypeExpression, bases<OOModel::TypeExpression>>("ReferenceTypeExpression")
		.add_property("typeExpression",
			make_function(&OOModel::ReferenceTypeExpression::typeExpression, return_internal_reference<>()),
			&OOModel::ReferenceTypeExpression::setTypeExpression)
		.add_property("isRValueReference",
			&OOModel::ReferenceTypeExpression::isRValueReference,
			&OOModel::ReferenceTypeExpression::setIsRValueReference)
		.def("typeName", make_function((const QString& (
		OOModel::ReferenceTypeExpression::*)())&OOModel::ReferenceTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ReferenceTypeExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::ReferenceTypeExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(
			&OOModel::ReferenceTypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ReferenceTypeExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ReferenceTypeExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ReferenceTypeExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ReferenceTypeExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ReferenceTypeExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isRValueReferenceNode", make_function(
			&OOModel::ReferenceTypeExpression::isRValueReferenceNode, return_internal_reference<>()))
		.def("type", &OOModel::ReferenceTypeExpression::type)
		.def("registerNewAttribute", ReferenceTypeExpression_registerNewAttribute1)
		.def("registerNewAttribute", ReferenceTypeExpression_registerNewAttribute2)
		.def("isSubtypeOf", ReferenceTypeExpression_isSubtypeOf1)
		.def("isSubtypeOf", ReferenceTypeExpression_isSubtypeOf2);
}

{
	Model::CompositeIndex (*PrimitiveTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::PrimitiveTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*PrimitiveTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::PrimitiveTypeExpression::registerNewAttribute;
	bool (OOModel::PrimitiveTypeExpression::*PrimitiveTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::PrimitiveTypeExpression::isSubtypeOf;
	bool (OOModel::PrimitiveTypeExpression::*PrimitiveTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::PrimitiveTypeExpression::isSubtypeOf;
	class_<OOModel::PrimitiveTypeExpression, bases<OOModel::TypeExpression>>("PrimitiveTypeExpression")
		.add_property("typeValue",
			&OOModel::PrimitiveTypeExpression::typeValue,
			&OOModel::PrimitiveTypeExpression::setTypeValue)
		.def("typeName", make_function((const QString& (
		OOModel::PrimitiveTypeExpression::*)())&OOModel::PrimitiveTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::PrimitiveTypeExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::PrimitiveTypeExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(
			&OOModel::PrimitiveTypeExpression::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::PrimitiveTypeExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::PrimitiveTypeExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::PrimitiveTypeExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::PrimitiveTypeExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::PrimitiveTypeExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::PrimitiveTypeExpression::type)
		.def("registerNewAttribute", PrimitiveTypeExpression_registerNewAttribute1)
		.def("registerNewAttribute", PrimitiveTypeExpression_registerNewAttribute2)
		.def("isSubtypeOf", PrimitiveTypeExpression_isSubtypeOf1)
		.def("isSubtypeOf", PrimitiveTypeExpression_isSubtypeOf2);
}

{
	bool (OOModel::ClassTypeExpression::*ClassTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::ClassTypeExpression::isSubtypeOf;
	bool (OOModel::ClassTypeExpression::*ClassTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::ClassTypeExpression::isSubtypeOf;
	Model::CompositeIndex (*ClassTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ClassTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*ClassTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ClassTypeExpression::registerNewAttribute;
	class_<OOModel::ClassTypeExpression, bases<OOModel::TypeExpression>>("ClassTypeExpression")
		.add_property("typeExpression",
			make_function(&OOModel::ClassTypeExpression::typeExpression, return_internal_reference<>()),
			&OOModel::ClassTypeExpression::setTypeExpression)
		.def("typeName", make_function((const QString& (
		OOModel::ClassTypeExpression::*)())&OOModel::ClassTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ClassTypeExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::ClassTypeExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ClassTypeExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ClassTypeExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ClassTypeExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ClassTypeExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ClassTypeExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ClassTypeExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::ClassTypeExpression::type)
		.def("isSubtypeOf", ClassTypeExpression_isSubtypeOf1)
		.def("isSubtypeOf", ClassTypeExpression_isSubtypeOf2)
		.def("registerNewAttribute", ClassTypeExpression_registerNewAttribute1)
		.def("registerNewAttribute", ClassTypeExpression_registerNewAttribute2);
}

{
	bool (OOModel::PointerTypeExpression::*PointerTypeExpression_isSubtypeOf1)(
		const QString&) const = &OOModel::PointerTypeExpression::isSubtypeOf;
	bool (OOModel::PointerTypeExpression::*PointerTypeExpression_isSubtypeOf2)(
		int) const = &OOModel::PointerTypeExpression::isSubtypeOf;
	Model::CompositeIndex (*PointerTypeExpression_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::PointerTypeExpression::registerNewAttribute;
	Model::CompositeIndex (*PointerTypeExpression_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::PointerTypeExpression::registerNewAttribute;
	class_<OOModel::PointerTypeExpression, bases<OOModel::TypeExpression>>("PointerTypeExpression")
		.add_property("typeExpression",
			make_function(&OOModel::PointerTypeExpression::typeExpression, return_internal_reference<>()),
			&OOModel::PointerTypeExpression::setTypeExpression)
		.def("typeName", make_function((const QString& (
		OOModel::PointerTypeExpression::*)())&OOModel::PointerTypeExpression::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::PointerTypeExpression::typeId)
		.def("hierarchyTypeIds", &OOModel::PointerTypeExpression::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::PointerTypeExpression::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::PointerTypeExpression::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::PointerTypeExpression::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::PointerTypeExpression::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::PointerTypeExpression::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::PointerTypeExpression::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("type", &OOModel::PointerTypeExpression::type)
		.def("isSubtypeOf", PointerTypeExpression_isSubtypeOf1)
		.def("isSubtypeOf", PointerTypeExpression_isSubtypeOf2)
		.def("registerNewAttribute", PointerTypeExpression_registerNewAttribute1)
		.def("registerNewAttribute", PointerTypeExpression_registerNewAttribute2);
}

{
	bool (OOModel::Field::*Field_isSubtypeOf1)(const QString&) const = &OOModel::Field::isSubtypeOf;
	bool (OOModel::Field::*Field_isSubtypeOf2)(int) const = &OOModel::Field::isSubtypeOf;
	Model::CompositeIndex (*Field_registerNewAttribute1)(const Model::Attribute&) = &OOModel::Field::registerNewAttribute;
	Model::CompositeIndex (*Field_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Field::registerNewAttribute;
	class_<OOModel::Field, bases<OOModel::VariableDeclaration>>("Field")
		.def("typeName", make_function((const QString& (OOModel::Field::*)(
			))&OOModel::Field::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Field::typeId)
		.def("hierarchyTypeIds", &OOModel::Field::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Field::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Field::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Field::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Field::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&OOModel::Field::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::Field::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("symbolType", &OOModel::Field::symbolType)
		.def("isSubtypeOf", Field_isSubtypeOf1)
		.def("isSubtypeOf", Field_isSubtypeOf2)
		.def("registerNewAttribute", Field_registerNewAttribute1)
		.def("registerNewAttribute", Field_registerNewAttribute2);
}

{
	Model::CompositeIndex (*FormalArgument_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::FormalArgument::registerNewAttribute;
	Model::CompositeIndex (*FormalArgument_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::FormalArgument::registerNewAttribute;
	bool (OOModel::FormalArgument::*FormalArgument_isSubtypeOf1)(const
		QString&) const = &OOModel::FormalArgument::isSubtypeOf;
	bool (OOModel::FormalArgument::*FormalArgument_isSubtypeOf2)(int) const = &OOModel::FormalArgument::isSubtypeOf;
	scope FormalArgumentscope = class_<OOModel::FormalArgument, bases<OOModel::VariableDeclaration>>("FormalArgument")
		.add_property("direction",
			&OOModel::FormalArgument::direction,
			&OOModel::FormalArgument::setDirection)
		.def("typeName", make_function((const QString& (
			OOModel::FormalArgument::*)())&OOModel::FormalArgument::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::FormalArgument::typeId)
		.def("hierarchyTypeIds", &OOModel::FormalArgument::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::FormalArgument::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::FormalArgument::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::FormalArgument::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::FormalArgument::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::FormalArgument::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::FormalArgument::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isUsedInParentMethod", &OOModel::FormalArgument::isUsedInParentMethod)
		.def("registerNewAttribute", FormalArgument_registerNewAttribute1)
		.def("registerNewAttribute", FormalArgument_registerNewAttribute2)
		.def("isSubtypeOf", FormalArgument_isSubtypeOf1)
		.def("isSubtypeOf", FormalArgument_isSubtypeOf2);
	enum_<OOModel::FormalArgument::Direction>("Direction")
		.value("IN", OOModel::FormalArgument::Direction::IN)
		.value("OUT", OOModel::FormalArgument::Direction::OUT)
		.value("INOUT", OOModel::FormalArgument::Direction::INOUT);
}

{
	bool (OOModel::SynchronizedStatement::*SynchronizedStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::SynchronizedStatement::isSubtypeOf;
	bool (OOModel::SynchronizedStatement::*SynchronizedStatement_isSubtypeOf2)(
		int) const = &OOModel::SynchronizedStatement::isSubtypeOf;
	Model::CompositeIndex (*SynchronizedStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::SynchronizedStatement::registerNewAttribute;
	Model::CompositeIndex (*SynchronizedStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::SynchronizedStatement::registerNewAttribute;
	class_<OOModel::SynchronizedStatement, bases<OOModel::Statement>>("SynchronizedStatement")
		.add_property("expression",
			make_function(&OOModel::SynchronizedStatement::expression, return_internal_reference<>()),
			&OOModel::SynchronizedStatement::setExpression)
		.add_property("body",
			make_function(&OOModel::SynchronizedStatement::body, return_internal_reference<>()),
			&OOModel::SynchronizedStatement::setBody)
		.def("typeName", make_function((const QString& (
		OOModel::SynchronizedStatement::*)())&OOModel::SynchronizedStatement::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::SynchronizedStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::SynchronizedStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::SynchronizedStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::SynchronizedStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::SynchronizedStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::SynchronizedStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::SynchronizedStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::SynchronizedStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", SynchronizedStatement_isSubtypeOf1)
		.def("isSubtypeOf", SynchronizedStatement_isSubtypeOf2)
		.def("registerNewAttribute", SynchronizedStatement_registerNewAttribute1)
		.def("registerNewAttribute", SynchronizedStatement_registerNewAttribute2);
}

{
	bool (OOModel::IfStatement::*IfStatement_isSubtypeOf1)(const QString&) const = &OOModel::IfStatement::isSubtypeOf;
	bool (OOModel::IfStatement::*IfStatement_isSubtypeOf2)(int) const = &OOModel::IfStatement::isSubtypeOf;
	Model::CompositeIndex (*IfStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::IfStatement::registerNewAttribute;
	Model::CompositeIndex (*IfStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::IfStatement::registerNewAttribute;
	class_<OOModel::IfStatement, bases<OOModel::Statement>>("IfStatement")
		.add_property("condition",
			make_function(&OOModel::IfStatement::condition, return_internal_reference<>()),
			&OOModel::IfStatement::setCondition)
		.add_property("thenBranch",
			make_function(&OOModel::IfStatement::thenBranch, return_internal_reference<>()),
			&OOModel::IfStatement::setThenBranch)
		.add_property("elseBranch",
			make_function(&OOModel::IfStatement::elseBranch, return_internal_reference<>()),
			&OOModel::IfStatement::setElseBranch)
		.def("typeName", make_function((const QString& (OOModel::IfStatement::*)(
			))&OOModel::IfStatement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::IfStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::IfStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::IfStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::IfStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::IfStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::IfStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::IfStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::IfStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("findSymbols", &OOModel::IfStatement::findSymbols)
		.def("isSubtypeOf", IfStatement_isSubtypeOf1)
		.def("isSubtypeOf", IfStatement_isSubtypeOf2)
		.def("registerNewAttribute", IfStatement_registerNewAttribute1)
		.def("registerNewAttribute", IfStatement_registerNewAttribute2);
}

{
	bool (OOModel::ReturnStatement::*ReturnStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::ReturnStatement::isSubtypeOf;
	bool (OOModel::ReturnStatement::*ReturnStatement_isSubtypeOf2)(int) const = &OOModel::ReturnStatement::isSubtypeOf;
	Model::CompositeIndex (*ReturnStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ReturnStatement::registerNewAttribute;
	Model::CompositeIndex (*ReturnStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ReturnStatement::registerNewAttribute;
	class_<OOModel::ReturnStatement, bases<OOModel::Statement>>("ReturnStatement")
		.add_property("values",
			make_function(&OOModel::ReturnStatement::values, return_internal_reference<>()),
			&OOModel::ReturnStatement::setValues)
		.def("typeName", make_function((const QString& (
			OOModel::ReturnStatement::*)())&OOModel::ReturnStatement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ReturnStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::ReturnStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ReturnStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ReturnStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ReturnStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ReturnStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ReturnStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ReturnStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", ReturnStatement_isSubtypeOf1)
		.def("isSubtypeOf", ReturnStatement_isSubtypeOf2)
		.def("registerNewAttribute", ReturnStatement_registerNewAttribute1)
		.def("registerNewAttribute", ReturnStatement_registerNewAttribute2);
}

{
	bool (OOModel::SwitchStatement::*SwitchStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::SwitchStatement::isSubtypeOf;
	bool (OOModel::SwitchStatement::*SwitchStatement_isSubtypeOf2)(int) const = &OOModel::SwitchStatement::isSubtypeOf;
	Model::CompositeIndex (*SwitchStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::SwitchStatement::registerNewAttribute;
	Model::CompositeIndex (*SwitchStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::SwitchStatement::registerNewAttribute;
	class_<OOModel::SwitchStatement, bases<OOModel::Statement>>("SwitchStatement")
		.add_property("switchExpression",
			make_function(&OOModel::SwitchStatement::switchExpression, return_internal_reference<>()),
			&OOModel::SwitchStatement::setSwitchExpression)
		.add_property("body",
			make_function(&OOModel::SwitchStatement::body, return_internal_reference<>()),
			&OOModel::SwitchStatement::setBody)
		.def("typeName", make_function((const QString& (
			OOModel::SwitchStatement::*)())&OOModel::SwitchStatement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::SwitchStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::SwitchStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::SwitchStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::SwitchStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::SwitchStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::SwitchStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::SwitchStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::SwitchStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", SwitchStatement_isSubtypeOf1)
		.def("isSubtypeOf", SwitchStatement_isSubtypeOf2)
		.def("registerNewAttribute", SwitchStatement_registerNewAttribute1)
		.def("registerNewAttribute", SwitchStatement_registerNewAttribute2);
}

{
	bool (OOModel::CaseStatement::*CaseStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::CaseStatement::isSubtypeOf;
	bool (OOModel::CaseStatement::*CaseStatement_isSubtypeOf2)(int) const = &OOModel::CaseStatement::isSubtypeOf;
	Model::CompositeIndex (*CaseStatement_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::CaseStatement::registerNewAttribute;
	Model::CompositeIndex (*CaseStatement_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::CaseStatement::registerNewAttribute;
	class_<OOModel::CaseStatement, bases<OOModel::Statement>>("CaseStatement")
		.add_property("caseExpression",
			make_function(&OOModel::CaseStatement::caseExpression, return_internal_reference<>()),
			&OOModel::CaseStatement::setCaseExpression)
		.add_property("body",
			make_function(&OOModel::CaseStatement::body, return_internal_reference<>()),
			&OOModel::CaseStatement::setBody)
		.def("typeName", make_function((const QString& (
			OOModel::CaseStatement::*)())&OOModel::CaseStatement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::CaseStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::CaseStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::CaseStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::CaseStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::CaseStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::CaseStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::CaseStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::CaseStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", CaseStatement_isSubtypeOf1)
		.def("isSubtypeOf", CaseStatement_isSubtypeOf2)
		.def("registerNewAttribute", CaseStatement_registerNewAttribute1)
		.def("registerNewAttribute", CaseStatement_registerNewAttribute2);
}

{
	bool (OOModel::BreakStatement::*BreakStatement_isSubtypeOf1)(const
		QString&) const = &OOModel::BreakStatement::isSubtypeOf;
	bool (OOModel::BreakStatement::*BreakStatement_isSubtypeOf2)(int) const = &OOModel::BreakStatement::isSubtypeOf;
	Model::CompositeIndex (*BreakStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::BreakStatement::registerNewAttribute;
	Model::CompositeIndex (*BreakStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::BreakStatement::registerNewAttribute;
	class_<OOModel::BreakStatement, bases<OOModel::Statement>>("BreakStatement")
		.def("typeName", make_function((const QString& (
			OOModel::BreakStatement::*)())&OOModel::BreakStatement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::BreakStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::BreakStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::BreakStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::BreakStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::BreakStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::BreakStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::BreakStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::BreakStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", BreakStatement_isSubtypeOf1)
		.def("isSubtypeOf", BreakStatement_isSubtypeOf2)
		.def("registerNewAttribute", BreakStatement_registerNewAttribute1)
		.def("registerNewAttribute", BreakStatement_registerNewAttribute2);
}

{
	bool (OOModel::TryCatchFinallyStatement::*TryCatchFinallyStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::TryCatchFinallyStatement::isSubtypeOf;
	bool (OOModel::TryCatchFinallyStatement::*TryCatchFinallyStatement_isSubtypeOf2)(
		int) const = &OOModel::TryCatchFinallyStatement::isSubtypeOf;
	Model::CompositeIndex (*TryCatchFinallyStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::TryCatchFinallyStatement::registerNewAttribute;
	Model::CompositeIndex (*TryCatchFinallyStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::TryCatchFinallyStatement::registerNewAttribute;
	class_<OOModel::TryCatchFinallyStatement, bases<OOModel::Statement>>("TryCatchFinallyStatement")
		.add_property("tryBody",
			make_function(&OOModel::TryCatchFinallyStatement::tryBody, return_internal_reference<>()),
			&OOModel::TryCatchFinallyStatement::setTryBody)
		.add_property("catchClauses",
			make_function(&OOModel::TryCatchFinallyStatement::catchClauses, return_internal_reference<>()),
			&OOModel::TryCatchFinallyStatement::setCatchClauses)
		.add_property("finallyBody",
			make_function(&OOModel::TryCatchFinallyStatement::finallyBody, return_internal_reference<>()),
			&OOModel::TryCatchFinallyStatement::setFinallyBody)
		.def("typeName", make_function((const QString& (
		OOModel::TryCatchFinallyStatement::*)(
			))&OOModel::TryCatchFinallyStatement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::TryCatchFinallyStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::TryCatchFinallyStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(
			&OOModel::TryCatchFinallyStatement::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::TryCatchFinallyStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::TryCatchFinallyStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::TryCatchFinallyStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::TryCatchFinallyStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::TryCatchFinallyStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", TryCatchFinallyStatement_isSubtypeOf1)
		.def("isSubtypeOf", TryCatchFinallyStatement_isSubtypeOf2)
		.def("registerNewAttribute", TryCatchFinallyStatement_registerNewAttribute1)
		.def("registerNewAttribute", TryCatchFinallyStatement_registerNewAttribute2);
}

{
	Model::CompositeIndex (*DeclarationStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::DeclarationStatement::registerNewAttribute;
	Model::CompositeIndex (*DeclarationStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::DeclarationStatement::registerNewAttribute;
	bool (OOModel::DeclarationStatement::*DeclarationStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::DeclarationStatement::isSubtypeOf;
	bool (OOModel::DeclarationStatement::*DeclarationStatement_isSubtypeOf2)(
		int) const = &OOModel::DeclarationStatement::isSubtypeOf;
	class_<OOModel::DeclarationStatement, bases<OOModel::Statement>>("DeclarationStatement")
		.add_property("declaration",
			make_function(&OOModel::DeclarationStatement::declaration, return_internal_reference<>()),
			&OOModel::DeclarationStatement::setDeclaration)
		.def("typeName", make_function((const QString& (
		OOModel::DeclarationStatement::*)())&OOModel::DeclarationStatement::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::DeclarationStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::DeclarationStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::DeclarationStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::DeclarationStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::DeclarationStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::DeclarationStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::DeclarationStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::DeclarationStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("definesSymbol", &OOModel::DeclarationStatement::definesSymbol)
		.def("symbolName", make_function((const QString& (
		OOModel::DeclarationStatement::*)())&OOModel::DeclarationStatement::symbolName,
			 return_value_policy<copy_const_reference>()))
		.def("symbolType", &OOModel::DeclarationStatement::symbolType)
		.def("registerNewAttribute", DeclarationStatement_registerNewAttribute1)
		.def("registerNewAttribute", DeclarationStatement_registerNewAttribute2)
		.def("isSubtypeOf", DeclarationStatement_isSubtypeOf1)
		.def("isSubtypeOf", DeclarationStatement_isSubtypeOf2);
}

{
	Model::CompositeIndex (*ForEachStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ForEachStatement::registerNewAttribute;
	Model::CompositeIndex (*ForEachStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ForEachStatement::registerNewAttribute;
	bool (OOModel::ForEachStatement::*ForEachStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::ForEachStatement::isSubtypeOf;
	bool (OOModel::ForEachStatement::*ForEachStatement_isSubtypeOf2)(int) const = &OOModel::ForEachStatement::isSubtypeOf;
	class_<OOModel::ForEachStatement, bases<OOModel::Statement>>("ForEachStatement")
		.add_property("varName",
			make_function((const QString& (
				OOModel::ForEachStatement::*)())&OOModel::ForEachStatement::varName, return_value_policy<copy_const_reference>()),
			&OOModel::ForEachStatement::setVarName)
		.add_property("varType",
			make_function(&OOModel::ForEachStatement::varType, return_internal_reference<>()),
			&OOModel::ForEachStatement::setVarType)
		.add_property("collection",
			make_function(&OOModel::ForEachStatement::collection, return_internal_reference<>()),
			&OOModel::ForEachStatement::setCollection)
		.add_property("body",
			make_function(&OOModel::ForEachStatement::body, return_internal_reference<>()),
			&OOModel::ForEachStatement::setBody)
		.def("typeName", make_function((const QString& (
			OOModel::ForEachStatement::*)())&OOModel::ForEachStatement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ForEachStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::ForEachStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ForEachStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ForEachStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ForEachStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ForEachStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ForEachStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ForEachStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("varNameNode", make_function(&OOModel::ForEachStatement::varNameNode, return_internal_reference<>()))
		.def("definesSymbol", &OOModel::ForEachStatement::definesSymbol)
		.def("symbolName", make_function((const QString& (
			OOModel::ForEachStatement::*)())&OOModel::ForEachStatement::symbolName, return_value_policy<copy_const_reference>()))
		.def("symbolType", &OOModel::ForEachStatement::symbolType)
		.def("registerNewAttribute", ForEachStatement_registerNewAttribute1)
		.def("registerNewAttribute", ForEachStatement_registerNewAttribute2)
		.def("isSubtypeOf", ForEachStatement_isSubtypeOf1)
		.def("isSubtypeOf", ForEachStatement_isSubtypeOf2);
}

{
	bool (OOModel::AssertStatement::*AssertStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::AssertStatement::isSubtypeOf;
	bool (OOModel::AssertStatement::*AssertStatement_isSubtypeOf2)(int) const = &OOModel::AssertStatement::isSubtypeOf;
	Model::CompositeIndex (*AssertStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::AssertStatement::registerNewAttribute;
	Model::CompositeIndex (*AssertStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::AssertStatement::registerNewAttribute;
	class_<OOModel::AssertStatement, bases<OOModel::Statement>>("AssertStatement")
		.add_property("expression",
			make_function(&OOModel::AssertStatement::expression, return_internal_reference<>()),
			&OOModel::AssertStatement::setExpression)
		.def("typeName", make_function((const QString& (
			OOModel::AssertStatement::*)())&OOModel::AssertStatement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::AssertStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::AssertStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::AssertStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::AssertStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::AssertStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::AssertStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::AssertStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::AssertStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", AssertStatement_isSubtypeOf1)
		.def("isSubtypeOf", AssertStatement_isSubtypeOf2)
		.def("registerNewAttribute", AssertStatement_registerNewAttribute1)
		.def("registerNewAttribute", AssertStatement_registerNewAttribute2);
}

{
	Model::CompositeIndex (*ExpressionStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ExpressionStatement::registerNewAttribute;
	Model::CompositeIndex (*ExpressionStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ExpressionStatement::registerNewAttribute;
	bool (OOModel::ExpressionStatement::*ExpressionStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::ExpressionStatement::isSubtypeOf;
	bool (OOModel::ExpressionStatement::*ExpressionStatement_isSubtypeOf2)(
		int) const = &OOModel::ExpressionStatement::isSubtypeOf;
	class_<OOModel::ExpressionStatement, bases<OOModel::Statement>>("ExpressionStatement")
		.add_property("expression",
			make_function(&OOModel::ExpressionStatement::expression, return_internal_reference<>()),
			&OOModel::ExpressionStatement::setExpression)
		.def("typeName", make_function((const QString& (
		OOModel::ExpressionStatement::*)())&OOModel::ExpressionStatement::typeName,
			 return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ExpressionStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::ExpressionStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ExpressionStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ExpressionStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ExpressionStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ExpressionStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ExpressionStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ExpressionStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("definesSymbol", &OOModel::ExpressionStatement::definesSymbol)
		.def("symbolName", make_function((const QString& (
		OOModel::ExpressionStatement::*)())&OOModel::ExpressionStatement::symbolName,
			 return_value_policy<copy_const_reference>()))
		.def("symbolType", &OOModel::ExpressionStatement::symbolType)
		.def("registerNewAttribute", ExpressionStatement_registerNewAttribute1)
		.def("registerNewAttribute", ExpressionStatement_registerNewAttribute2)
		.def("isSubtypeOf", ExpressionStatement_isSubtypeOf1)
		.def("isSubtypeOf", ExpressionStatement_isSubtypeOf2);
}

{
	bool (OOModel::ContinueStatement::*ContinueStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::ContinueStatement::isSubtypeOf;
	bool (OOModel::ContinueStatement::*ContinueStatement_isSubtypeOf2)(
		int) const = &OOModel::ContinueStatement::isSubtypeOf;
	Model::CompositeIndex (*ContinueStatement_registerNewAttribute1)(
		const Model::Attribute&) = &OOModel::ContinueStatement::registerNewAttribute;
	Model::CompositeIndex (*ContinueStatement_registerNewAttribute2)(
		const QString&, const QString&, bool, bool, bool) = &OOModel::ContinueStatement::registerNewAttribute;
	class_<OOModel::ContinueStatement, bases<OOModel::Statement>>("ContinueStatement")
		.def("typeName", make_function((const QString& (
			OOModel::ContinueStatement::*)())&OOModel::ContinueStatement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::ContinueStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::ContinueStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::ContinueStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::ContinueStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::ContinueStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::ContinueStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::ContinueStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::ContinueStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", ContinueStatement_isSubtypeOf1)
		.def("isSubtypeOf", ContinueStatement_isSubtypeOf2)
		.def("registerNewAttribute", ContinueStatement_registerNewAttribute1)
		.def("registerNewAttribute", ContinueStatement_registerNewAttribute2);
}

{
	Model::CompositeIndex (*LoopStatement_registerNewAttribute1)(const
		Model::Attribute&) = &OOModel::LoopStatement::registerNewAttribute;
	Model::CompositeIndex (*LoopStatement_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::LoopStatement::registerNewAttribute;
	bool (OOModel::LoopStatement::*LoopStatement_isSubtypeOf1)(
		const QString&) const = &OOModel::LoopStatement::isSubtypeOf;
	bool (OOModel::LoopStatement::*LoopStatement_isSubtypeOf2)(int) const = &OOModel::LoopStatement::isSubtypeOf;
	scope LoopStatementscope = class_<OOModel::LoopStatement, bases<OOModel::Statement>>("LoopStatement")
		.add_property("condition",
			make_function(&OOModel::LoopStatement::condition, return_internal_reference<>()),
			&OOModel::LoopStatement::setCondition)
		.add_property("initStep",
			make_function(&OOModel::LoopStatement::initStep, return_internal_reference<>()),
			&OOModel::LoopStatement::setInitStep)
		.add_property("updateStep",
			make_function(&OOModel::LoopStatement::updateStep, return_internal_reference<>()),
			&OOModel::LoopStatement::setUpdateStep)
		.add_property("body",
			make_function(&OOModel::LoopStatement::body, return_internal_reference<>()),
			&OOModel::LoopStatement::setBody)
		.add_property("loopKind",
			&OOModel::LoopStatement::loopKind,
			&OOModel::LoopStatement::setLoopKind)
		.def("typeName", make_function((const QString& (
			OOModel::LoopStatement::*)())&OOModel::LoopStatement::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::LoopStatement::typeId)
		.def("hierarchyTypeIds", &OOModel::LoopStatement::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::LoopStatement::typeNameStatic,
			 return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::LoopStatement::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::LoopStatement::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::LoopStatement::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(
			&OOModel::LoopStatement::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::LoopStatement::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("findSymbols", &OOModel::LoopStatement::findSymbols)
		.def("registerNewAttribute", LoopStatement_registerNewAttribute1)
		.def("registerNewAttribute", LoopStatement_registerNewAttribute2)
		.def("isSubtypeOf", LoopStatement_isSubtypeOf1)
		.def("isSubtypeOf", LoopStatement_isSubtypeOf2);
	enum_<OOModel::LoopStatement::LoopKind>("LoopKind")
		.value("PreCheck", OOModel::LoopStatement::LoopKind::PreCheck)
		.value("PostCheck", OOModel::LoopStatement::LoopKind::PostCheck);
}

{
	bool (OOModel::Block::*Block_isSubtypeOf1)(const QString&) const = &OOModel::Block::isSubtypeOf;
	bool (OOModel::Block::*Block_isSubtypeOf2)(int) const = &OOModel::Block::isSubtypeOf;
	Model::CompositeIndex (*Block_registerNewAttribute1)(const Model::Attribute&) = &OOModel::Block::registerNewAttribute;
	Model::CompositeIndex (*Block_registerNewAttribute2)(const
		QString&, const QString&, bool, bool, bool) = &OOModel::Block::registerNewAttribute;
	class_<OOModel::Block, bases<OOModel::Statement>>("Block")
		.add_property("items",
			make_function(&OOModel::Block::items, return_internal_reference<>()),
			&OOModel::Block::setItems)
		.def("typeName", make_function((const QString& (OOModel::Block::*)(
			))&OOModel::Block::typeName, return_value_policy<copy_const_reference>()))
		.def("typeId", &OOModel::Block::typeId)
		.def("hierarchyTypeIds", &OOModel::Block::hierarchyTypeIds)
		.def("typeNameStatic", make_function(&OOModel::Block::typeNameStatic, return_value_policy<copy_const_reference>()))
		.staticmethod("typeNameStatic")
		.def("typeIdStatic", &OOModel::Block::typeIdStatic)
		.staticmethod("typeIdStatic")
		.def("initType", &OOModel::Block::initType)
		.staticmethod("initType")
		.def("clone", make_function(&OOModel::Block::clone, return_internal_reference<>()))
		.def("createDefaultInstance", make_function(&OOModel::Block::createDefaultInstance, return_internal_reference<>()))
		.staticmethod("createDefaultInstance")
		.def("getMetaData", make_function(&OOModel::Block::getMetaData, return_internal_reference<>()))
		.staticmethod("getMetaData")
		.def("isSubtypeOf", Block_isSubtypeOf1)
		.def("isSubtypeOf", Block_isSubtypeOf2)
		.def("registerNewAttribute", Block_registerNewAttribute1)
		.def("registerNewAttribute", Block_registerNewAttribute2);
}

{
	using Classs = Model::TypedList<OOModel::Class>;
	class_<Model::TypedList<OOModel::Class>, bases<Model::List>>("Classs")
		.def("__len__", &Classs::size)
		.def("__iter__", iterator<Classs, return_internal_reference<>>());

	using Methods = Model::TypedList<OOModel::Method>;
	class_<Model::TypedList<OOModel::Method>, bases<Model::List>>("Methods")
		.def("__len__", &Methods::size)
		.def("__iter__", iterator<Methods, return_internal_reference<>>());

	using FormalMetaArguments = Model::TypedList<OOModel::FormalMetaArgument>;
	class_<Model::TypedList<OOModel::FormalMetaArgument>, bases<Model::List>>("FormalMetaArguments")
		.def("__len__", &FormalMetaArguments::size)
		.def("__iter__", iterator<FormalMetaArguments, return_internal_reference<>>());

	using Enumerators = Model::TypedList<OOModel::Enumerator>;
	class_<Model::TypedList<OOModel::Enumerator>, bases<Model::List>>("Enumerators")
		.def("__len__", &Enumerators::size)
		.def("__iter__", iterator<Enumerators, return_internal_reference<>>());

	using Expressions = Model::TypedList<OOModel::Expression>;
	class_<Model::TypedList<OOModel::Expression>, bases<Model::List>>("Expressions")
		.def("__len__", &Expressions::size)
		.def("__iter__", iterator<Expressions, return_internal_reference<>>());

	using CatchClauses = Model::TypedList<OOModel::CatchClause>;
	class_<Model::TypedList<OOModel::CatchClause>, bases<Model::List>>("CatchClauses")
		.def("__len__", &CatchClauses::size)
		.def("__iter__", iterator<CatchClauses, return_internal_reference<>>());

	using MetaCallMappings = Model::TypedList<OOModel::MetaCallMapping>;
	class_<Model::TypedList<OOModel::MetaCallMapping>, bases<Model::List>>("MetaCallMappings")
		.def("__len__", &MetaCallMappings::size)
		.def("__iter__", iterator<MetaCallMappings, return_internal_reference<>>());

	using Modules = Model::TypedList<OOModel::Module>;
	class_<Model::TypedList<OOModel::Module>, bases<Model::List>>("Modules")
		.def("__len__", &Modules::size)
		.def("__iter__", iterator<Modules, return_internal_reference<>>());

	using FormalResults = Model::TypedList<OOModel::FormalResult>;
	class_<Model::TypedList<OOModel::FormalResult>, bases<Model::List>>("FormalResults")
		.def("__len__", &FormalResults::size)
		.def("__iter__", iterator<FormalResults, return_internal_reference<>>());

	using MemberInitializers = Model::TypedList<OOModel::MemberInitializer>;
	class_<Model::TypedList<OOModel::MemberInitializer>, bases<Model::List>>("MemberInitializers")
		.def("__len__", &MemberInitializers::size)
		.def("__iter__", iterator<MemberInitializers, return_internal_reference<>>());

	using FormalArguments = Model::TypedList<OOModel::FormalArgument>;
	class_<Model::TypedList<OOModel::FormalArgument>, bases<Model::List>>("FormalArguments")
		.def("__len__", &FormalArguments::size)
		.def("__iter__", iterator<FormalArguments, return_internal_reference<>>());

	using Texts = Model::TypedList<Model::Text>;
	class_<Model::TypedList<Model::Text>, bases<Model::List>>("Texts")
		.def("__len__", &Texts::size)
		.def("__iter__", iterator<Texts, return_internal_reference<>>());

	using MetaBindings = Model::TypedList<OOModel::MetaBinding>;
	class_<Model::TypedList<OOModel::MetaBinding>, bases<Model::List>>("MetaBindings")
		.def("__len__", &MetaBindings::size)
		.def("__iter__", iterator<MetaBindings, return_internal_reference<>>());

	using Projects = Model::TypedList<OOModel::Project>;
	class_<Model::TypedList<OOModel::Project>, bases<Model::List>>("Projects")
		.def("__len__", &Projects::size)
		.def("__iter__", iterator<Projects, return_internal_reference<>>());

	using Declarations = Model::TypedList<OOModel::Declaration>;
	class_<Model::TypedList<OOModel::Declaration>, bases<Model::List>>("Declarations")
		.def("__len__", &Declarations::size)
		.def("__iter__", iterator<Declarations, return_internal_reference<>>());

	using UsedLibrarys = Model::TypedList<Model::UsedLibrary>;
	class_<Model::TypedList<Model::UsedLibrary>, bases<Model::List>>("UsedLibrarys")
		.def("__len__", &UsedLibrarys::size)
		.def("__iter__", iterator<UsedLibrarys, return_internal_reference<>>());

	using Fields = Model::TypedList<OOModel::Field>;
	class_<Model::TypedList<OOModel::Field>, bases<Model::List>>("Fields")
		.def("__len__", &Fields::size)
		.def("__iter__", iterator<Fields, return_internal_reference<>>());

	using FormalTypeArguments = Model::TypedList<OOModel::FormalTypeArgument>;
	class_<Model::TypedList<OOModel::FormalTypeArgument>, bases<Model::List>>("FormalTypeArguments")
		.def("__len__", &FormalTypeArguments::size)
		.def("__iter__", iterator<FormalTypeArguments, return_internal_reference<>>());

}

}

} /* namespace InformationScripting */
