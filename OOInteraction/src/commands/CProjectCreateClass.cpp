/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * CProjectCreateClass.cpp
 *
 *  Created on: Mar 6, 2012
 *      Author: Dimitar Asenov
 */

#include "commands/CProjectCreateClass.h"

#include "OOModel/src/top_level/Project.h"
#include "OOModel/src/top_level/Class.h"


#include "InteractionBase/src/handlers/SetCursorEvent.h"

namespace OOInteraction {

bool CProjectCreateClass::canInterpret(Visualization::Item* /*source*/, Visualization::Item* /*target*/,
		const QStringList& commandTokens)
{
	QString name;
	QString visibility;
	bool classFound;
	bool unknownFormat;

	findClass(commandTokens, name, visibility, classFound, unknownFormat);

	return classFound && !unknownFormat;
}

Interaction::CommandResult* CProjectCreateClass::execute(Visualization::Item* /*source*/,
		Visualization::Item* target, const QStringList& commandTokens)
{
	QString name;
	QString visibility;
	bool classFound;
	bool unknownFormat;

	findClass(commandTokens, name, visibility, classFound, unknownFormat);

	auto pr = dynamic_cast<OOModel::Project*> (target->node());
	Q_ASSERT(pr);

	auto cl = new OOModel::Class();
	if (!name.isEmpty()) cl->setName(name);
	if (visibility == "private" ) cl->setVisibility(OOModel::Visibility::PRIVATE);
	else if (visibility == "protected" ) cl->setVisibility(OOModel::Visibility::PROTECTED);
	else if (visibility == "public" ) cl->setVisibility(OOModel::Visibility::PUBLIC);
	else cl->setVisibility(OOModel::Visibility::DEFAULT);

	pr->model()->beginModification(pr, "create class");
	pr->classes()->append(cl);
	pr->model()->endModification();

	if (name.isNull()) QApplication::postEvent(target->scene(),
		new Interaction::SetCursorEvent(target, cl->nameNode(), Interaction::SetCursorEvent::CursorOnLeft));
	else
		QApplication::postEvent(target->scene(),
			new Interaction::SetCursorEvent(target, cl->nameNode(), Interaction::SetCursorEvent::CursorOnRight));

	return new Interaction::CommandResult();
}

QList<Interaction::CommandSuggestion*> CProjectCreateClass::suggest(Visualization::Item* /*source*/,
		Visualization::Item* /*target*/, const QString& textSoFar)
{
	QList<Interaction::CommandSuggestion*> s;

	QString name;
	QString visibility;
	bool classFound;
	bool unknownFormat;

	findClass(textSoFar.split(" "), name, visibility, classFound, unknownFormat);

	if (!unknownFormat)
	{
		QString commandText = textSoFar + (classFound?"":" class");
		QString explanation = "Create a ";
		if (!visibility.isEmpty()) explanation += visibility + " ";
		explanation += "class" + (name.isEmpty() ? "" : " called '" + name + "'");
		s.append(new Interaction::CommandSuggestion(commandText, explanation));
	}

	return s;
}

QStringList CProjectCreateClass::commandForms(Visualization::Item* /*source*/, Visualization::Item* /*target*/,
		const QString& /*textSoFar*/)
{
	//TODO: Implement this
	return QStringList();
}

void CProjectCreateClass::findClass(const QStringList& tokens, QString& name, QString& visibility,
				bool& classFound, bool& unknownFormat)
{
	name = QString();
	visibility = QString();
	classFound = false;
	unknownFormat = false;

	if (tokens.size() <= 4)
	{
		for(QString t: tokens)
		{
			if (!name.isNull()) // The name should be the last argument
			{
				unknownFormat = true;
				break;
			}

			// Before the 'class' keyword even a prefix of the other keywords is enough.
			// After the 'class' keyword, anything which is not precise is considered the class name.
			if ( classFound ? t == "private" : QString("private").startsWith(t)) visibility = "private";
			else if ( classFound ? t == "public" : QString("public").startsWith(t)) visibility = "public";
			else if ( classFound ? t == "protected" : QString("protected").startsWith(t)) visibility = "protected";
			else if (QString("class").startsWith(t)) classFound = true;
			else if (classFound) name = t;
			else
			{
				unknownFormat = true;
				break;
			}
		}
	}
	else
	{
		unknownFormat = true;
	}
}

} /* namespace OOInteraction */
