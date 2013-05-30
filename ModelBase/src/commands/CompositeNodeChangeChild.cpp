/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "CompositeNodeChangeChild.h"
#include "nodes/Node.h"

#include "ModelException.h"

namespace Model {

CompositeNodeChangeChild::CompositeNodeChangeChild(Node* target, Node* newValue_,
		const CompositeIndex &attributeIndex_, QVector< QVector<Node*> >* subnodes_) :
		NodeOwningCommand(target, "set node", (*subnodes_)[attributeIndex_.level()][attributeIndex_.index()], newValue_),
		newVal(newValue_), oldVal((*subnodes_)[attributeIndex_.level()][attributeIndex_.index()]),
	attributeIndex(attributeIndex_), subnodes(subnodes_)
{
	if (newValue_ && newValue_->parent())
		throw ModelException("Set as a child of CompositeNode a node that already has a parent.");
}

void CompositeNodeChangeChild::redo()
{
	(*subnodes)[attributeIndex.level()][attributeIndex.index()] = newVal;
	if (newVal) newVal->setParent(target());
	if (oldVal) oldVal->setParent(nullptr);
	NodeOwningCommand::redo();
}

void CompositeNodeChangeChild::undo()
{
	(*subnodes)[attributeIndex.level()][attributeIndex.index()] = oldVal;
	if (newVal) newVal->setParent(nullptr);
	if (oldVal) oldVal->setParent(target());
	NodeOwningCommand::undo();
}

}
