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

#pragma once

#include "AnchorLayoutAnchor.h"

struct _lprec;

namespace Visualization {

class Element;
class Item;

/**
 * This class can solve the linear program given by a list of elements and anchors. That is, it can place elements
 * along an axis, even if there are some circular dependencies involved. This solver is only used, if there are such
 * dependencies along the axis to be handled.
 *
 * The library used to solve the linear programming problem is lp_solve.
 */
class AnchorLayoutConstraintSolver {
	public:
		AnchorLayoutConstraintSolver();
		virtual ~AnchorLayoutConstraintSolver();
		/**
		 * Taking a list of \a elements and a list of \a anchors, computes the positions along the axis given by
		 * \a orientation for all the \a elements, relative to the \a item.
		 */
		void placeElements(const QVector<Element*>& elements, QList<AnchorLayoutAnchor*>& anchors,
				AnchorLayoutAnchor::Orientation orientation, Item* item);
	private:
		void initializeConstraintSolver(int numVariables);
		void addGreaterEqualConstraint(QVector<QPair<int, float>> constraintRow, float result);
		void addEqualConstraint(QVector<QPair<int, float>> constraintRow, float result);
		void setMinimizeObjective(QVector<float> objectiveRow);
		QVector<float> solveConstraints();
		void cleanUpConstraintSolver();

		int startVariable(int elementIndex);
		int endVariable(int elementIndex);

		_lprec* lp_{};
		double* rowValues_{};
		int* columnIndices_{};
		int numVariables_{};

};

inline int AnchorLayoutConstraintSolver::startVariable(int elementIndex)
{
	return 2 * elementIndex;
}
inline int AnchorLayoutConstraintSolver::endVariable(int elementIndex)
{
	return 2 * elementIndex + 1;
}
} /* namespace Visualization */
