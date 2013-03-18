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

#ifndef VisualizationBase_ANCHORLAYOUTCONSTRAINTSOLVER_H_
#define VisualizationBase_ANCHORLAYOUTCONSTRAINTSOLVER_H_

#include "AnchorLayoutAnchor.h"

#include <lpsolve/lp_lib.h>

namespace Visualization {

class Element;
class Item;

class AnchorLayoutConstraintSolver {
	public:
		AnchorLayoutConstraintSolver();
		virtual ~AnchorLayoutConstraintSolver();
		void placeElements(QList<Element*>& elements, QList<AnchorLayoutAnchor*>& anchors,
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

		lprec* lp_{};
		double* rowValues_{};
		int* columnIndices_{};
		int numVariables_{};

};

inline void AnchorLayoutConstraintSolver::initializeConstraintSolver(int numVariables)
{
	lp_ = make_lp(0, numVariables);
	Q_ASSERT(lp_ != nullptr);
	rowValues_ = new double[numVariables];
	columnIndices_ = new int[numVariables];
	numVariables_ = numVariables;
	set_add_rowmode(lp_, true);
}
inline void AnchorLayoutConstraintSolver::cleanUpConstraintSolver()
{
	SAFE_DELETE(lp_);
	delete[] rowValues_;
	delete[] columnIndices_;
	numVariables_ = 0;
}
inline int AnchorLayoutConstraintSolver::startVariable(int elementIndex)
{
	return 2 * elementIndex;
}
inline int AnchorLayoutConstraintSolver::endVariable(int elementIndex)
{
	return 2 * elementIndex + 1;
}
} /* namespace Visualization */
#endif /* VisualizationBase_ANCHORLAYOUTCONSTRAINTSOLVER_H_ */
