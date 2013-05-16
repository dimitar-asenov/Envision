/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "OOReference.h"
#include "../expressions/ReferenceExpression.h"
#include "../declarations/Class.h"

#include "../types/SymbolProviderType.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::OOReference)

namespace OOModel {

NODE_DEFINE_EMPTY_CONSTRUCTORS(OOReference)
NODE_DEFINE_TYPE_REGISTRATION_METHODS(OOReference)

bool OOReference::resolve()
{
	// TODO Handle the case where the symbol is defined multiple times in a better way

	auto parent = static_cast<ReferenceExpression*>(this->parent());

	Model::Node* symbol = nullptr;

	if (parent->prefix())
	{
		// Perform a downward search starting from the target of the prefix
		auto t = parent->prefix()->type();
		if (auto sp = dynamic_cast<SymbolProviderType*>(t))
		{
			if (sp->symbolProvider())
			{
				auto symbolList = sp->symbolProvider()->findSymbols( name(), this, SEARCH_DOWN, false);
				if (symbolList.size() == 1) symbol = symbolList.first();
			}
		}
		SAFE_DELETE(t);
	}
	else
	{
		// Perform an upward search starting from the current node
		auto symbolList = findSymbols(name(), this, SEARCH_UP, false);
		if (symbolList.size() == 1) symbol = symbolList.first();
	}

	if (target() != symbol) setTarget(symbol);
	return isResolved();
}

} /* namespace OOModel */
