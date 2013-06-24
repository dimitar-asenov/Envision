/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "oointeraction.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "expression_editor/OOExpressionBuilder.h"

#include "OOModel/src/allOOModelNodes.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/node_extensions/Position.h"
#include "VisualizationBase/src/items/RootItem.h"

using namespace OOModel;
using namespace Visualization;

namespace OOInteraction {

Class* addClass(Project* parent)
{
	auto cl = new Class("SomeClass");
	if (parent) parent->classes()->append(cl);

	return cl;
}

Method* addDivBySix(Class* parent)
{
	auto divbysix = new Method("findDivBySix");
	if (parent) parent->methods()->append(divbysix);

	FormalResult* divbysixResult = new FormalResult();
	divbysixResult->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	divbysix->results()->append(divbysixResult);
	FormalArgument* arg = new FormalArgument();
	divbysix->arguments()->append(arg);
	arg->setName("numbers");
	ArrayTypeExpression* argType = new ArrayTypeExpression();
	argType->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	arg->setTypeExpression(argType);

	ExpressionStatement* es = new ExpressionStatement();
	es->setExpression( new EmptyExpression());
	divbysix->items()->append(es);

	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("int")));
	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("true")));
	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("123456")));

	VariableDeclarationExpression* exprtest = new VariableDeclarationExpression("exprtest");
	divbysix->items()->append(new ExpressionStatement(exprtest));
	exprtest->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	exprtest->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("+aa++&b,*e/d-#") );

	VariableDeclarationExpression* exprtest2 = new VariableDeclarationExpression("exprtest2");
	divbysix->items()->append(new ExpressionStatement(exprtest2));
	exprtest2->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest2->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("a*b+c/e-d++%--e*-f==g") );

	VariableDeclarationExpression* exprtest3 = new VariableDeclarationExpression("exprtest3");
	divbysix->items()->append(new ExpressionStatement(exprtest3));
	exprtest3->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest3->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("a<b||c>d&&e<=f|g&h^~i") );

	VariableDeclarationExpression* exprtest4 = new VariableDeclarationExpression("exprtest4");
	divbysix->items()->append(new ExpressionStatement(exprtest4));
	exprtest4->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest4->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("new value[5]") );

	VariableDeclarationExpression* exprtest5 = new VariableDeclarationExpression("exprtest5");
	divbysix->items()->append(new ExpressionStatement(exprtest5));
	exprtest5->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest5->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("(castto)object") );

	VariableDeclarationExpression* exprtest6 = new VariableDeclarationExpression("exprtest6");
	divbysix->items()->append(new ExpressionStatement(exprtest6));
	exprtest6->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest6->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("{a,bb,ccc}") );

	VariableDeclarationExpression* exprtest7 = new VariableDeclarationExpression("exprtest7");
	divbysix->items()->append(new ExpressionStatement(exprtest7));
	exprtest7->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest7->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("{{123,hello},{2,b}}") );

	VariableDeclarationExpression* exprtest8 = new VariableDeclarationExpression("exprtest8");
	divbysix->items()->append(new ExpressionStatement(exprtest8));
	exprtest8->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest8->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("a.b+c.d[i].f") );

	VariableDeclarationExpression* exprtest9 = new VariableDeclarationExpression("exprtest9");
	divbysix->items()->append(new ExpressionStatement(exprtest9));
	exprtest9->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest9->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("a()+a.b()+a.b[i].f().g()") );

	VariableDeclarationExpression* exprtest10 = new VariableDeclarationExpression("exprtest10");
	divbysix->items()->append(new ExpressionStatement(exprtest10));
	exprtest10->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest10->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("this.b(a,b,c,123,false)") );

	VariableDeclarationExpression* exprtest11 = new VariableDeclarationExpression("exprtest11");
	divbysix->items()->append(new ExpressionStatement(exprtest11));
	exprtest11->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest11->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("a+\"hello world\"") );

	VariableDeclarationExpression* exprtest12 = new VariableDeclarationExpression("exprtest12");
	divbysix->items()->append(new ExpressionStatement(exprtest12));
	exprtest12->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::VOID));
	exprtest12->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("int[]") );

	auto exprtest13 = dynamic_cast<AssignmentExpression*>( OOExpressionBuilder::getOOExpression("a=b%=c>>>=d+C"));
	divbysix->items()->append(new ExpressionStatement(exprtest13));

	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("int abc")));

	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("int abc=5+3")));

	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("int cond=abc<50?42:b+c")));

	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("Generic<T,U> instance")));

	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("int a=method<T,S>(x)")));
	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("a||b||c||d-a+b+c+d")));
	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("a+b+c+d-d||a||b||c||d")));

	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("SomeClass var")));
	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("var.a")));

	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("int d=(a+b)/42")));
	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("throw BadException")));
	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("delete aPointer")));
	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("delete[] anArrayPointer")));


	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("const int xc=42")));
	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("volatile int xv")));
	divbysix->items()->append(new ExpressionStatement(
		OOExpressionBuilder::getOOExpression("const volatile int vcx=1")));

	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("auto aa=SomeClass")));


	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("[]() f")));
	divbysix->items()->append(new ExpressionStatement( OOExpressionBuilder::getOOExpression("[](int) g")));
	divbysix->items()->append(new ExpressionStatement(
			OOExpressionBuilder::getOOExpression("[](int arg,long arg2) h")));
	divbysix->items()->append(new ExpressionStatement(
			OOExpressionBuilder::getOOExpression("[](int arg,long arg2)->(int) h")));
	divbysix->items()->append(new ExpressionStatement(
					OOExpressionBuilder::getOOExpression("[](int arg,long arg2)->(int x) h")));
	divbysix->items()->append(new ExpressionStatement(
					OOExpressionBuilder::getOOExpression("[](int arg,long arg2)->(int x,int y) h")));

	divbysix->items()->append(new ExpressionStatement(
					OOExpressionBuilder::getOOExpression("a.b[42]()+x()()")));

	VariableDeclarationExpression* result = new VariableDeclarationExpression("result");
	divbysix->items()->append( new ExpressionStatement(result) );
	result->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	result->decl()->setInitialValue(new IntegerLiteral(-1));

	LoopStatement* sixloop = new LoopStatement();
	divbysix->items()->append(sixloop);
	VariableDeclarationExpression* sixLoopInit = new VariableDeclarationExpression("i");
	sixloop->setInitStep(sixLoopInit);
	sixLoopInit->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	sixLoopInit->decl()->setInitialValue(new IntegerLiteral(0));
	BinaryOperation* sixLoopCond = new BinaryOperation();
	sixloop->setCondition(sixLoopCond);
	sixLoopCond->setLeft(new ReferenceExpression("i"));
	sixLoopCond->setOp(BinaryOperation::LESS);
	MethodCallExpression* sizeCall = new MethodCallExpression("size", new ReferenceExpression("numbers"));
	sixLoopCond->setRight(sizeCall);

	//TODO test the visualization without the remaining parts of this method
	AssignmentExpression* sixLoopUpdate = new AssignmentExpression();
	sixloop->setUpdateStep(sixLoopUpdate);
	sixLoopUpdate->setLeft(new ReferenceExpression("i"));
	sixLoopUpdate->setOp(AssignmentExpression::PLUS_ASSIGN);
	sixLoopUpdate->setRight(new IntegerLiteral(1));

	VariableDeclarationExpression* n = new VariableDeclarationExpression("n");
	sixloop->body()->append(new ExpressionStatement(n));
	n->decl()->setTypeExpression(new PrimitiveTypeExpression(PrimitiveTypeExpression::PrimitiveTypes::INT));
	BinaryOperation* item = new BinaryOperation();
	n->decl()->setInitialValue(item);
	item->setLeft(new ReferenceExpression("numbers"));
	item->setOp(BinaryOperation::ARRAY_INDEX);
	item->setRight(new ReferenceExpression("i"));

	IfStatement* ifdiv2 = new IfStatement();
	sixloop->body()->append(ifdiv2);
	BinaryOperation* eq0 = new BinaryOperation();
	ifdiv2->setCondition(eq0);
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight(new IntegerLiteral(0));
	BinaryOperation* div2 = new BinaryOperation();
	eq0->setLeft(div2);
	div2->setLeft(new ReferenceExpression("n"));
	div2->setOp(BinaryOperation::REMAINDER);
	div2->setRight(new IntegerLiteral(2));
	ifdiv2->elseBranch()->append(new ContinueStatement());

	IfStatement* ifdiv3 = new IfStatement();
	ifdiv2->thenBranch()->append(ifdiv3);
	eq0 = new BinaryOperation();
	ifdiv3->setCondition(eq0);
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight(new IntegerLiteral(0));
	BinaryOperation* div3 = new BinaryOperation();
	eq0->setLeft(div3);
	div3->setLeft(new ReferenceExpression("n"));
	div3->setOp(BinaryOperation::REMAINDER);
	div3->setRight(new IntegerLiteral(3));

	AssignmentExpression* resultFound = new AssignmentExpression();
	ifdiv3->thenBranch()->append(new ExpressionStatement(resultFound));
	resultFound->setLeft(new ReferenceExpression("result"));
	resultFound->setOp(AssignmentExpression::ASSIGN);
	resultFound->setRight(new ReferenceExpression("i"));
	ifdiv3->thenBranch()->append(new BreakStatement());

	ReturnStatement* divbysixReturn = new ReturnStatement();
	divbysixReturn->values()->append(new ReferenceExpression("result"));
	divbysix->items()->append(divbysixReturn);

	return divbysix;
}

TEST(OOInteraction, SimpleTest)
{
	auto pr = new Project("NewProject");
	auto cl = addClass(pr);
	addDivBySix(cl);

	auto top_level = pr;
	auto model = new Model::Model(top_level);


	VisualizationManager::instance().mainScene()->addTopLevelItem( new RootItem(top_level));
	VisualizationManager::instance().mainScene()->listenToModel(model);

	CHECK_CONDITION(top_level != nullptr);
}

}
