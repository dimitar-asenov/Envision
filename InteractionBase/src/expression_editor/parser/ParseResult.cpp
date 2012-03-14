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
 * ParseResult.cpp
 *
 *  Created on: Jan 11, 2012
 *      Author: Dimitar Asenov
 */

#include "expression_editor/parser/ParseResult.h"

namespace Interaction {

ParseResult::ParseResult()
	:errors(0), emptyExpressions(0), missing_inner_tokens(0), missing_trailing_tokens(0)
{
}

ParseResult::ParseResult(int errors, int missing_inner_tokens, int missing_trailing_tokens)
	: errors(errors), missing_inner_tokens(missing_inner_tokens), missing_trailing_tokens(missing_trailing_tokens)
{
}

bool operator< (const ParseResult& left, const ParseResult& right)
{
	if ( left.errors != right.errors ) return  left.errors < right.errors;
	if ( left.missing_inner_tokens != right.missing_inner_tokens ) return  left.missing_inner_tokens < right.missing_inner_tokens;
	if ( left.missing_trailing_tokens != right.missing_trailing_tokens ) return  left.missing_trailing_tokens < right.missing_trailing_tokens;
	if ( left.emptyExpressions != right.emptyExpressions ) return  left.emptyExpressions < right.emptyExpressions;

	return false;
}

} /* namespace InteractionBase */
