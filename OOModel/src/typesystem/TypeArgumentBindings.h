/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#include "../oomodel_api.h"
#include "../types/Type.h"

namespace Model {
	class Node;
}

namespace OOModel {

class FormalTypeArgument;
class ReferenceExpression;

class OOMODEL_API TypeArgumentBindings {
	public:
		TypeArgumentBindings() = default;
		TypeArgumentBindings(const TypeArgumentBindings& other);
		TypeArgumentBindings(const TypeArgumentBindings&& other);

		void insert(FormalTypeArgument* argument, std::unique_ptr<Type> type);
		void insertFromReference(const ReferenceExpression* ref, Model::Node* genericNode);

		std::unique_ptr<Type> bindingFor(FormalTypeArgument* argument);

		bool operator == (const TypeArgumentBindings& other) const;

	private:

		/**
		 * This map is typically going to contain only very few elements, that's why we don't use unordered_map.
		 */
		std::map<FormalTypeArgument*, std::unique_ptr<Type>> bindings_;
};

inline bool TypeArgumentBindings::operator == (const TypeArgumentBindings& other) const
{ return bindings_ == other.bindings_; }

}
