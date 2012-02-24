/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/***********************************************************************************************************************
 * SimpleTest.cpp
 *
 *  Created on: Jan 12, 2012
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oointeraction.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "expression_editor/OOExpressionBuilder.h"

#include "OOModel/headers/allOOModelNodes.h"

#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/views/MainView.h"
#include "VisualizationBase/headers/node_extensions/Position.h"

using namespace OOModel;
using namespace Visualization;

namespace OOInteraction {

Class* addClass(Model::Model* model, Project* parent)
{
	Class* cl = nullptr;

	if (!parent) cl = dynamic_cast<Class*> (model->createRoot("Class"));
	model->beginModification(parent ? static_cast<Model::Node*> (parent) :cl, "Adding a hello world class.");
	if (!cl)
	{
		cl = new Class();
		parent->classes()->append(cl);
	}

	cl->setName("SomeClass");

	model->endModification();
	return cl;
}

Method* addDivBySix(Model::Model* model, Class* parent)
{
	Method* divbysix = nullptr;

	if (!parent) divbysix = dynamic_cast<Method*> (model->createRoot("Method"));
	model->beginModification(parent? static_cast<Model::Node*> (parent) : divbysix, "Adding a divBySix method.");
	if (!divbysix)
	{
		divbysix = new Method();
		parent->methods()->append(divbysix);
	}

	divbysix->setName("findDivBySix");
	FormalResult* divbysixResult = new FormalResult();
	divbysixResult->setType(new PrimitiveType(PrimitiveType::INT));
	divbysix->results()->append(divbysixResult);
	FormalArgument* arg = new FormalArgument();
	divbysix->arguments()->append(arg);
	arg->setName("numbers");
	ArrayType* argType = new ArrayType();
	argType->setType(new PrimitiveType(PrimitiveType::INT));
	arg->setType(argType);

	VariableDeclaration* exprtest = new VariableDeclaration();
	divbysix->items()->append(exprtest);
	exprtest->setName("exprtest");
	exprtest->setType(new PrimitiveType(PrimitiveType::INT));
	exprtest->setInitialValue( OOExpressionBuilder::getOOExpression("+aa++&b*e/d-#") );

	VariableDeclaration* exprtest2 = new VariableDeclaration();
	divbysix->items()->append(exprtest2);
	exprtest2->setName("exprtest2");
	exprtest2->setType(new PrimitiveType(PrimitiveType::VOID));
	exprtest2->setInitialValue( OOExpressionBuilder::getOOExpression("a*b+c/e-d++%--e*-f==g") );

	VariableDeclaration* exprtest3 = new VariableDeclaration();
	divbysix->items()->append(exprtest3);
	exprtest3->setName("exprtest3");
	exprtest3->setType(new PrimitiveType(PrimitiveType::VOID));
	exprtest3->setInitialValue( OOExpressionBuilder::getOOExpression("a < b || c > d && e <= f | g & h ^ ~i") );

	VariableDeclaration* exprtest4 = new VariableDeclaration();
	divbysix->items()->append(exprtest4);
	exprtest4->setName("exprtest4");
	exprtest4->setType(new PrimitiveType(PrimitiveType::VOID));
	exprtest4->setInitialValue( OOExpressionBuilder::getOOExpression("new value [5]") );

	VariableDeclaration* exprtest5 = new VariableDeclaration();
	divbysix->items()->append(exprtest5);
	exprtest5->setName("exprtest5");
	exprtest5->setType(new PrimitiveType(PrimitiveType::VOID));
	exprtest5->setInitialValue( OOExpressionBuilder::getOOExpression("(castto) object") );

	VariableDeclaration* exprtest6 = new VariableDeclaration();
	divbysix->items()->append(exprtest6);
	exprtest6->setName("exprtest6");
	exprtest6->setType(new PrimitiveType(PrimitiveType::VOID));
	exprtest6->setInitialValue( OOExpressionBuilder::getOOExpression("{{123, hello}, {2, b}}") );

	VariableDeclaration* result = new VariableDeclaration();
	divbysix->items()->append(result);
	result->setName("result");
	result->setType(new PrimitiveType(PrimitiveType::INT));
	result->setInitialValue(new IntegerLiteral(-1));

	LoopStatement* sixloop = new LoopStatement();
	divbysix->items()->append(sixloop);
	VariableDeclaration* sixLoopInit = new VariableDeclaration();
	sixloop->setInitStep(sixLoopInit);
	sixLoopInit->setName("i");
	sixLoopInit->setType(new PrimitiveType(PrimitiveType::INT));
	sixLoopInit->setInitialValue(new IntegerLiteral(0));
	BinaryOperation* sixLoopCond = new BinaryOperation();
	sixloop->setCondition(sixLoopCond);
	sixLoopCond->setLeft(new VariableAccess("local:i"));
	sixLoopCond->setOp(BinaryOperation::LESS);
	MethodCallExpression* sizeCall = new MethodCallExpression();
	sixLoopCond->setRight(sizeCall);
	sizeCall->ref()->set("size");
	sizeCall->setPrefix(new VariableAccess("local:numbers"));

	//TODO test the visualization without the remaining parts of this method
	AssignmentStatement* sixLoopUpdate = new AssignmentStatement();
	sixloop->setUpdateStep(sixLoopUpdate);
	sixLoopUpdate->setLeft(new VariableAccess("local:i"));
	sixLoopUpdate->setOp(AssignmentStatement::PLUS_ASSIGN);
	sixLoopUpdate->setRight(new IntegerLiteral(1));

	VariableDeclaration* n = new VariableDeclaration();
	sixloop->body()->append(n);
	n->setName("n");
	n->setType(new PrimitiveType(PrimitiveType::INT));
	BinaryOperation* item = new BinaryOperation();
	n->setInitialValue(item);
	item->setLeft(new VariableAccess("local:numbers"));
	item->setOp(BinaryOperation::ARRAY_INDEX);
	item->setRight(new VariableAccess("local:i"));

	IfStatement* ifdiv2 = new IfStatement();
	sixloop->body()->append(ifdiv2);
	BinaryOperation* eq0 = new BinaryOperation();
	ifdiv2->setCondition(eq0);
	eq0->setOp(BinaryOperation::EQUALS);
	eq0->setRight(new IntegerLiteral(0));
	BinaryOperation* div2 = new BinaryOperation();
	eq0->setLeft(div2);
	div2->setLeft(new VariableAccess("local:n"));
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
	div3->setLeft(new VariableAccess("local:n"));
	div3->setOp(BinaryOperation::REMAINDER);
	div3->setRight(new IntegerLiteral(3));

	AssignmentStatement* resultFound = new AssignmentStatement();
	ifdiv3->thenBranch()->append(resultFound);
	resultFound->setLeft(new VariableAccess("local:result"));
	resultFound->setOp(AssignmentStatement::ASSIGN);
	resultFound->setRight(new VariableAccess("local:i"));
	ifdiv3->thenBranch()->append(new BreakStatement());

	ReturnStatement* divbysixReturn = new ReturnStatement();
	divbysixReturn->values()->append(new VariableAccess("local:result"));
	divbysix->items()->append(divbysixReturn);

	model->endModification();
	return divbysix;
}

TEST(OOInteraction, SimpleTest)
{
	Model::Model* model = new Model::Model();

	Class* cl = nullptr;
	cl = addClass(model, nullptr);

	Method* divbysix = nullptr;
	divbysix = addDivBySix(model, cl);

	Model::Node* top_level = nullptr;
	if (cl) top_level = cl;
	else top_level = divbysix;

	Scene* scene = new Scene();
	scene->addTopLevelItem( scene->defaultRenderer()->render(nullptr, top_level) );
	scene->scheduleUpdate();
	scene->listenToModel(model);

	// Create view
	MainView* view = new MainView(scene);

	CHECK_CONDITION(view != nullptr);
}

}
