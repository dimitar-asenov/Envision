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

#include "../contractslibrary_api.h"

#if defined(ContractsLibrary_EXPORTS)
	#include "ModelBase/src/visitor/Visitor.hpp"
#else
	#include "ModelBase/src/visitor/Visitor.h"
#endif

namespace Model {
	class Node;
}

namespace OOModel {
	class MethodCallExpression;
	class ReferenceExpression;
	class Method;
}

namespace ContractsLibrary {

class CONTRACTSLIBRARY_API ValueAtReturnVisitor : public Model::Visitor<ValueAtReturnVisitor, Model::Node*> {
	public:
		ValueAtReturnVisitor();

		virtual Model::Node* visitChildren(Model::Node* n);

		static Model::Node* visitMethodCall(ValueAtReturnVisitor* v, OOModel::MethodCallExpression* call);
		static Model::Node* visitReference(ValueAtReturnVisitor* v, OOModel::ReferenceExpression* ref);

		static void init();
		static void setMethods(OOModel::Method* ensuresMethod, OOModel::Method* valueAtReturnMethod);

		int numWrapped();
		int numUnwrapped();

	private:
		bool inEnsuresCall_;
		bool inValueAtReturnCall_;
		bool outReference_;

		int numWrapped_;
		int numUnwrapped_;

		static OOModel::Method* ensuresMethod_;
		static OOModel::Method* valueAtReturnMethod_;

};

inline int ValueAtReturnVisitor::numWrapped() { return numWrapped_;}
inline int ValueAtReturnVisitor::numUnwrapped() { return numUnwrapped_;}
}
