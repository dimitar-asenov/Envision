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

#pragma once

#include "../informationscripting_api.h"

#include "../InformationScriptingException.h"
#include "ArgumentRule.h"

namespace Model {
	class Node;
}

class QCommandLineParser;
class QCommandLineOption;

namespace InformationScripting {

class INFORMATIONSCRIPTING_API ArgumentParser
{
	public:
		enum class Scope : int {Local, Global, Input};

		ArgumentParser(std::initializer_list<QCommandLineOption> options,
								  const QStringList& args);
		Scope scope() const;

		QString argument(const QString& argName) const;
		bool isArgumentSet(const QString& argName) const;

		QString queryName() const;

	private:
		std::unique_ptr<QCommandLineParser> argParser_{};
		Scope scope_{};
		QString queryName_;
		static const QStringList SCOPE_ARGUMENT_NAMES;
};

inline ArgumentParser::Scope ArgumentParser::scope() const { return scope_; }
inline QString ArgumentParser::queryName() const { return queryName_; }

} /* namespace InformationScripting */