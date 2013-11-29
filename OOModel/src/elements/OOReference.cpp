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
#include "../expressions/types/ClassTypeExpression.h"
#include "../expressions/types/ArrayTypeExpression.h"
#include "../expressions/MethodCallExpression.h"
#include "../expressions/AssignmentExpression.h"
#include "../expressions/CastExpression.h"

#include "../types/SymbolProviderType.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::OOReference)

namespace OOModel {

NODE_DEFINE_EMPTY_CONSTRUCTORS(OOReference)
NODE_DEFINE_TYPE_REGISTRATION_METHODS(OOReference)

bool OOReference::resolve()
{
	// TODO Handle the case where the symbol is defined multiple times in a better way

	// TODO this is not multithread friendly.
	if (resolving_) return false;
	resolving_ = true;

	auto parent = static_cast<ReferenceExpression*>(this->parent());

	SymbolTypes searchForType = ANY_SYMBOL;
	if ( referenceTargetKind() == ReferenceTargetKind::Container ) searchForType &= ~METHOD;

	QSet<Node*> candidateTargets;
	if (parent->prefix())
	{
		// Perform a downward search starting from the target of the prefix
		auto t = parent->prefix()->type();
		if (auto sp = dynamic_cast<SymbolProviderType*>(t))
		{
			if (sp->symbolProvider())
			{
				// It's important below that we change the source to sp->symbolProvider() in the call to findSymbols.
				// See NameImport.cpp for more info.

				sp->symbolProvider()->findSymbols(candidateTargets, name(), sp->symbolProvider(), SEARCH_DOWN,
						searchForType, false);
			}
		}
		SAFE_DELETE(t);
	}
	else
	{
		// Perform an upward search starting from the current node
		findSymbols(candidateTargets, name(), this, SEARCH_UP,  searchForType, false);
	}

	setTarget( resolveAmbiguity(candidateTargets) );

	resolving_ = false;
	return isResolved();
}

OOReference::ReferenceTargetKind OOReference::referenceTargetKind()
{
	auto parentRefExpr = static_cast<ReferenceExpression*>(this->parent());
	auto construct = parentRefExpr->parent();

	if (auto refExpr = DCast<ReferenceExpression>(construct))
	{
		// If this reference appears before a '.' operator, it must be a container
		if (parentRefExpr == refExpr->prefix()) return ReferenceTargetKind::Container;
	}
	else if (auto vd = DCast<VariableDeclaration>(construct))
	{
		if (parentRefExpr == vd->typeExpression()) return ReferenceTargetKind::Container;
	}
	else if (auto fr = DCast<FormalResult>(construct))
	{
		if (parentRefExpr == fr->typeExpression()) return ReferenceTargetKind::Container;
	}
	else if (auto fa = DCast<FormalArgument>(construct))
	{
		if (parentRefExpr == fa->typeExpression()) return ReferenceTargetKind::Container;
	}
	else if (auto ate = DCast<ArrayTypeExpression>(construct))
	{
		if (parentRefExpr == ate->typeExpression()) return ReferenceTargetKind::Container;
	}
	else if (auto ce = DCast<CastExpression>(construct))
	{
		if (parentRefExpr == ce->castType()) return ReferenceTargetKind::Container;
	}
	else if (auto cte = DCast<ClassTypeExpression>(construct))
	{
		if (parentRefExpr == cte->typeExpression()) return ReferenceTargetKind::Container;
	}
	else if (auto mce = DCast<MethodCallExpression>(construct))
	{
		if (parentRefExpr == mce->callee()) return ReferenceTargetKind::Callable;
	}
	else if (auto ae = DCast<AssignmentExpression>(construct))
	{
		if (parentRefExpr == ae->left()) return ReferenceTargetKind::Assignable;
	}
	else if (auto el = DCast<Model::TypedList<Expression>>(construct))
	{
		if (auto cl = DCast<Class>(el->parent()))
		{
			if (el == cl->baseClasses()) return ReferenceTargetKind::Container;
		}
		else if (auto refExpr = DCast<ReferenceExpression>(el->parent()))
		{
			if (el == refExpr->typeArguments()) return ReferenceTargetKind::Container;
		}
	}

	return ReferenceTargetKind::Unknown;
}

Model::Node* OOReference::resolveAmbiguity(QSet<Model::Node*>& candidates)
{
	if ( candidates.isEmpty() ) return nullptr;
	if ( candidates.size() == 1) return *candidates.begin(); // TODO: possibly check this for compliance?

	//TODO: Resolve ambiguity
	return nullptr;
}

} /* namespace OOModel */
