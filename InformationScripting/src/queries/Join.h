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

#include "LinearQuery.h"
#include "../query_framework/ArgumentParser.h"

namespace InformationScripting {

class INFORMATIONSCRIPTING_API Join : public LinearQuery
{
	public:
		virtual Optional<TupleSet> executeLinear(TupleSet input) override;

		static void registerDefaultQueries();

	private:
		friend class QueryRegistry;

		static const QStringList VALUE_ARGUMENT_NAMES;
		static const QStringList AS_ARGUMENT_NAMES;
		static const QStringList ON_ARGUMENT_NAMES;

		ArgumentParser arguments_;

		Join(Model::Node* target, QStringList args, std::vector<ArgumentRule> argumentRules);

		Optional<QList<NamedProperty>> extractProperties(const Tuple& t,
																		 const QList<std::pair<QString, QString>>& values);

		using TaggedValue = std::pair<QString, QString>;
};

}
