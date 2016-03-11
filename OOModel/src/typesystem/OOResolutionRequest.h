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
#include "TypeArgumentBindings.h"

#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/util/ResolutionRequest.h"

namespace OOModel {

class SymbolMatcher;

class OOMODEL_API OOResolutionRequest : public Model::ResolutionRequest {

	public:
		OOResolutionRequest() = delete;

		OOResolutionRequest(QSet<Model::Node*>& result, const Model::SymbolMatcher& matcher, const Model::Node* source,
			Model::Node::FindSymbolDirection direction, Model::Node::SymbolTypes symbolTypes, bool exhaustAllScopes,
								  TypeArgumentBindings typeArgumentBindings = {});

		virtual ~OOResolutionRequest() override;

		virtual std::unique_ptr<ResolutionRequest> clone() const override;
		virtual std::unique_ptr<ResolutionRequest> clone(const Model::Node::FindSymbolDirection direction,
																		 const bool exhaustAllScopes) const override;
		virtual std::unique_ptr<ResolutionRequest> clone(const Model::Node::FindSymbolDirection direction) const override;
		virtual std::unique_ptr<ResolutionRequest> clone(const Model::Node* source,
																		 const Model::Node::FindSymbolDirection direction) const override;
		virtual std::unique_ptr<ResolutionRequest> clone(const Model::Node* source,
																		 const Model::Node::FindSymbolDirection direction,
																		 const bool exhaustAllScopes) const override;

		TypeArgumentBindings& typeArgumentBindings();
	private:
		TypeArgumentBindings typeArgumentBindings_;
};

inline TypeArgumentBindings& OOResolutionRequest::typeArgumentBindings() { return typeArgumentBindings_; }

inline OOResolutionRequest::OOResolutionRequest(QSet<Model::Node*>& result, const Model::SymbolMatcher& matcher,
																const Model::Node* source, Model::Node::FindSymbolDirection direction,
																Model::Node::SymbolTypes symbolTypes, bool exhaustAllScopes,
																TypeArgumentBindings typeArgumentBindings)
	: ResolutionRequest{result, matcher, source, direction, symbolTypes, exhaustAllScopes},
	  typeArgumentBindings_{typeArgumentBindings} {}

}
