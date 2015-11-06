/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "ArgumentParser.h"
#include "QueryParsingException.h"

namespace InformationScripting {

const QStringList ArgumentParser::SCOPE_ARGUMENT_NAMES{"s", "scope"};

ArgumentParser::ArgumentParser(std::initializer_list<QCommandLineOption> options,
													  const QStringList& args)
	: argParser_{std::make_unique<QCommandLineParser>()}, queryName_{args[0]}
{
	argParser_->addOption({SCOPE_ARGUMENT_NAMES, "Scope argument", SCOPE_ARGUMENT_NAMES[1]});
	argParser_->addOptions(options);

	// Since all our options require values we don't want -abc to be interpreted as -a -b -c but as --abc
	argParser_->setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	if (!argParser_->parse(args))
		throw QueryParsingException("Query arguments parsing error: " + argParser_->errorText());

	QString scope = argParser_->value(SCOPE_ARGUMENT_NAMES[0]);
	if (scope == "g") scope_ = Scope::Global;
	else if (scope == "of") scope_ = Scope::Input;
}

void ArgumentParser::setArgTo(QStringList& args, const QStringList& argNames, const QString& type)
{
	Q_ASSERT(argNames.size() > 0);
	bool set = false;
	for (auto& arg : args)
	{
		if (arg.size() > 1 && argNames.contains(arg.mid(1)))
		{
			arg.replace(QRegularExpression("=.*"), "=" + type);
			set = true;
		}
	}
	if (!set)
		args.append(QString("-%1=%2").arg(argNames[0], type));
}

QString ArgumentParser::argument(const QString& argName) const
{
	return argParser_->value(argName);
}

bool ArgumentParser::isArgumentSet(const QString& argName) const
{
	return argParser_->isSet(argName);
}

} /* namespace InformationScripting */
