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

#pragma once

#include "Type.h"

namespace OOModel {
class LambdaExpression;

class OOMODEL_API FunctionType : public Type {
	public:
		FunctionType(bool isValueType, std::vector<std::unique_ptr<Type>> arguments, std::unique_ptr<Type> result = {});
		FunctionType(bool isValueType, std::vector<std::unique_ptr<Type>> arguments,
						 std::vector<std::unique_ptr<Type>> results);

		FunctionType(const FunctionType& other);

		virtual bool equals(const Type* other) const override;
		virtual FunctionType* clone() const override;

		const std::vector<std::unique_ptr<Type>>& arguments() const;
		const std::vector<std::unique_ptr<Type>>& results() const;
		const Type* result() const;

	private:
		std::vector<std::unique_ptr<Type>> arguments_;
		std::vector<std::unique_ptr<Type>> results_;
};

inline const std::vector<std::unique_ptr<Type>>& FunctionType::arguments() const { return arguments_; }
inline const std::vector<std::unique_ptr<Type> >& FunctionType::results() const { return results_; }
inline const Type* FunctionType::result() const
{Q_ASSERT(results_.size() <= 1); return results_.empty() ? nullptr : results_.front().get();}

}
