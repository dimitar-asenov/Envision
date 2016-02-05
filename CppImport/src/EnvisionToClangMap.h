/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "cppimport_api.h"

namespace Model {
	class Node;
}

namespace CppImport {

/**
 * logically holds the mapping from Envision AST nodes to Clang AST nodes.
 * in practice finds and stores the relevant source range of the Clang AST nodes since that is all information we need.
 */
class CPPIMPORT_API EnvisionToClangMap
{
	public:
		QMultiHash<Model::Node*, clang::SourceRange>::iterator begin();
		QMultiHash<Model::Node*, clang::SourceRange>::iterator end();

		void mapAst(clang::SourceRange sourceRange, Model::Node* envisionAstNode);

		const QSet<Model::Node*> nodes() const;
		QList<clang::SourceRange> get(Model::Node* node) const;
		bool contains(Model::Node* node) const;
		void remove(Model::Node* node);
		void clear();

		Model::Node* closestParentWithAstMapping(Model::Node* node) const;
	private:
		QMultiHash<Model::Node*, clang::SourceRange> envisionToClangMap_;
};

inline QMultiHash<Model::Node*, clang::SourceRange>::iterator EnvisionToClangMap::begin()
{ return envisionToClangMap_.begin(); }

inline QMultiHash<Model::Node*, clang::SourceRange>::iterator EnvisionToClangMap::end()
{ return envisionToClangMap_.end(); }

inline const QSet<Model::Node*> EnvisionToClangMap::nodes() const { return envisionToClangMap_.keys().toSet(); }

inline void EnvisionToClangMap::clear() { envisionToClangMap_.clear(); }

inline bool EnvisionToClangMap::contains(Model::Node* node) const { return envisionToClangMap_.contains(node); }

inline void EnvisionToClangMap::remove(Model::Node* node) { envisionToClangMap_.remove(node); }

}
