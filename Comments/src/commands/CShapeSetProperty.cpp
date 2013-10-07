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

#include "CShapeSetProperty.h"
#include "items/VCommentDiagramShape.h"

#include "VisualizationBase/src/VisualizationManager.h"

namespace Comments {

bool CShapeSetProperty::canInterpret(Visualization::Item*, Visualization::Item*, const QStringList& commandTokens)
{
	return commandTokens.size() == 2 &&
			(commandTokens.first() == "textcolor"
					|| commandTokens.first() == "bgcolor"
					|| commandTokens.first() == "bordercolor");
}

Interaction::CommandResult* CShapeSetProperty::execute(Visualization::Item*, Visualization::Item* target, const QStringList& commandTokens)
{
	auto shape = dynamic_cast<VCommentDiagramShape*>(target);
	shape->node()->model()->beginModification(shape->node(), "Setting color");

	if(commandTokens.first() == "textcolor")
		shape->node()->setTextColor(commandTokens.last());
	else if(commandTokens.first() == "bgcolor")
		shape->node()->setBackgroundColor(commandTokens.last());
	else if(commandTokens.first() == "bordercolor")
		shape->node()->setShapeColor(commandTokens.last());

	shape->node()->model()->endModification();
	shape->setUpdateNeeded(Visualization::Item::StandardUpdate);
	return new Interaction::CommandResult();
}

QList<Interaction::CommandSuggestion*> CShapeSetProperty::suggest(Visualization::Item*, Visualization::Item*, const QString& textSoFar)
{
	QList<Interaction::CommandSuggestion*> s;
	if(QString("textcolor").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive))
		s.append(new Interaction::CommandSuggestion("textcolor", "Set shape's foreground color"));
	if(QString("bgcolor").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive))
		s.append(new Interaction::CommandSuggestion("bgcolor", "Set shape's background color"));
	if(QString("bordercolor").startsWith(textSoFar.trimmed(), Qt::CaseInsensitive))
		s.append(new Interaction::CommandSuggestion("bordercolor", "Set shape's border color"));
	return s;
}

QStringList CShapeSetProperty::commandForms(Visualization::Item*, Visualization::Item*, const QString&)
{
	QStringList forms;
	return forms;
}

}
