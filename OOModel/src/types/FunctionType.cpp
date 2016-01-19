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

#include "FunctionType.h"

namespace OOModel {

FunctionType::FunctionType(bool isValueType, QList<const Type*> arguments, QList<const Type*> results)
: Type{isValueType}, arguments_{arguments}, results_{results}
{}

FunctionType::FunctionType(bool isValueType, QList<const Type*> arguments, Type* result)
: Type{isValueType}, arguments_{arguments}
{
	if (result) results_.append(result);
}

FunctionType::FunctionType(const FunctionType& other)
: Type{other}
{
	for (auto a : other.arguments_) arguments_.append(a->clone());
	for (auto r : other.results_) results_.append(r->clone());
}

FunctionType::~FunctionType()
{
	for (auto a : arguments_) SAFE_DELETE( a);
	for (auto r : results_) SAFE_DELETE(r);
}

bool FunctionType::equals(const Type* other) const
{
	auto ft = dynamic_cast<const FunctionType*> (other);
	if (!ft) return false;

	if (arguments_.size() != ft->arguments_.size()) return false;
	if (results_.size() != ft->results_.size()) return false;

	for (int i = 0; i<arguments_.size(); ++i)
		if (! arguments_.at(i)->equals(ft->arguments_.at(i))) return false;

	for (int i = 0; i<results_.size(); ++i)
		if (! results_.at(i)->equals(ft->results_.at(i))) return false;

	return true;
}

FunctionType* FunctionType::clone() const
{
	return new FunctionType{*this};
}

}
