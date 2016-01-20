/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "../src/OOInteractionPlugin.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "../src/expression_editor/OOExpressionBuilder.h"
#include "../src/string_offset_providers/StringComponents.h"
#include "../src/expression_editor/OOOperatorDescriptorList.h"

#include "InteractionBase/src/expression_editor/OperatorDescriptor.h"

#include "OOModel/src/allOOModelNodes.h"

#include "VisualizationBase/src/VisualizationManager.h"
#include "VisualizationBase/src/Scene.h"
#include "VisualizationBase/src/node_extensions/Position.h"

using namespace OOModel;
using namespace Visualization;

namespace OOInteraction {

Method* addTestMethod()
{
	auto testMethod = new Method{"testMethod"};

	FormalResult* divbysixResult = new FormalResult{};
	divbysixResult->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	testMethod->results()->append(divbysixResult);
	FormalArgument* arg = new FormalArgument{};
	testMethod->arguments()->append(arg);
	arg->setName("numbers");
	ArrayTypeExpression* argType = new ArrayTypeExpression{};
	argType->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	arg->setTypeExpression(argType);

	ExpressionStatement* es = new ExpressionStatement{};
	es->setExpression( new EmptyExpression{});
	testMethod->items()->append(es);

	//Errors
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("{{b}{}")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("a[,,]")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("++{{c,d}")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("++{{a,b},{c,d}")});

	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("int")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("true")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("123456")});

	VariableDeclarationExpression* exprtest = new VariableDeclarationExpression{"exprtest"};
	testMethod->items()->append(new ExpressionStatement{exprtest});
	exprtest->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	exprtest->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("+aa++&b,*e/d-#") );

	VariableDeclarationExpression* exprtest2 = new VariableDeclarationExpression{"exprtest2"};
	testMethod->items()->append(new ExpressionStatement{exprtest2});
	exprtest2->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest2->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("a*b+c/e-d++%--e*-f==g") );

	VariableDeclarationExpression* exprtest3 = new VariableDeclarationExpression{"exprtest3"};
	testMethod->items()->append(new ExpressionStatement{exprtest3});
	exprtest3->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest3->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("a<b||c>d&&e<=f|g&h^~i") );

	VariableDeclarationExpression* exprtest4 = new VariableDeclarationExpression{"exprtest4"};
	testMethod->items()->append(new ExpressionStatement{exprtest4});
	exprtest4->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest4->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("new value[5]") );


	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("int[] foo=new int[10]")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("new int[5,3,2]")});
	testMethod->items()->append(new ExpressionStatement{
										  OOExpressionBuilder::getOOExpression("new int[2,2]{{a,b},{c,d}}")});

	VariableDeclarationExpression* exprtest5 = new VariableDeclarationExpression{"exprtest5"};
	testMethod->items()->append(new ExpressionStatement{exprtest5});
	exprtest5->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest5->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("(castto)object") );

	VariableDeclarationExpression* exprtest6 = new VariableDeclarationExpression{"exprtest6"};
	testMethod->items()->append(new ExpressionStatement{exprtest6});
	exprtest6->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest6->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("{a,bb,ccc}") );

	VariableDeclarationExpression* exprtest7 = new VariableDeclarationExpression{"exprtest7"};
	testMethod->items()->append(new ExpressionStatement{exprtest7});
	exprtest7->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest7->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("{{123,hello},{2,b}}") );

	VariableDeclarationExpression* exprtest8 = new VariableDeclarationExpression{"exprtest8"};
	testMethod->items()->append(new ExpressionStatement{exprtest8});
	exprtest8->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest8->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("a.b+c.d[i].f") );

	VariableDeclarationExpression* exprtest9 = new VariableDeclarationExpression{"exprtest9"};
	testMethod->items()->append(new ExpressionStatement{exprtest9});
	exprtest9->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest9->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("a()+a.b()+a.b[i].f().g()") );

	VariableDeclarationExpression* exprtest10 = new VariableDeclarationExpression{"exprtest10"};
	testMethod->items()->append(new ExpressionStatement{exprtest10});
	exprtest10->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest10->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("this.b(a,b,c,123,false)") );

	VariableDeclarationExpression* exprtest11 = new VariableDeclarationExpression{"exprtest11"};
	testMethod->items()->append(new ExpressionStatement{exprtest11});
	exprtest11->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest11->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("a+\"hello world\"") );

	VariableDeclarationExpression* exprtest12 = new VariableDeclarationExpression{"exprtest12"};
	testMethod->items()->append(new ExpressionStatement{exprtest12});
	exprtest12->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::VOID});
	exprtest12->decl()->setInitialValue( OOExpressionBuilder::getOOExpression("int[]") );

	auto exprtest13 = DCast<AssignmentExpression>( OOExpressionBuilder::getOOExpression("a=b%=c>>>=d+C"));
	testMethod->items()->append(new ExpressionStatement{exprtest13});

	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("int abc")});

	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("int abc=5+3")});

	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("int cond=abc<50?42:b+c")});

	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("Generic<T,U> instance")});

	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("int a=method<T,S>(x)")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("a||b||c||d-a+b+c+d")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("a+b+c+d-d||a||b||c||d")});

	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("SomeClass var")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("var.a")});

	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("int d=(a+b)/42")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("throw BadException")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("delete aPointer")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("delete[] anArrayPointer")});


	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("const int xc=42")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("volatile int xv")});
	testMethod->items()->append(new ExpressionStatement{
		OOExpressionBuilder::getOOExpression("const volatile int vcx=1")});

	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("auto aa=SomeClass")});


	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("[]() f")});
	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("[](int) g")});
	testMethod->items()->append(new ExpressionStatement{
			OOExpressionBuilder::getOOExpression("[](int arg,long arg2) h")});
	testMethod->items()->append(new ExpressionStatement{
			OOExpressionBuilder::getOOExpression("[](int arg,long arg2)->(int) h")});
	testMethod->items()->append(new ExpressionStatement{
					OOExpressionBuilder::getOOExpression("[](int arg,long arg2)->(int x) h")});
	testMethod->items()->append(new ExpressionStatement{
					OOExpressionBuilder::getOOExpression("[](int arg,long arg2)->(int x,int y) h")});

	testMethod->items()->append(new ExpressionStatement{ OOExpressionBuilder::getOOExpression("a.b[42]()+x()()")});
	testMethod->items()->append(new ExpressionStatement{
			OOExpressionBuilder::getOOExpression("::.globalVar=::.globalFunct(42)")});
	testMethod->items()->append(new ExpressionStatement{
			OOExpressionBuilder::getOOExpression("typename ::.Vis.StyleType var=\"value\"")});
	testMethod->items()->append(new ExpressionStatement{
			OOExpressionBuilder::getOOExpression("int size=sizeof(int)")});
	testMethod->items()->append(new ExpressionStatement{
			OOExpressionBuilder::getOOExpression("int align=alignof(float)")});
	testMethod->items()->append(new ExpressionStatement{
			OOExpressionBuilder::getOOExpression("auto tid=typeid(double)")});

	VariableDeclarationExpression* result = new VariableDeclarationExpression{"result"};
	testMethod->items()->append( new ExpressionStatement{result} );
	result->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	result->decl()->setInitialValue(new IntegerLiteral{-1});

	LoopStatement* sixloop = new LoopStatement{};
	testMethod->items()->append(sixloop);
	VariableDeclarationExpression* sixLoopInit = new VariableDeclarationExpression{"i"};
	sixloop->setInitStep(sixLoopInit);
	sixLoopInit->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	sixLoopInit->decl()->setInitialValue(new IntegerLiteral{0});
	BinaryOperation* sixLoopCond = new BinaryOperation{};
	sixloop->setCondition(sixLoopCond);
	sixLoopCond->setLeft(new ReferenceExpression{"i"});
	sixLoopCond->setOp(BinaryOperation::LESS);
	MethodCallExpression* sizeCall = new MethodCallExpression{"size", new ReferenceExpression{"numbers"}};
	sixLoopCond->setRight(sizeCall);

	//TODO test the visualization without the remaining parts of this method
	AssignmentExpression* sixLoopUpdate = new AssignmentExpression{};
	sixloop->setUpdateStep(sixLoopUpdate);
	sixLoopUpdate->setLeft(new ReferenceExpression{"i"});
	sixLoopUpdate->setOp(AssignmentExpression::PLUS_ASSIGN);
	sixLoopUpdate->setRight(new IntegerLiteral{1});

	VariableDeclarationExpression* n = new VariableDeclarationExpression{"n"};
	sixloop->body()->append(new ExpressionStatement{n});
	n->decl()->setTypeExpression(new PrimitiveTypeExpression{PrimitiveTypeExpression::PrimitiveTypes::INT});
	BinaryOperation* item = new BinaryOperation{};
	n->decl()->setInitialValue(item);
	item->setLeft(new ReferenceExpression{"numbers"});
	item->setOp(BinaryOperation::ARRAY_INDEX);
	item->setRight(new ReferenceExpression{"i"});

	IfStatement* ifdiv2 = new IfStatement{};
	sixloop->body()->append(ifdiv2);
	BinaryOperation* eq0 = new BinaryOperation{};
	ifdiv2->setCondition(eq0);
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight(new IntegerLiteral{0});
	BinaryOperation* div2 = new BinaryOperation{};
	eq0->setLeft(div2);
	div2->setLeft(new ReferenceExpression{"n"});
	div2->setOp(BinaryOperation::REMAINDER);
	div2->setRight(new IntegerLiteral{2});
	ifdiv2->elseBranch()->append(new ContinueStatement{});

	IfStatement* ifdiv3 = new IfStatement{};
	ifdiv2->thenBranch()->append(ifdiv3);
	eq0 = new BinaryOperation{};
	ifdiv3->setCondition(eq0);
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight(new IntegerLiteral{0});
	BinaryOperation* div3 = new BinaryOperation{};
	eq0->setLeft(div3);
	div3->setLeft(new ReferenceExpression{"n"});
	div3->setOp(BinaryOperation::REMAINDER);
	div3->setRight(new IntegerLiteral{3});

	AssignmentExpression* resultFound = new AssignmentExpression{};
	ifdiv3->thenBranch()->append(new ExpressionStatement{resultFound});
	resultFound->setLeft(new ReferenceExpression{"result"});
	resultFound->setOp(AssignmentExpression::ASSIGN);
	resultFound->setRight(new ReferenceExpression{"i"});
	ifdiv3->thenBranch()->append(new BreakStatement{});

	ReturnStatement* divbysixReturn = new ReturnStatement{};
	divbysixReturn->values()->append(new ReferenceExpression{"result"});
	testMethod->items()->append(divbysixReturn);

	return testMethod;
}

