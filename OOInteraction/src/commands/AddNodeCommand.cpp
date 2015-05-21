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

#include "AddNodeCommand.h"
#include "VisualizationBase/src/items/ViewItem.h"
#include "OOModel/src/declarations/Project.h"

namespace OOInteraction {

AddNodeCommand::AddNodeCommand()
	:MenuCommand("addNode", QStringList({"current", "0", "0"}))
{
}

bool AddNodeCommand::canUseTarget(Visualization::Item *, Visualization::Item *target,
		const std::unique_ptr<Visualization::Cursor>&)
{
	return target->hasNode() && dynamic_cast<OOModel::Method*>(target->node());
}

Interaction::CommandResult* AddNodeCommand::executeWithArguments(Visualization::Item *, Visualization::Item *target,
		const QStringList& arguments, const std::unique_ptr<Visualization::Cursor>&)
{
	auto method = dynamic_cast<OOModel::Method*>(target->node());
	auto name = arguments.at(0);
	if (name == "current")
		name = target->scene()->currentViewItem()->name();
	auto view = target->scene()->getViewItem(name);
	if (view)
	{
		view->insertNode(method);
		return new Interaction::CommandResult();
	}
	else
		return new Interaction::CommandResult(new Interaction::CommandError(
											"The view with name " + name + " does not exist"));
}

QString AddNodeCommand::description(Visualization::Item *, Visualization::Item *,
		const QStringList &arguments, const std::unique_ptr<Visualization::Cursor> &)
{
	auto name = arguments.at(0);
	if (name == "current")
		return "Add the current item to the current view at column " +
				arguments.at(1) + " and row " + arguments.at(2);
	else
		return "Add the current item to the view " + name + " at column " +
				arguments.at(1) + " and row " + arguments.at(2);

}
}
