/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "ChangeMonitor.h"

#include "ValueAtReturnVisitor.h"
#include "OOModel/src/expressions/Expression.h"
#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/model/TreeManager.h"

namespace ContractsLibrary {

ChangeMonitor::ChangeMonitor()
{}

ChangeMonitor::~ChangeMonitor()
{}

void ChangeMonitor::listenToTreeManager(Model::TreeManager* manager)
{
	connect(manager, &Model::TreeManager::nodesModified, this,
			  &ChangeMonitor::nodesModified, Qt::QueuedConnection);
}

void ChangeMonitor::nodesModified(QSet<Node*> modifiedNodes, QSet<Node*>)
{
	ValueAtReturnVisitor v;
	for (auto n : modifiedNodes) v.visit(n);
}

void ChangeMonitor::expressionModified(OOModel::Expression*& exp, int& cursorIndex)
{
	ValueAtReturnVisitor v;
	v.visit(exp);
	const int length = QString{"Contract.ValueAtReturn("}.size();
	cursorIndex += (v.numWrapped() - v.numUnwrapped())*length;
}

}
