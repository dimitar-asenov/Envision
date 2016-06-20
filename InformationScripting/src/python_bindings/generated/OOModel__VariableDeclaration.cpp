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


} /* namespace InformationScripting */
