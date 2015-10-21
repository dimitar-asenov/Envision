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

namespace InformationScripting {

class ArgumentParser;

struct INFORMATIONSCRIPTING_API ArgumentValue {
	enum ValuePolicy { NotEmpty, NotEquals };
	QString name;
	ValuePolicy policy{NotEmpty};
	QString value{};
};

class INFORMATIONSCRIPTING_API ArgumentRule {
	public:
		using ArgumentCheck = std::function<bool (const ArgumentParser&, const std::vector<ArgumentValue>&)>;
		std::function<bool (const ArgumentParser&)> check;
		QString violationMessage;
		ArgumentRule(ArgumentCheck checkFunction, QString message, std::vector<ArgumentValue> expectedArguments);

		static bool requireAll(const ArgumentParser& parser, const std::vector<ArgumentValue>& values);
		static bool requireOneOf(const ArgumentParser& parser, const std::vector<ArgumentValue>& values);
		static bool atMostOneOf(const ArgumentParser& parser, const std::vector<ArgumentValue>& values);
	private:
		// The argument value check used in the rule functions
		class ArgumentValueCheck {
			public:
				ArgumentValueCheck(const ArgumentParser& parser);
				bool operator() (const ArgumentValue& value) const;
			private:
				const ArgumentParser& parser_;
		};
};

} /* namespace InformationScripting */
