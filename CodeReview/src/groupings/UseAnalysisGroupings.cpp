/***********************************************************************************************************************
**
** Copyright (c) 2011, 2016 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
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

#include "UseAnalysisGroupings.h"

#include "ModelBase/src/nodes/Reference.h"

namespace CodeReview
{

bool const UseAnalysisGroupings::ONLY_CONSIDER_CHANGES = true;

UseAnalysisGroupings::DiffFrameInfo UseAnalysisGroupings::computeDiffFrameDependenciesAndPresentations
		(VersionControlUI::DiffFrame* diffFrame)
{
	auto oldNode = diffFrame->oldVersionNode();
	auto newNode = diffFrame->newVersionNode();

	if (!oldNode && !newNode)
		return {};

	QList<Model::Node*> stack;
	if (oldNode)
		stack << oldNode;
	if (newNode)
		stack << newNode;

	DiffFrameInfo diffFrameInfo;

	while (!stack.isEmpty())
	{
		auto top = stack.takeLast();
		if (top)
		{

			if (auto referenceNode = DCast<Model::Reference>(top))
			{
				if (referenceNode->isResolved())
					diffFrameInfo.refersTo_.insert(referenceNode->target());

			}
			else
			{
				stack.append(top->children());
				diffFrameInfo.shows_.insert(top);
			}
		}
	}

	// remove ids which are shown by this DiffFrame from the dependencies
	diffFrameInfo.refersTo_.subtract(diffFrameInfo.shows_);

	return diffFrameInfo;
}


GroupedDiffFrames UseAnalysisGroupings::useAnalysisGrouping(DiffFrames diffFrame)
{
	QList<QList<VersionControlUI::DiffFrame*>> result;

	QHash<VersionControlUI::DiffFrame*, DiffFrameInfo> diffPairsWithInfo;
	QHash<VersionControlUI::DiffFrame*, QList<VersionControlUI::DiffFrame*>*> groupLists;

	for (auto diffCompPair : diffFrame)
	{
		auto info = computeDiffFrameDependenciesAndPresentations(diffCompPair);

		diffPairsWithInfo.insert(diffCompPair, info);
	}

	for (auto diffPair : diffPairsWithInfo.keys())
	{
		bool dependsOnOther = false;

		for (auto otherDiffPair : diffPairsWithInfo.keys())
		{
			if (diffPair == otherDiffPair) continue;

			if (dependsOn(diffPairsWithInfo.value(diffPair), otherDiffPair, diffPairsWithInfo.value(otherDiffPair)))
			{
				dependsOnOther = true;
				auto iter = groupLists.find(otherDiffPair);
				if (iter != groupLists.end())
				{
					(*iter)->append(diffPair);
					groupLists.insert(diffPair, *iter);
				} else
				{
					result.prepend({});
					result.first().append(diffPair);
					result.first().append(otherDiffPair);
					groupLists.insert(diffPair, &result.first());
					groupLists.insert(otherDiffPair, &result.first());
				}
			}
		}

		if (!dependsOnOther && !groupLists.contains(diffPair))
		{
			result.prepend({});
			result.first().append(diffPair);
			groupLists.insert(diffPair, &result.first());
		}
	}
	return result;
}

bool UseAnalysisGroupings::dependsOn(DiffFrameInfo infoA,
												VersionControlUI::DiffFrame* frameB, DiffFrameInfo infoB)
{
	if (ONLY_CONSIDER_CHANGES)
	{
		for (auto referal : infoA.refersTo_)
		{
			for (auto changesOld : frameB->oldChangedNodes())
				if (changesOld->isSameOrAncestorOf(referal))
					return true;
			for (auto changesNew : frameB->newChangedNodes())
				if (changesNew->isSameOrAncestorOf(referal))
					return true;
		}
		return false;
	} else
	{
		auto refers = infoA.refersTo_;
		auto shows = infoB.shows_;
		return !refers.intersect(shows).isEmpty();
	}
}

}
