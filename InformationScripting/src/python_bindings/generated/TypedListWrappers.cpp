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

void initTypedListWrappers() {
	using Classs = Model::TypedList<OOModel::Class>;
	auto ClasssDef = class_<Model::TypedList<OOModel::Class>, bases<Model::List>>("Classs");
		ClasssDef.def("__len__", &Classs::size);
		ClasssDef.def("__iter__", iterator<Classs, return_internal_reference<>>());

	using Projects = Model::TypedList<OOModel::Project>;
	auto ProjectsDef = class_<Model::TypedList<OOModel::Project>, bases<Model::List>>("Projects");
		ProjectsDef.def("__len__", &Projects::size);
		ProjectsDef.def("__iter__", iterator<Projects, return_internal_reference<>>());

	using UsedLibrarys = Model::TypedList<Model::UsedLibrary>;
	auto UsedLibrarysDef = class_<Model::TypedList<Model::UsedLibrary>, bases<Model::List>>("UsedLibrarys");
		UsedLibrarysDef.def("__len__", &UsedLibrarys::size);
		UsedLibrarysDef.def("__iter__", iterator<UsedLibrarys, return_internal_reference<>>());

	using FormalMetaArguments = Model::TypedList<OOModel::FormalMetaArgument>;
	auto FormalMetaArgumentsDef = class_<Model::TypedList<OOModel::FormalMetaArgument>,
		 bases<Model::List>>("FormalMetaArguments");
		FormalMetaArgumentsDef.def("__len__", &FormalMetaArguments::size);
		FormalMetaArgumentsDef.def("__iter__", iterator<FormalMetaArguments, return_internal_reference<>>());

	using Modules = Model::TypedList<OOModel::Module>;
	auto ModulesDef = class_<Model::TypedList<OOModel::Module>, bases<Model::List>>("Modules");
		ModulesDef.def("__len__", &Modules::size);
		ModulesDef.def("__iter__", iterator<Modules, return_internal_reference<>>());

	using Enumerators = Model::TypedList<OOModel::Enumerator>;
	auto EnumeratorsDef = class_<Model::TypedList<OOModel::Enumerator>, bases<Model::List>>("Enumerators");
		EnumeratorsDef.def("__len__", &Enumerators::size);
		EnumeratorsDef.def("__iter__", iterator<Enumerators, return_internal_reference<>>());

	using Fields = Model::TypedList<OOModel::Field>;
	auto FieldsDef = class_<Model::TypedList<OOModel::Field>, bases<Model::List>>("Fields");
		FieldsDef.def("__len__", &Fields::size);
		FieldsDef.def("__iter__", iterator<Fields, return_internal_reference<>>());

	using FormalTypeArguments = Model::TypedList<OOModel::FormalTypeArgument>;
	auto FormalTypeArgumentsDef = class_<Model::TypedList<OOModel::FormalTypeArgument>,
		 bases<Model::List>>("FormalTypeArguments");
		FormalTypeArgumentsDef.def("__len__", &FormalTypeArguments::size);
		FormalTypeArgumentsDef.def("__iter__", iterator<FormalTypeArguments, return_internal_reference<>>());

	using Declarations = Model::TypedList<OOModel::Declaration>;
	auto DeclarationsDef = class_<Model::TypedList<OOModel::Declaration>, bases<Model::List>>("Declarations");
		DeclarationsDef.def("__len__", &Declarations::size);
		DeclarationsDef.def("__iter__", iterator<Declarations, return_internal_reference<>>());

	using MemberInitializers = Model::TypedList<OOModel::MemberInitializer>;
	auto MemberInitializersDef = class_<Model::TypedList<OOModel::MemberInitializer>,
		 bases<Model::List>>("MemberInitializers");
		MemberInitializersDef.def("__len__", &MemberInitializers::size);
		MemberInitializersDef.def("__iter__", iterator<MemberInitializers, return_internal_reference<>>());

	using MetaCallMappings = Model::TypedList<OOModel::MetaCallMapping>;
	auto MetaCallMappingsDef = class_<Model::TypedList<OOModel::MetaCallMapping>, bases<Model::List>>("MetaCallMappings");
		MetaCallMappingsDef.def("__len__", &MetaCallMappings::size);
		MetaCallMappingsDef.def("__iter__", iterator<MetaCallMappings, return_internal_reference<>>());

	using Expressions = Model::TypedList<OOModel::Expression>;
	auto ExpressionsDef = class_<Model::TypedList<OOModel::Expression>, bases<Model::List>>("Expressions");
		ExpressionsDef.def("__len__", &Expressions::size);
		ExpressionsDef.def("__iter__", iterator<Expressions, return_internal_reference<>>());

	using Methods = Model::TypedList<OOModel::Method>;
	auto MethodsDef = class_<Model::TypedList<OOModel::Method>, bases<Model::List>>("Methods");
		MethodsDef.def("__len__", &Methods::size);
		MethodsDef.def("__iter__", iterator<Methods, return_internal_reference<>>());

	using Texts = Model::TypedList<Model::Text>;
	auto TextsDef = class_<Model::TypedList<Model::Text>, bases<Model::List>>("Texts");
		TextsDef.def("__len__", &Texts::size);
		TextsDef.def("__iter__", iterator<Texts, return_internal_reference<>>());

	using FormalResults = Model::TypedList<OOModel::FormalResult>;
	auto FormalResultsDef = class_<Model::TypedList<OOModel::FormalResult>, bases<Model::List>>("FormalResults");
		FormalResultsDef.def("__len__", &FormalResults::size);
		FormalResultsDef.def("__iter__", iterator<FormalResults, return_internal_reference<>>());

	using MetaBindings = Model::TypedList<OOModel::MetaBinding>;
	auto MetaBindingsDef = class_<Model::TypedList<OOModel::MetaBinding>, bases<Model::List>>("MetaBindings");
		MetaBindingsDef.def("__len__", &MetaBindings::size);
		MetaBindingsDef.def("__iter__", iterator<MetaBindings, return_internal_reference<>>());

	using CatchClauses = Model::TypedList<OOModel::CatchClause>;
	auto CatchClausesDef = class_<Model::TypedList<OOModel::CatchClause>, bases<Model::List>>("CatchClauses");
		CatchClausesDef.def("__len__", &CatchClauses::size);
		CatchClausesDef.def("__iter__", iterator<CatchClauses, return_internal_reference<>>());

	using FormalArguments = Model::TypedList<OOModel::FormalArgument>;
	auto FormalArgumentsDef = class_<Model::TypedList<OOModel::FormalArgument>, bases<Model::List>>("FormalArguments");
		FormalArgumentsDef.def("__len__", &FormalArguments::size);
		FormalArgumentsDef.def("__iter__", iterator<FormalArguments, return_internal_reference<>>());

}


} /* namespace InformationScripting */
