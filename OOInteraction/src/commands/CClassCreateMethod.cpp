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
 * CClassCreateMethod.cpp
 *
 *  Created on: Mar 1, 2012
 *      Author: Dimitar Asenov
 */

#include "commands/CClassCreateMethod.h"

#include "OOModel/src/top_level/Class.h"
#include "OOModel/src/top_level/Method.h"

#include "InteractionBase/src/handlers/SetCursorEvent.h"

namespace OOInteraction {

bool CClassCreateMethod::canInterpret(Visualization::Item* /*source*/, Visualization::Item* /*target*/,
		const QStringList& commandTokens)
{
	QString name;
	QString visibility;
	QString storage;
	bool methodFound;
	bool unknownFormat;

	findMethod(commandTokens, name, visibility, storage, methodFound, unknownFormat);

	return methodFound && !unknownFormat;
}

Interaction::CommandResult* CClassCreateMethod::execute(Visualization::Item* /*source*/,
		Visualization::Item* target, const QStringList& commandTokens)
{
	QString name;
	QString visibility;
	QString storage;
	bool methodFound;
	bool unknownFormat;

	findMethod(commandTokens, name, visibility, storage, methodFound, unknownFormat);

	auto cl = dynamic_cast<OOModel::Class*> (target->node());
	Q_ASSERT(cl);

	auto m = new OOModel::Method();
	if (!name.isEmpty()) m->setName(name);
	if (visibility == "private" ) m->setVisibility(OOModel::Visibility::PRIVATE);
	else if (visibility == "protected" ) m->setVisibility(OOModel::Visibility::PROTECTED);
	else if (visibility == "public" ) m->setVisibility(OOModel::Visibility::PUBLIC);
	else m->setVisibility(OOModel::Visibility::DEFAULT);
	if (storage == "static") m->setStorageSpecifier(OOModel::StorageSpecifier::CLASS_VARIABLE);
	else m->setStorageSpecifier(OOModel::StorageSpecifier::INSTANCE_VARIABLE);

	cl->model()->beginModification(cl, "create method");
	cl->methods()->append(m);
	cl->model()->endModification();

	if (name.isNull()) QApplication::postEvent(target->scene(),
		new Interaction::SetCursorEvent(target, m->nameNode(), Interaction::SetCursorEvent::CursorOnLeft));
	else
		QApplication::postEvent(target->scene(),
			new Interaction::SetCursorEvent(target, m->nameNode(), Interaction::SetCursorEvent::CursorOnRight));

	return new Interaction::CommandResult();
}

QList<Interaction::CommandSuggestion*> CClassCreateMethod::suggest(Visualization::Item* /*source*/,
		Visualization::Item* /*target*/, const QString& textSoFar)
{
	QList<Interaction::CommandSuggestion*> s;

	QString name;
	QString visibility;
	QString storage;
	bool methodFound;
	bool unknownFormat;

	findMethod(textSoFar.split(" "), name, visibility, storage, methodFound, unknownFormat);

	if (!unknownFormat)
	{
		QString commandText = textSoFar + (methodFound?"":" method");
		QString explanation = "Create a ";
		if (!visibility.isEmpty()) explanation += visibility + " ";
		if (!storage.isEmpty()) explanation += storage + " ";
		explanation += "method" + (name.isEmpty() ? "" : " called '" + name + "'");
		s.append(new Interaction::CommandSuggestion(commandText, explanation));
	}

	return s;
}

QStringList CClassCreateMethod::commandForms(Visualization::Item* /*source*/, Visualization::Item* /*target*/,
		const QString& /*textSoFar*/)
{
	//TODO: Implement this
	return QStringList();
}

void CClassCreateMethod::findMethod(const QStringList& tokens, QString& name, QString& visibility, QString& storage,
				bool& methodFound, bool& unknownFormat)
{
	name = QString();
	visibility = QString();
	storage = QString();
	methodFound = false;
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

			// Before the 'method' keyword even a prefix of the other keywords is enough.
			// After the 'method' keyword, anything which is not precise is considered the method name.
			if ( methodFound ? t == "private" : QString("private").startsWith(t)) visibility = "private";
			else if ( methodFound ? t == "public" : QString("public").startsWith(t)) visibility = "public";
			else if ( methodFound ? t == "protected" : QString("protected").startsWith(t)) visibility = "protected";
			else if ( methodFound ? t == "static" : QString("static").startsWith(t)) storage = "static";
			else if (QString("method").startsWith(t)) methodFound = true;
			else if (methodFound) name = t;
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
