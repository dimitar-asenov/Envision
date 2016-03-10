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

#include "../modelbase_api.h"
#include "../nodes/Node.h"

namespace Model {

class SymbolMatcher;

class MODELBASE_API ResolutionRequest {

	public:
		ResolutionRequest() = delete;

		/**
		 * Represents a request for finding all nodes which define a symbol with a name matching \a matcher in the
		 * scope of a node.
		 *
		 * The \a source Node specifies what node should be used as a reference when determining what symbols are visible.
		 *
		 * The \a direction specifies what search to perform.
		 *
		 * The \a symbolTypes specifies what symbol types should be returned. If a symbol matches any of the requested
		 * types it will be returned.
		 *
		 * If \a exhaustAllScopes is false, the search will halt as soon as symbols are found within a scope. This is
		 * useful when resolving links and it is important to find the "nearest" symbols that match with respect to scope.
		 * If \a exhaustAllScopes is true, the search will explore all scopes in corresponding to the provided mode. This
		 * is useful when \a symbolExp can match multiple symbols with different names (for example during auto completion
		 * list build up).
		 */
		ResolutionRequest(QSet<Node*>& result, const SymbolMatcher& matcher, const Node* source,
			Node::FindSymbolDirection direction, Node::SymbolTypes symbolTypes, bool exhaustAllScopes);

		virtual ~ResolutionRequest();

		virtual std::unique_ptr<ResolutionRequest> clone() const;
		virtual std::unique_ptr<ResolutionRequest> clone(const Node::FindSymbolDirection direction,
																		 const bool exhaustAllScopes) const;
		virtual std::unique_ptr<ResolutionRequest> clone(const Node::FindSymbolDirection direction) const;
		virtual std::unique_ptr<ResolutionRequest> clone(const Node* source,
																		 const Node::FindSymbolDirection direction) const;
		virtual std::unique_ptr<ResolutionRequest> clone(const Node* source,
																		 const Node::FindSymbolDirection direction,
																		 const bool exhaustAllScopes) const;

		QSet<Node*>& result() const;
		const SymbolMatcher& matcher() const;
		const Node* source() const;
		Node::FindSymbolDirection direction() const;
		Node::SymbolTypes symbolTypes() const;
		bool exhaustAllScopes() const;

	private:
		QSet<Node*>& result_;
		const SymbolMatcher& matcher_;
		const Node* source_;
		const Node::FindSymbolDirection direction_;
		const Node::SymbolTypes symbolTypes_;
		const bool exhaustAllScopes_;
};

inline QSet<Node*>& ResolutionRequest::result() const { return result_; }
inline const SymbolMatcher& ResolutionRequest::matcher() const { return matcher_; }
inline const Node* ResolutionRequest::source() const { return source_; }
inline Node::FindSymbolDirection ResolutionRequest::direction() const { return direction_; }
inline Node::SymbolTypes ResolutionRequest::symbolTypes() const { return symbolTypes_; }
inline bool ResolutionRequest::exhaustAllScopes() const { return exhaustAllScopes_; }

inline ResolutionRequest::ResolutionRequest(QSet<Node*>& result, const SymbolMatcher& matcher, const Node* source,
	Node::FindSymbolDirection direction, Node::SymbolTypes symbolTypes, bool exhaustAllScopes)
	:result_{result}, matcher_{matcher}, source_{source}, direction_{direction}, symbolTypes_{symbolTypes},
	  exhaustAllScopes_{exhaustAllScopes}{}

}