Method* addMainMethod()
{
	auto method = new Method{"Main"};

	method->items()->append(new ExpressionStatement{
			OOExpressionBuilder::getOOExpression("System.out.println(\"Hello world\")")});
	return method;
}

class SimpleTest : public Test<OOInteractionPlugin, SimpleTest> { public: void test()
{
	auto pr = new Project{"NewProject"};
	auto cl = new Class{"SomeClass"};
	pr->classes()->append(cl);
	cl->methods()->append(addTestMethod());

	auto top_level = pr;
	auto manager = new Model::TreeManager{top_level};


	VisualizationManager::instance().mainScene()->addTopLevelNode(top_level);
	VisualizationManager::instance().mainScene()->listenToTreeManager(manager);

	CHECK_CONDITION(top_level != nullptr);
}};

//class ExpressionParsingTest : public Test<OOInteractionPlugin, ExpressionParsingTest> { public: void test()
//{
//	// Try specific strings which have been problematic in the past
//	QStringList problematicStrings = {"SPACE<(> ", "x<>", "\\x ", "@@@@", "new [", "+=", "\\)", "new# "};
//	for (auto expressionString : problematicStrings)
//	{
//		auto expression = OOExpressionBuilder::getOOExpression(expressionString);
//		CHECK_CONDITION(expression);
//		CHECK_STR_EQUAL(expressionString, StringComponents::stringForNode(expression));
//		SAFE_DELETE(expression);
//	}

