/***********************************************************************************************************************
**
** Copyright (c) 2016 ETH Zurich
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

#include "ListMergeComponentV2.h"
#include "../../filepersistence_api.h"
#include "MergeChange.h"

#include "ModelBase/src/persistence/PersistentStore.h"
#include "../Diff3Parse.h"
#include "../../simple/GenericNode.h"
#include "../../simple/GenericTree.h"

namespace FilePersistence {

bool ListMergeComponentV2::isList(const QString& type)
{
	return type.startsWith("TypedListOf") || type.endsWith("List");
}

bool ListMergeComponentV2::isUnorderedList(const QString& type)
{
	// TODO: The types below sould be configured by plug-ins, not hardcoded here.
	static QStringList unorderedListTypes{"TypedListOfClass",
													  "TypedListOfMethod",
													  "TypedListOfFormalArgument",
													  "TypedListOfFormalResult",
													  "TypedListOfField",
													  "TypedListOfUsedLibrary"};
	return unorderedListTypes.contains(type);
}

bool ListMergeComponentV2::isOrderedList(const QString& type)
{
	return !isUnorderedList(type) && isList(type);
}

void ListMergeComponentV2::run(MergeData& mergeData)
{
	//	applyNonConflictingChanges(mergeData);
	// Temporarily remove update changes(Conflicts)
	auto listsToMerge = computeListsToMerge(mergeData);
	for (auto listContainerId : listsToMerge)
	{
		auto map = getAdjustedIndices(listContainerId, mergeData);
		adjustCG(map, mergeData);
		//	applyChanges(mergeData);
		// removeHoles();
		// Restore Update Changes(Conflicts)
		// Report Conflicts
	}

}


}
