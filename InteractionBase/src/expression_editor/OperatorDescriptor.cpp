/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "OperatorDescriptor.h"

namespace Interaction {

OperatorDescriptor::OperatorDescriptor() {
}

OperatorDescriptor::~OperatorDescriptor() {
}

OperatorDescriptor::OperatorDescriptor(const QString& prefixText, const QString& postfixText)
	: name_{"Error: " + prefixText + postfixText}, precedence_{0},
	  associativity_{prefixText.isEmpty() ? LeftAssociative : RightAssociative}, isError_{true}
{
	Q_ASSERT(prefixText.isEmpty() || postfixText.isEmpty());
	num_operands_ = 1;
	if (!prefixText.isEmpty())
	{
		signature_.append(prefixText);
		expectedTokens_ <<  ExpectedToken{ExpectedToken::FIRST_DELIM, prefixText};
		prefixTokens_.append(prefixText);
	}
	signature_.append("expr");
	expectedTokens_ <<  ExpectedToken{ExpectedToken::VALUE};
	if (!postfixText.isEmpty())
	{
		signature_.append(postfixText);
		expectedTokens_ <<  ExpectedToken{ExpectedToken::FIRST_DELIM, postfixText};
		postfixTokens_.append(postfixText);
	}
	expectedTokens_ << ExpectedToken{ExpectedToken::END};
}

OperatorDescriptor::OperatorDescriptor(const QString& name, const QString& signature, int precedence,
		Associativity associativity)
	: name_{name}, precedence_{precedence}, associativity_{associativity}
{
	signature_ = signature.split(" ", Qt::SkipEmptyParts);
	signature_.replaceInStrings("SPACE", " ");

	QList<QStringList> preInPostFixTokens;
	preInPostFixTokens.append( QStringList{} ); // Add an empty prefix list to begin with

	auto addNonDelimiter = [this, &preInPostFixTokens] (ExpectedToken::ExpectedType type)
	{
	  expectedTokens_ <<  ExpectedToken{type};
	  ++num_operands_;
	  preInPostFixTokens.append( QStringList{} ); // Begin a new infix/postfix
	};

	int sigIndex = 0;
	bool lastDelimiter = true;
	while (sigIndex < signature_.length())
	{
		auto s = signature_[sigIndex];

		bool delimiter = false;
		if ( s == "id" ) addNonDelimiter(ExpectedToken::ID);
		else if ( s == "type" ) addNonDelimiter(ExpectedToken::TYPE);
		else if ( s == "expr" ) addNonDelimiter(ExpectedToken::VALUE);
		else if ( s == "typeOrExpr" ) addNonDelimiter(ExpectedToken::ANY);
		else
		{
			// This is a delimiter, but what kind?
			if (expectedTokens_.isEmpty() ||
				 (expectedTokens_.last().type != ExpectedToken::FIRST_DELIM
					&& expectedTokens_.last().type != ExpectedToken::FOLLOWING_DELIM))
			{
				expectedTokens_ << ExpectedToken{ExpectedToken::FIRST_DELIM, s};
			}
			else expectedTokens_ << ExpectedToken{ExpectedToken::FOLLOWING_DELIM, s};
			QStringList& last = preInPostFixTokens.last();
			last.append(s);

			delimiter = true;
		}

		if (!lastDelimiter && !delimiter)
		{
			//There are two consecutive non-delimiter tokens. Insert an empty signature entry.
			signature_.insert(sigIndex, "");
			++sigIndex; // Jump over the signature part that we just inserted.
		}

		++sigIndex;
		lastDelimiter = delimiter;
	}
	expectedTokens_ << ExpectedToken{ExpectedToken::END};

	prefixTokens_ = preInPostFixTokens.takeFirst();
	if (!preInPostFixTokens.isEmpty()) postfixTokens_ = preInPostFixTokens.takeLast();
	if (!preInPostFixTokens.isEmpty()) infixesTokens_ = preInPostFixTokens;
}

QStringList OperatorDescriptor::delimiters()
{
	QStringList list;
	list.append("");

	for (int i = 0; i< signature_.size(); ++i)
	{
		if ( !isDelimiter(signature_.at(i))) list.append("");
		else list.last().append( signature_.at(i) );
	}

	return list;
}

bool OperatorDescriptor::isDelimiter(int signatureIndex)
{
	Q_ASSERT(signatureIndex < signature_.size());
	if (isError_) return prefixTokens_.isEmpty() != (signatureIndex == 0);
	else return isDelimiter(signature_[signatureIndex]);
}

}
