/***********************************************************************************************************************
 **
 ** Copyright (c) 2015 ETH Zurich
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
#include "OOActions.h"

#include "../commands/CommandHelper.h"

#include "OOVisualization/src/declarations/VClass.h"
#include "FilePersistence/src/SystemClipboard.h"

namespace OOInteraction {

bool OOActions::pasteIntoTopLevelContainer(Visualization::Item *target, QKeySequence,
														 Interaction::ActionRegistry::InputState)
{
	if (target->ignoresCopyAndPaste()) return false;

	auto node = target->findAncestorWithNode()->node();
	if (!node) return false;

	FilePersistence::SystemClipboard clipboard;
	auto manager = node->manager();

	auto cursor = std::unique_ptr<Visualization::Cursor>{target->scene()->mainCursor()->clone()};

	auto theClass = DCast<OOModel::Class>(node);
	for (int i = 0; i<clipboard.numNodes(); ++i)
	{
		// We provide a null parent as this will be set in the instruction below.
		Model::Node* newNode = clipboard.create(manager, nullptr);

		if (auto newClass = DCast<OOModel::Class>(newNode))
		{
			CommandHelper::addToParent(theClass, theClass->classes(), newClass,
												theClass->classes()->nodes() + theClass->methods()->nodes(),
												target, cursor, false);
		}
		else if (auto newMethod = DCast<OOModel::Method>(newNode))
		{
			CommandHelper::addToParent(theClass, theClass->methods(), newMethod,
												theClass->classes()->nodes() + theClass->methods()->nodes(),
												target, cursor, false);
		}
		else
			SAFE_DELETE(newNode);

		if (clipboard.hasNext() ) clipboard.next();
	}

	target->setUpdateNeeded(Visualization::Item::StandardUpdate);
	return true;
}

}
