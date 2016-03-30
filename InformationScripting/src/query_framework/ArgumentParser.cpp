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
#include "../queries/Query.h"

namespace InformationScripting {

const QStringList ArgumentParser::LOCAL_SCOPE_ARGUMENT_NAMES{"l", "local"};
const QStringList ArgumentParser::GLOBAL_SCOPE_ARGUMENT_NAMES{"g", "global"};
const QStringList ArgumentParser::INPUT_SCOPE_ARGUMENT_NAMES{"i", "input"};

ArgumentParser::ArgumentParser(std::initializer_list<QCommandLineOption> options,
													  const QStringList& args, bool addScopeArguments)
	: argParser_{std::make_unique<QCommandLineParser>()}, queryName_{args[0]}
{
	if (!argParser_->addOptions(options))
		Q_ASSERT(false);
	initParser(args, addScopeArguments);
}

ArgumentParser::ArgumentParser(std::initializer_list<PositionalArgument> options,
										 const QStringList& args, bool addScopeArguments)
	: argParser_{std::make_unique<QCommandLineParser>()}, queryName_{args[0]}
{
	for (const auto& opt : options)
		argParser_->addPositionalArgument(opt.name_, opt.description_, opt.syntax_);

	initParser(args, addScopeArguments);
}

ArgumentParser::ArgumentParser(std::initializer_list<QCommandLineOption> options,
										 std::initializer_list<PositionalArgument> positionalArgs,
										 const QStringList& args, bool addScopeArguments)
	: argParser_{std::make_unique<QCommandLineParser>()}, queryName_{args[0]}
{
	if (!argParser_->addOptions(options))
		Q_ASSERT(false);
	for (const auto& opt : positionalArgs)
		argParser_->addPositionalArgument(opt.name_, opt.description_, opt.syntax_);

	initParser(args, addScopeArguments);
}

void ArgumentParser::setArgTo(QStringList& args, const QStringList& argNames, const QString& type)
{
	Q_ASSERT(argNames.size() > 0);
	bool set = false;
	for (auto& arg : args)
	{
		if (arg.size() > 1 && argNames.contains(arg.mid(1)))
		{
			arg.replace(QRegularExpression{"=.*"}, "=" + type);
			set = true;
		}
	}
	if (!set)
		args.append(QString{"-%1=%2"}.arg(argNames[0], type));
}

ArgumentParser::Scope ArgumentParser::scope(const Query* of) const
{
	Q_ASSERT(of);
	// Manually defined scope argument is always respected:
	if (scope_ != Scope::Undefined) return scope_;

	return of->hasInput() ? Scope::Input : Scope::Local;
}

QString ArgumentParser::argument(const QString& argName) const
{
	return argParser_->value(argName);
}

bool ArgumentParser::isArgumentSet(const QString& argName) const
{
	return argParser_->isSet(argName);
}

int ArgumentParser::numPositionalArguments() const
{
	return argParser_->positionalArguments().size();
}

QString ArgumentParser::positionalArgument(int index) const
{
	auto posArgs = argParser_->positionalArguments();
	Q_ASSERT(index >= 0 && index < posArgs.size());
	return posArgs[index];
}

void ArgumentParser::initParser(const QStringList& args, bool addScopeArguments)
{
	if (addScopeArguments)
	{
		if (!argParser_->addOption(QCommandLineOption{LOCAL_SCOPE_ARGUMENT_NAMES}))
			Q_ASSERT(false);
		if (!argParser_->addOption(QCommandLineOption{GLOBAL_SCOPE_ARGUMENT_NAMES}))
			Q_ASSERT(false);
		if (!argParser_->addOption(QCommandLineOption{INPUT_SCOPE_ARGUMENT_NAMES}))
			Q_ASSERT(false);
	}

	// Since all our options require values we don't want -abc to be interpreted as -a -b -c but as --abc
	argParser_->setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	if (!argParser_->parse(args))
		throw QueryParsingException{"Query arguments parsing error: " + argParser_->errorText()};

	if (addScopeArguments)
	{
		if (argParser_->isSet(LOCAL_SCOPE_ARGUMENT_NAMES[0])) scope_ = Scope::Local;
		else if (argParser_->isSet(GLOBAL_SCOPE_ARGUMENT_NAMES[0])) scope_ = Scope::Global;
		else if (argParser_->isSet(INPUT_SCOPE_ARGUMENT_NAMES[0])) scope_ = Scope::Input;
	}
}

}
