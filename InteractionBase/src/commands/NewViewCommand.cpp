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

#include "NewViewCommand.h"
#include "VisualizationBase/src/items/ViewItem.h"

namespace Interaction {

NewViewCommand::NewViewCommand()
	:MenuCommand("newView", QStringList({"name", ""}))
{
}

bool NewViewCommand::canUseTarget(Visualization::Item *, Visualization::Item *,
		const std::unique_ptr<Visualization::Cursor>&)
{
	return true;
}

CommandResult* NewViewCommand::executeWithArguments(Visualization::Item *, Visualization::Item *target,
		const QStringList& arguments, const std::unique_ptr<Visualization::Cursor>&)
{
	QString name = arguments.at(0);
	bool open = arguments.at(1).compare("open", Qt::CaseInsensitive) == 0;
	auto view = new Visualization::ViewItem(nullptr, name);
	target->scene()->addTopLevelItem(view, false);
	if (open)
		target->scene()->switchToView(view);
	return new CommandResult();
}

QString NewViewCommand::description(Visualization::Item *, Visualization::Item *,
		const QStringList &arguments, const std::unique_ptr<Visualization::Cursor> &)
{
	QString name = arguments.at(0);
	if (arguments.at(1).compare("open", Qt::CaseInsensitive) == 0)
		return "Create a new view named " + name + " and open it";
	else
		return "Create a new view named " + name + " without opening";
}
}
