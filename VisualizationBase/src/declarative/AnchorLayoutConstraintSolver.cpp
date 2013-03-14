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

#include "AnchorLayoutConstraintSolver.h"

#include "Element.h"

#include <lpsolve/lp_lib.h>

namespace Visualization {

AnchorLayoutConstraintSolver::AnchorLayoutConstraintSolver()
{
	// TODO Auto-generated constructor stub

}

AnchorLayoutConstraintSolver::~AnchorLayoutConstraintSolver()
{
	// TODO Auto-generated destructor stub
}


void AnchorLayoutConstraintSolver::placeElements(QList<Element*>& elements, QList<AnchorLayoutAnchor*>& anchors,
		AnchorLayoutAnchor::Orientation orientation, Item* item)
{
	// elements already have a minimum size

	initializeConstraintSolver(elements.size() * 2);

	// add size constraints for each element
	// if element's size depends on parent's size
	// 	element.end - element.start >= element.size
	// else
	// 	element.end - element.start = element.size
	for (auto e : elements)
	{
		int elementIndex = elements.indexOf(e);
		QVector<QPair<int, float>> constraintRow = {QPair<int, float>(endVariable(elementIndex), 1.0),
																	QPair<int, float>(startVariable(elementIndex), -1.0)};
		float size;
		if (orientation == AnchorLayoutAnchor::Orientation::Horizontal)
			size = (float) e->size().width();
		else // orientation == AnchorLayoutAnchor::Orientation::Vertical
			size = (float) e->size().height();
		if (e->sizeDependsOnParent(item))
			addGreaterEqualConstraint(constraintRow, size);
		else
			addEqualConstraint(constraintRow, size);
	}

	// add constraints for each anchor
	// placeElement.start + relativePlaceEdgePosition * placeElement.end
	// == fixedElement.start + relativeFixedEdgePosition * fixedElement.end + offset
	for (auto a : anchors)
	{
		int placeElementIndex = elements.indexOf(a->placeElement());
		int fixedElementIndex = elements.indexOf(a->fixedElement());

		QVector<QPair<int, float>> constraintRow =
			{
					QPair<int, float>(startVariable(placeElementIndex),1 - a->relativePlaceEdgePosition()),
					QPair<int, float>(endVariable(placeElementIndex),a->relativePlaceEdgePosition()),
					QPair<int, float>(startVariable(fixedElementIndex), -(1 - a->relativeFixedEdgePosition())),
					QPair<int, float>(endVariable(fixedElementIndex),-a->relativeFixedEdgePosition())
			};
		addEqualConstraint(constraintRow, (float) a->offset());
	}

	// add objective function
	// minimize the sum of the sizes of all the elements
	QVector<float> objectiveRow;
	for (int i=0; i<elements.size(); ++i)
	{
		objectiveRow.append(-1.0); // start variable of i-th element
		objectiveRow.append(1.0); // end variable of i-th element
	}
	setMinimizeObjective(objectiveRow);

	QVector<float> solution = solveConstraints();

	cleanUpConstraintSolver();

	// Apply the solution
	for (int i=0; i<elements.size(); ++i)
	{
		Element* element = elements.at(i);
		int size = std::ceil(solution[endVariable(i)] - solution[startVariable(i)]);
		int position = std::ceil(solution[startVariable(i)]);

		if (orientation == AnchorLayoutAnchor::Orientation::Horizontal)
		{
			if (size > element->size().width())
				element->computeSize(item, size, element->size().height());
			element->setPos(QPoint(position, element->pos().y()));
		}
		else // orientation == AnchorLayoutAnchor::Orientation::Vertical
		{
			if (size > element->size().height())
				element->computeSize(item, element->size().width(), size);
			element->setPos(QPoint(element->pos().x(), position));
		}
	}
}

void AnchorLayoutConstraintSolver::addGreaterEqualConstraint(QVector<QPair<int, float>> constraintRow, float result)
{
	for (int i=0; i<constraintRow.size(); ++i)
	{
		columnIndices_[i] = constraintRow[i].first + 1;
		rowValues_[i] = (double) constraintRow[i].second;
	}
	bool success = add_constraintex(lp_, constraintRow.size(), rowValues_, columnIndices_, GE, result);
	Q_ASSERT(success);
}

void AnchorLayoutConstraintSolver::addEqualConstraint(QVector<QPair<int, float>> constraintRow, float result)
{
	for (int i=0; i<constraintRow.size(); ++i)
	{
		columnIndices_[i] = constraintRow[i].first + 1;
		rowValues_[i] = (double) constraintRow[i].second;
	}
	bool success = add_constraintex(lp_, constraintRow.size(), rowValues_, columnIndices_, EQ, result);
	Q_ASSERT(success);
}

void AnchorLayoutConstraintSolver::setMinimizeObjective(QVector<float> objectiveRow)
{
	set_add_rowmode(lp_, false);
	for (int i=0; i<numVariables_; ++i)
	{
		columnIndices_[i] = i+1;
		rowValues_[i] = (double) objectiveRow[i];
	}
	bool success = set_obj_fnex(lp_, numVariables_, rowValues_, columnIndices_);
	Q_ASSERT(success);
	set_minim(lp_);
}

QVector<float> AnchorLayoutConstraintSolver::solveConstraints()
{
	set_verbose(lp_, CRITICAL);
	solve(lp_);
	get_variables(lp_, rowValues_);
	QVector<float> result;
	for (int i=0; i<numVariables_; ++i)
		result.append((float) rowValues_[i]);
	return result;
}

void AnchorLayoutConstraintSolver::initializeConstraintSolver(int numVariables)
{
	lp_ = make_lp(0, numVariables);
	Q_ASSERT(lp_ != nullptr);
	rowValues_ = new double[numVariables];
	columnIndices_ = new int[numVariables];
	numVariables_ = numVariables;
	set_add_rowmode(lp_, true);
}

void AnchorLayoutConstraintSolver::cleanUpConstraintSolver()
{
	SAFE_DELETE(lp_);
	// make safe, add to destructor
	delete[] rowValues_;
	delete[] columnIndices_;
	numVariables_ = 0;
}

} /* namespace Visualization */
