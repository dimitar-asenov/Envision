/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "ListMergeComp.h"
#include "ConflictPairs.h"

namespace FilePersistence {

ListMergeComp::ListMergeComp(QSet<QString>& listTypes)
{
	listTypes_ = QSet<QString>(listTypes);
}

void ListMergeComp::run(const std::unique_ptr<GenericTree>&,
								const std::unique_ptr<GenericTree>&,
								const std::unique_ptr<GenericTree>&,
								ChangeDependencyGraph& cdgA,
								ChangeDependencyGraph& cdgB,
								QSet<ChangeDescription*>& conflictingChanges,
								ConflictPairs& conflictPairs)
{
	computeListsToMerge(cdgA, cdgB, conflictingChanges, conflictPairs);
}

void ListMergeComp::computeListsToMerge(ChangeDependencyGraph& cdgA,
															  ChangeDependencyGraph& cdgB,
															  QSet<ChangeDescription*>& conflictingChanges,
															  ConflictPairs& conflictPairs)
{
	foreach (ChangeDescription* change, conflictingChanges)
	{
		if (onlyChildStructure(change) && listTypes_.contains(change->nodeA()->type()))
		{
			// node of list type and one branch only changes child structure
			ConflictPairs::iterator other = conflictPairs.find(change);
			while (other != conflictPairs.end() &&	other.key() == change &&
					 other.value()->id() != change->id()) other++;
			if (other != conflictPairs.end() && onlyChildStructure(other.value()))
			{
				// other branch only changes child structure
				// now gather all element ids
				QSet<Model::NodeIdType> idsToCheck;
				foreach (GenericNode* child, change->nodeA()->children()) {
					idsToCheck.insert(child->id());
				}
				foreach (GenericNode* child, change->nodeB()->children()) {
					idsToCheck.insert(child->id());
				}
				foreach (GenericNode* child, other.value()->nodeB()->children()) {
					idsToCheck.insert(child->id());
				}
				bool mergeOk = true;
				// for all elements, check that conflicts are on label only
				foreach (Model::NodeIdType id, idsToCheck) {
					IdToChangeDescriptionHash::const_iterator elemChange1 = cdgA.changes().find(id);
					if (elemChange1 != cdgA.changes().end() && elemChange1.key() == id &&
						 !onlyLabel(elemChange1.value()))
					{
						// branch A changes more than label
						ConflictPairs::iterator elemChange2 = conflictPairs.find(elemChange1.value());
						while (elemChange2 != conflictPairs.end() && elemChange2.key() == elemChange1.value())
						{
							if (!(elemChange2.value()->id() == id && onlyLabel(elemChange2.value())))
							{
								// unresolvable
								mergeOk = false;
								break;
							}
						}
					}
					// the same symmetrically
					elemChange1 = cdgB.changes().find(id);
					if (elemChange1 != cdgB.changes().end() && elemChange1.key() == id &&
						 !onlyLabel(elemChange1.value()))
					{
						// branch A changes more than label
						ConflictPairs::iterator elemChange2 = conflictPairs.find(elemChange1.value());
						while (elemChange2 != conflictPairs.end() && elemChange2.key() == elemChange1.value())
						{
							if (!(elemChange2.value()->id() == id && onlyLabel(elemChange2.value())))
							{
								// unresolvable
								mergeOk = false;
								break;
							}
						}
					}
					if (!mergeOk) break;
				}
				if (mergeOk) listsToMerge_.insert(change->nodeA());
			}
		}
	}
}

} /* namespace FilePersistence */