//	// Start with a list of some unused characters, together with a few variables and the empty space
//	QStringList operatorTokens{" ", "SPACE", "#", "_", "x", "5"};

//	// Add all keywords and symbols from operator signatures.
//	for (int i = 0; i< OOOperatorDescriptorList::instance()->size(); ++i)
//	{
//		auto descriptor = OOOperatorDescriptorList::instance()->at(i);
//		for (auto signatureElement : descriptor->signature())
//		{
//			Q_ASSERT(!signatureElement.isEmpty());

//			if (signatureElement[0].isLetter())
//			{
//				if (!operatorTokens.contains(signatureElement)) operatorTokens << signatureElement;
//			}
//			else
//				for (int j = 0; j < signatureElement.size(); ++j)
//					if (!operatorTokens.contains(QString{signatureElement[j]}))
//						operatorTokens << QString{signatureElement[j]};
//		}
//	}

//	// NOTE: Here we overwrite the list above, with a shorter version to speed up checking.
//	// The list below was created by printing the list above and carefully removing operators which are analogous in
//	// behavior (see the definitions in OOOOperatorDescriptorList.cpp)
//	operatorTokens = QStringList{" ", "SPACE", "#", "_", "x", "5", "+", "expr", "!", "~", "(", "typeOrExpr", ")", "*",
//							"&", "<", "-", ">", "=", "[", "]", "?", ":", "int", "type", "this", "auto", "null",
//							"true", ",", "{", "}", "new", "delete", ".", "id", "const", "throw", "sizeof", "\\", "@"};

