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

#include "ArgumentRule.h"

#include "ArgumentParser.h"

namespace InformationScripting {

ArgumentRule::ArgumentRule(ArgumentCheck checkFunction, QString message, std::vector<ArgumentValue> expectedArguments)
	: violationMessage{message}
{
	check = std::bind(checkFunction, std::placeholders::_1, expectedArguments);
}

bool ArgumentRule::requireAll(const ArgumentParser& parser, const std::vector<ArgumentValue>& values)
{
	return std::all_of(values.begin(), values.end(), ArgumentValueCheck{parser});
}

bool ArgumentRule::requireOneOf(const ArgumentParser& parser, const std::vector<ArgumentValue>& values)
{
	return std::any_of(values.begin(), values.end(), ArgumentValueCheck{parser});
}

bool ArgumentRule::atMostOneOf(const ArgumentParser& parser, const std::vector<ArgumentValue>& values)
{
	return std::count_if(values.begin(), values.end(), ArgumentValueCheck{parser}) <= 1;
}

ArgumentRule::ArgumentValueCheck::ArgumentValueCheck(const ArgumentParser& parser) : parser_{parser} {}

bool ArgumentRule::ArgumentValueCheck::operator()(const ArgumentValue& value) const
{
	using ValuePolicy = ArgumentValue::ValuePolicy;
	return (value.policy == ValuePolicy::NotEmpty && parser_.isArgumentSet(value.name)
			  && !parser_.argument(value.name).isEmpty())
			|| (value.policy == ValuePolicy::NotEquals && parser_.isArgumentSet(value.name)
				 && parser_.argument(value.name) != value.value);
}

} /* namespace InformationScripting */
