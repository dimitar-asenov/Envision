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

FunctionType::FunctionType(bool isValueType, std::vector<std::unique_ptr<Type> > arguments,
									std::vector<std::unique_ptr<Type> > results)
	: Type{isValueType}, arguments_{std::move(arguments)}, results_{std::move(results)}
{}

FunctionType::FunctionType(bool isValueType, std::vector<std::unique_ptr<Type> > arguments,
									std::unique_ptr<Type> result)
	: Type{isValueType}, arguments_{std::move(arguments)}
{
	if (result) results_.push_back(std::move(result));
}

FunctionType::FunctionType(const FunctionType& other)
: Type{other}
{
	for (auto & a : other.arguments_) arguments_.emplace_back(a->clone());
	for (auto & r : other.results_) results_.emplace_back(r->clone());
}

bool FunctionType::equals(const Type* other) const
{
	auto ft = dynamic_cast<const FunctionType*> (other);
	if (!ft) return false;

	if (arguments_.size() != ft->arguments_.size()) return false;
	if (results_.size() != ft->results_.size()) return false;

	for (size_t i = 0; i<arguments_.size(); ++i)
		if (! arguments_[i]->equals(ft->arguments_[i].get())) return false;

	for (size_t i = 0; i<results_.size(); ++i)
		if (! results_[i]->equals(ft->results_[i].get())) return false;

	return true;
}

FunctionType* FunctionType::clone() const
{
	return new FunctionType{*this};
}

}