//	// Generate all possible sequences of length up to MAX_SEQUENCE_LENGTH and test that each one is correctly
//	// parsed
//	constexpr int MAX_SEQUENCE_LENGTH = 5;
//	int indices[MAX_SEQUENCE_LENGTH];

//	long totalCombinations = 1;
//	for (int maxLenghtThisLoop = 1; maxLenghtThisLoop <= MAX_SEQUENCE_LENGTH; ++maxLenghtThisLoop )
//	{
//		totalCombinations *= operatorTokens.size();
//		log.info("Testing parsing of sequences of length " + QString::number(maxLenghtThisLoop));

//		for (int i = 0; i<maxLenghtThisLoop; ++i) indices[i] = 0;

//		QString testString;
//		long count = 0;
//		while (true)
//		{
//			// Test the current combination
//			testString.clear();
//			for (int i = 0; i<maxLenghtThisLoop; ++i) testString.append(operatorTokens[indices[i]]);
//			auto expression = OOExpressionBuilder::getOOExpression(testString);
//			CHECK_CONDITION(expression);
//			CHECK_STR_EQUAL(testString, StringComponents::stringForNode(expression));
//			SAFE_DELETE(expression);

//			//Advance counter
//			bool carry = true;
//			for (int i = maxLenghtThisLoop-1; carry && i>=0; --i)
//			{
//				++indices[i];
//				if (indices[i] == operatorTokens.size())
//				{
//					indices[i] = 0;
//					carry = true;
//				}
//				else carry = false;
//			}

//			if (carry || ++count % 10000 == 0)
//				log.info("Length: " + QString::number(maxLenghtThisLoop) + " done: " +QString::number(count)
//							 + " total: " +QString::number(totalCombinations));

//			if (carry) break;
//		}
//	}

//	CHECK_CONDITION(true);
//}};

}
