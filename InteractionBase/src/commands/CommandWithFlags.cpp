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
#include "CommandWithFlags.h"
#include "CommandError.h"

namespace Interaction {

CommandWithFlags::CommandWithFlags(const QString& name, const QList<QStringList>& attributes,
	bool usePossibleNames, bool limitToMatchingNames)
: Command{name}, attributes_{attributes}, usePossibleNames_{usePossibleNames},
  limitToMatchingNames_{limitToMatchingNames}
{
}

bool CommandWithFlags::canInterpret(Visualization::Item* /*source*/, Visualization::Item* /*target*/,
		const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	QString name;
	QStringList attributes;
	bool commandFound;
	bool unknownFormat;

	findParts(commandTokens, name, attributes, commandFound, unknownFormat);

	return commandFound && !unknownFormat;
}

CommandResult* CommandWithFlags::execute(Visualization::Item* source,
		Visualization::Item* target, const QStringList& commandTokens,
		const std::unique_ptr<Visualization::Cursor>& cursor)
{
	QString name;
	QStringList attributes;
	bool commandFound;
	bool unknownFormat;

	findParts(commandTokens, name, attributes, commandFound, unknownFormat);

	if (usePossibleNames_ && limitToMatchingNames_)
	{
		auto matching = matchingNames(source, target, cursor, name);
		if (matching.isEmpty())
			return new CommandResult{new CommandError{name + " is not a valid name for " + this->name()}};
		if (matching.size() > 1 && !matching.contains(name)) // No exact match, but multiple options => ambigious
			return new CommandResult{new CommandError{name + " is ambiguous for " + this->name()}};

		name = matching.first();
	}

	return executeNamed(source, target, cursor, name, attributes);
}

QList<CommandSuggestion*> CommandWithFlags::suggest(Visualization::Item* source,
		Visualization::Item* target, const QString& textSoFar, const std::unique_ptr<Visualization::Cursor>& cursor)
{
	QString name;
	QStringList attributes;
	bool commandFound;
	bool unknownFormat;

	findParts(textSoFar.split(" "), name, attributes, commandFound, unknownFormat);

	if (unknownFormat) return {};
	return suggestNamed(source, target, textSoFar, cursor, name, attributes, commandFound);
}

QList<CommandSuggestion*> CommandWithFlags::suggestNamed(Visualization::Item* source,
		Visualization::Item* target, const QString& textSoFar,
		const std::unique_ptr<Visualization::Cursor>& cursor, const QString& name,
		const QStringList& attributes, bool commandFound)
{
	QString commandText = textSoFar + (commandFound?"":" " + this->name());
	QString explanation = this->name();
	bool atLeastOneAttribute = false;
	for (auto attr : attributes)
		if (!attr.isEmpty())
			{
				if (!atLeastOneAttribute) explanation += " [";
				explanation += (atLeastOneAttribute ? "," : "") + attr;
				atLeastOneAttribute = true;
			}
	if (atLeastOneAttribute) explanation += "]";

	QList<CommandSuggestion*> s;

	if (usePossibleNames_)
		for (auto matching : matchingNames(source, target, cursor, name))
			s.append(new CommandSuggestion{commandText + (name.isEmpty() ? " " + matching : ""),
					explanation + " " + matching});

	if (!usePossibleNames_ || !limitToMatchingNames_)
		s.append( new CommandSuggestion{commandText, explanation + (name.isEmpty() ? "" : " " + name)} );

	return s;
}

void CommandWithFlags::findParts(const QStringList& tokens, QString& name, QStringList& attributes,
				bool& commandFound, bool& unknownFormat, bool useFirstValueAsDefaultAttribute)
{
	name = QString{};
	attributes.clear();

	if (useFirstValueAsDefaultAttribute)
		for (auto attributeValues : attributes_) attributes.append(attributeValues.first());
	else
		for (int i = 0; i<attributes_.size(); ++i) attributes.append(QString{});

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
				if (this->name().startsWith(t)) commandFound = true;
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

QStringList CommandWithFlags::possibleNames(Visualization::Item*, Visualization::Item*,
		const std::unique_ptr<Visualization::Cursor>&)
{
	return {};
}

QStringList CommandWithFlags::matchingNames(Visualization::Item* source, Visualization::Item* target,
		const std::unique_ptr<Visualization::Cursor>& cursor, const QString& nameToLookFor)
{
	if (nameToLookFor.isEmpty()) return possibleNames(source, target, cursor);

	// Use a pattern like this 'a*b*c*' in order to simplify the search. Note that the first letter must match.
	QString searchPattern = nameToLookFor;
	for (int i = searchPattern.size(); i>=1; --i) searchPattern.insert(i, "*");
	auto regExp = QRegExp{searchPattern, Qt::CaseInsensitive, QRegExp::Wildcard};

	QStringList result;
	for (auto s : possibleNames(source, target, cursor))
		if (regExp.exactMatch(s)) result << s;

	return result;
}

}
