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


} /* namespace InformationScripting */
