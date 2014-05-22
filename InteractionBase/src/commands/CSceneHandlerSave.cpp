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
#include "CSceneHandlerSave.h"

#include "VisualizationBase/src/items/RootItem.h"
#include "VisualizationBase/src/VisualizationManager.h"
#include "FilePersistence/src/simple/SimpleTextFileStore.h"
#include "ModelBase/src/model/Model.h"

using namespace Visualization;

namespace Interaction {

bool CSceneHandlerSave::canInterpret(Item*, Item*, const QStringList& commandTokens)
{
	return (commandTokens.size() <= 2) && QString("save").startsWith(commandTokens.first());
}

CommandResult* CSceneHandlerSave::execute(Item* source, Item*, const QStringList& commandTokens)
{
	Model::Node* node = nullptr;
	auto p = source;
	while (p)
	{
		node = p->node();
		if (node) break;
		p = p->parent();
	}

	Q_ASSERT(node);
	auto model = node->model();
	Q_ASSERT(model);

	if (commandTokens.size() == 1)
	{
		// Just save the model
		model->save();
	}

	if (commandTokens.size() == 2)
	{
		// Save the model at a particular location and with a particular name
		model->setName(commandTokens[1]);
		model->save(new FilePersistence::SimpleTextFileStore("projects/"));
	}

	return new CommandResult();
}

QList<CommandSuggestion*> CSceneHandlerSave::suggest(Item*, Item*, const QString& textSoFar)
{
	auto trimmed = textSoFar.trimmed();
	auto split = trimmed.split(" ");
	if (split.size() > 2 ) return {};
	if (split.size() > 0 && ! QString("save").startsWith(split.first())) return {};

	auto name = split.size() > 1 ? split[1] : QString();

	QString commandString = "save";
	QString text = "Save the current model";
	if (!name.isNull())
	{
		commandString += " " + name;
		text += " with the name " + name;
	}

	return {new CommandSuggestion(commandString, text)};
}

QStringList CSceneHandlerSave::commandForms(Item*, Item*, const QString& textSoFar)
{
	QStringList forms;
	if (textSoFar.isEmpty() || QString("save").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive) )
		forms.append("save");
	return forms;
}

} /* namespace Interaction */
