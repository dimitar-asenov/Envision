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

extern void initTypedListWrappers();

extern void initClassNode();
extern void initClassCompositeNode();
extern void initClassModifier();
extern void initClassReference();
extern void initClassList();
extern void initClassBoolean();
extern void initClassCharacter();
extern void initClassText();
extern void initClassFloat();
extern void initClassInteger();
extern void initClassExpression();
extern void initClassDeclaration();
extern void initClassMemberInitializer();
extern void initClassEnumerator();
extern void initClassCatchClause();
extern void initClassFormalResult();
extern void initClassStatementItem();
extern void initClassFormalTypeArgument();
extern void initClassUsedLibrary();
extern void initClassOOReference();
extern void initClassTypedListOfStatementItem();
extern void initClassNameText();
extern void initClassCommaExpression();
extern void initClassUnaryOperation();
extern void initClassMetaCallExpression();
extern void initClassUnfinishedOperator();
extern void initClassNullLiteral();
extern void initClassFloatLiteral();
extern void initClassTypeTraitExpression();
extern void initClassSuperExpression();
extern void initClassConditionalExpression();
extern void initClassTypeNameOperator();
extern void initClassThisExpression();
extern void initClassBinaryOperation();
extern void initClassEmptyExpression();
extern void initClassMethodCallExpression();
extern void initClassDeleteExpression();
extern void initClassLambdaExpression();
extern void initClassIntegerLiteral();
extern void initClassVariableDeclarationExpression();
extern void initClassStringLiteral();
extern void initClassInstanceOfExpression();
extern void initClassBooleanLiteral();
extern void initClassTypeExpression();
extern void initClassReferenceExpression();
extern void initClassArrayInitializer();
extern void initClassThrowExpression();
extern void initClassErrorExpression();
extern void initClassAssignmentExpression();
extern void initClassGlobalScopeExpression();
extern void initClassCharacterLiteral();
extern void initClassCastExpression();
extern void initClassNewExpression();
extern void initClassProject();
extern void initClassExplicitTemplateInstantiation();
extern void initClassMetaBinding();
extern void initClassNameImport();
extern void initClassVariableDeclaration();
extern void initClassMetaCallMapping();
extern void initClassMethod();
extern void initClassModule();
extern void initClassClass();
extern void initClassMetaDefinition();
extern void initClassTypeAlias();
extern void initClassFormalMetaArgument();
extern void initClassCommentStatementItem();
extern void initClassStatement();
extern void initClassStatementItemList();
extern void initClassFunctionTypeExpression();
extern void initClassTypeQualifierExpression();
extern void initClassArrayTypeExpression();
extern void initClassAutoTypeExpression();
extern void initClassReferenceTypeExpression();
extern void initClassPrimitiveTypeExpression();
extern void initClassClassTypeExpression();
extern void initClassPointerTypeExpression();
extern void initClassField();
extern void initClassFormalArgument();
extern void initClassSynchronizedStatement();
extern void initClassIfStatement();
extern void initClassReturnStatement();
extern void initClassSwitchStatement();
extern void initClassCaseStatement();
extern void initClassBreakStatement();
extern void initClassTryCatchFinallyStatement();
extern void initClassDeclarationStatement();
extern void initClassForEachStatement();
extern void initClassAssertStatement();
extern void initClassExpressionStatement();
extern void initClassContinueStatement();
extern void initClassLoopStatement();
extern void initClassBlock();


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
