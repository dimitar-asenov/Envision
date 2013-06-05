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

#include "GridConstructors.h"

#include "GridBasedOffsetProvider.h"
#include "Cell.h"
#include "ListCell.h"
#include "OOVisualization/src/expressions/allOOExpressionVisualizations.h"
#include "OOVisualization/src/alternative/VKeywordMethodCall.h"

#include "VisualizationBase/src/items/Text.h"

using namespace OOVisualization;
namespace OOInteraction {

void GridConstructors::initializeAll()
{
	GridBasedOffsetProvider::addGridConstructor<VMethodCallExpression>(
	[](GridBasedOffsetProvider* grid, VMethodCallExpression* vis){
		grid->add(new Cell(0, vis->layout()->at<Visualization::Item>(0), 0));
		grid->add(new ListCell(1, vis->arguments(), 1, "(", ",", ")"));
	});

	GridBasedOffsetProvider::addGridConstructor<VCastExpression>(
	[](GridBasedOffsetProvider* grid, VCastExpression* vis){
		grid->add(new Cell(0, 0, vis->layout()->at<Visualization::Item>(0), 3));
		grid->add(new Cell(0, 1, vis->layout()->at<Visualization::Item>(1), 1));
	});

	GridBasedOffsetProvider::addGridConstructor<VClassType>(
	[](GridBasedOffsetProvider* grid, VClassType* vis){
		grid->add(new Cell(0, vis->reference(), 0));
		// This is different from below since reference() does not simply return a TextRenderer
		// There is no need to manually set isIndivisible
	});

	GridBasedOffsetProvider::addGridConstructor<VPrimitiveType>(
	[](GridBasedOffsetProvider* grid, VPrimitiveType* vis){
		grid->add(new Cell(0, vis->item(), 0));
	});

	GridBasedOffsetProvider::addGridConstructor<VAutoType>(
	[](GridBasedOffsetProvider* grid, VAutoType* vis){
		grid->add(new Cell(0, vis->item(), 0));
	});

	GridBasedOffsetProvider::addGridConstructor<VBooleanLiteral>(
	[](GridBasedOffsetProvider* grid, VBooleanLiteral* vis){
		grid->add(new Cell(0, vis->item(), 0));
	});

	GridBasedOffsetProvider::addGridConstructor<VIntegerLiteral>(
	[](GridBasedOffsetProvider* grid, VIntegerLiteral* vis){
		grid->add(new Cell(0, vis->item(), 0));
	});

	GridBasedOffsetProvider::addGridConstructor<VNullLiteral>(
	[](GridBasedOffsetProvider* grid, VNullLiteral* vis){
		grid->add(new Cell(0, vis->item(), 0));
	});

	GridBasedOffsetProvider::addGridConstructor<VThisExpression>(
	[](GridBasedOffsetProvider* grid, VThisExpression* vis){
		grid->add(new Cell(0, vis->item(), 0));
	});

	GridBasedOffsetProvider::addGridConstructor<VKeywordMethodCall>(
	[](GridBasedOffsetProvider* grid, VKeywordMethodCall* vis){
		grid->add(new Cell(0, vis->keyword(), 0));
		grid->add(new ListCell(1, vis->arguments(), 1, "(", ",", ")"));
	});

	GridBasedOffsetProvider::addGridConstructor<VNewExpression>(
	[](GridBasedOffsetProvider* grid, VNewExpression* vis){
		grid->add(new Cell(0, vis->layout()->at<Visualization::Item>(0), 0));
		grid->add(new Cell(2, vis->layout()->at<Visualization::Item>(1), 2));
		if (vis->layout()->length() > 2)
		{
			grid->add(new Cell(3, vis->layout()->at<Visualization::Item>(2), 3));
			grid->add(new Cell(4, vis->layout()->at<Visualization::Item>(3), 4));
			grid->add(new Cell(5, vis->layout()->at<Visualization::Item>(4), 5));
		}
	});

	GridBasedOffsetProvider::addGridConstructor<VReferenceExpression>(
	[](GridBasedOffsetProvider* grid, VReferenceExpression* vis){
		int index = 0;
		if (vis->prefix())
		{
			grid->add(new Cell(0, vis->prefix(), 0));
			grid->add(new Cell(1, vis->separator(), 1));

			index = 2;
		}

		grid->add(new Cell(index, vis->name(), index));
		if (vis->typeArguments()) grid->add(new ListCell(index+1, vis->typeArguments(), index+1, "<", ",", ">"));
	});

	GridBasedOffsetProvider::addGridConstructor<VThrowExpression>(
	[](GridBasedOffsetProvider* grid, VThrowExpression* vis){
		grid->add(new Cell(0, vis->layout()->at<Visualization::Item>(0), 0));
		grid->add(new Cell(2, vis->layout()->at<Visualization::Item>(1), 2));
	});

	GridBasedOffsetProvider::addGridConstructor<VTypeQualifierExpression>(
		[](GridBasedOffsetProvider* grid, VTypeQualifierExpression* vis){
			grid->add(new Cell(0, vis->layout()->at<Visualization::Item>(0), 0));
			grid->add(new Cell(2, vis->layout()->at<Visualization::Item>(1), 2));
		});

	GridBasedOffsetProvider::addGridConstructor<VDeleteExpression>(
		[](GridBasedOffsetProvider* grid, VDeleteExpression* vis){
			grid->add(new Cell(0, vis->layout()->at<Visualization::Item>(0), 0));
			grid->add(new Cell(2, vis->layout()->at<Visualization::Item>(1), 2));
		});

	GridBasedOffsetProvider::addGridConstructor<VVariableDeclarationExpression>(
	[](GridBasedOffsetProvider* grid, VVariableDeclarationExpression* vis){
		grid->add(new Cell(0, vis->declarationType(), 0));
		grid->add(new Cell(2, vis->name(), 2));
		if (vis->initialValue())
		{
			grid->add(new Cell(3, vis->assignmentSymbol(), 3));
			grid->add(new Cell(4, vis->initialValue(), 4));
		}
	});

	GridBasedOffsetProvider::addGridConstructor<VUnaryOperation>(
	[](GridBasedOffsetProvider* grid, VUnaryOperation* vis){
		grid->setFilterNullAndEmptyComponents();

		if (vis->node()->op() == OOModel::UnaryOperation::PARENTHESIS)
		{
			grid->add(new Cell(1, vis->operand(), 1));
			grid->setSize(3, 1);
		}
		else
			for(int i = 0; i<vis->layout()->length(); ++i)
				grid->add(new Cell(i, vis->layout()->at<Visualization::Item>(i), i));
	});

	GridBasedOffsetProvider::addGridConstructor<VFunctionType>(
	[](GridBasedOffsetProvider* grid, VFunctionType* vis){
		grid->add(new Cell(0, vis->icon(), 0));
		grid->add(new ListCell(1, vis->arguments(), 1, "(", ",", ")"));
		if (vis->results())
		{
			grid->add(new Cell(2, vis->resultSymbol(), 2));
			grid->add(new ListCell(3, vis->results(), 3, "(", ",", ")"));
		}
	});
}

} /* namespace OOInteraction */
