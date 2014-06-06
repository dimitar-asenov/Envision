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

#include "CreateNamedObjectWithAttributes.h"

namespace Interaction {

CreateNamedObjectWithAttributes::CreateNamedObjectWithAttributes(const QString& commandName,
		const QList<QStringList>& attributes): commandName_(commandName), attributes_(attributes)
{
}

bool CreateNamedObjectWithAttributes::canInterpret(Visualization::Item* /*source*/, Visualization::Item* /*target*/,
		const QStringList& commandTokens)
{
	QString name;
	QStringList attributes;
	bool commandFound;
	bool unknownFormat;

	findParts(commandTokens, name, attributes, commandFound, unknownFormat);

	return commandFound && !unknownFormat;
}

CommandResult* CreateNamedObjectWithAttributes::execute(Visualization::Item* source,
		Visualization::Item* target, const QStringList& commandTokens)
{
	QString name;
	QStringList attributes;
	bool commandFound;
	bool unknownFormat;

	findParts(commandTokens, name, attributes, commandFound, unknownFormat);

	return create(source, target, name, attributes);
}

QList<CommandSuggestion*> CreateNamedObjectWithAttributes::suggest(Visualization::Item* /*source*/,
		Visualization::Item* /*target*/, const QString& textSoFar)
{
	QList<CommandSuggestion*> s;

	QString name;
	QStringList attributes;
	bool commandFound;
	bool unknownFormat;

	findParts(textSoFar.split(" "), name, attributes, commandFound, unknownFormat);

	if (!unknownFormat)
	{
		QString commandText = textSoFar + (commandFound?"":" " + commandName_);
		QString explanation = "Create a ";
		for (auto attr : attributes)
			if (!attr.isEmpty()) explanation += attr + " ";
		explanation += commandName_ + (name.isEmpty() ? "" : " called '" + name + "'");
		s.append(new CommandSuggestion(commandText, explanation));
	}

	return s;
}

QStringList CreateNamedObjectWithAttributes::commandForms(Visualization::Item* /*source*/,
		Visualization::Item* /*target*/, const QString& /*textSoFar*/)
{
	//TODO: Implement this
	return QStringList();
}

void CreateNamedObjectWithAttributes::findParts(const QStringList& tokens, QString& name, QStringList& attributes,
				bool& commandFound, bool& unknownFormat, bool useFirstValueAsDefaultAttribute)
{
	name = QString();
	attributes.clear();

	if (useFirstValueAsDefaultAttribute)
		for (auto attributeValues : attributes_) attributes.append(attributeValues.first());
	else
		for (int i = 0; i<attributes_.size(); ++i) attributes.append(QString());

	commandFound = false;
	unknownFormat = false;

	if (tokens.size() <= 2 + attributes_.size()) // 1 for name, 1 for command and the rest for the number of attributes.
	{
		int index = 0; // To keep track of how many tokens we've visited
		for (QString t: tokens)
		{
			++index;
			// If we've seen a command name and this is the last token, it is considered the name
			if (commandFound && index == tokens.size())
			{
				name = t;
				break;
			}

			// Try to interpret this token as an attribute
			auto attr = attributes.begin();
			bool found = false;
			for (auto attrValues : attributes_)
			{
				for (auto val : attrValues)
				{
					// A prefix of the attributes is enough
					if (val.startsWith(t))
					{
						*attr = val;
						found = true;
						break;
					}
				}
				if (found) break;
				++attr;
			}

			// If this token was not recognized as an attribute try the command name
			if (!found)
			{
				if (commandName_.startsWith(t)) commandFound = true;
				else
				{
					unknownFormat = true;
					break;
				}
			}
		}
	}
	else unknownFormat = true;
}

} /* namespace OOInteraction */
