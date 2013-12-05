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
#include "../declarations/Method.h"
#include "../expressions/types/ClassTypeExpression.h"
#include "../expressions/types/ArrayTypeExpression.h"
#include "../expressions/MethodCallExpression.h"
#include "../expressions/AssignmentExpression.h"
#include "../expressions/CastExpression.h"

#include "../types/SymbolProviderType.h"
#include "../types/ErrorType.h"
#include "../typesystem/TypeSystem.h"

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
	if ( referenceTargetKind() != ReferenceTargetKind::Callable ) searchForType &= ~METHOD;

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
						searchForType, searchForType.testFlag(METHOD) ); 	// When search for methods do an exhaustive search.
																							// This is important for overloads.
			}
		}
		SAFE_DELETE(t);
	}
	else
	{
		// Perform an upward search starting from the current node
		// When search for methods do an exhaustive search. This is important for overloads.
		findSymbols(candidateTargets, name(), this, SEARCH_UP,  searchForType, searchForType.testFlag(METHOD));
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

	auto parentRefExpr = static_cast<ReferenceExpression*>(this->parent());
	auto construct = parentRefExpr->parent();
	if (auto mce = DCast<MethodCallExpression>(construct))
	{
		auto methodToCall = resolveAmbiguousMethodCall(candidates, mce);
		if (methodToCall) return methodToCall;
	}

	//TODO: Resolve additional ambiguities
	return nullptr;
}

Model::Node* OOReference::resolveAmbiguousMethodCall(QSet<Model::Node*>& candidates,
		MethodCallExpression* callExpression)
{
	// TODO: So far this implements only the simpler cases of Java. Complex cases or other languages need to be
	// considered.

	QSet<Method*> filtered;


	// Exclude all non-methods
	for(auto target : candidates)
		if (auto method = DCast<Method>(target)) filtered.insert(method);
	if (filtered.size() == 1) return *filtered.begin();
	if (filtered.isEmpty()) return nullptr;

	auto callee = static_cast<ReferenceExpression*>(callExpression->callee());

	// Exclude methods with less type arguments or unequal number of formal arguments
	// TODO: Consider default method arguments and variable method arity.
	auto it = filtered.begin();
	while(it != filtered.end())
	{
		if ((*it)->typeArguments()->size() < callee->typeArguments()->size()
				|| callExpression->arguments()->size() != (*it)->arguments()->size())
			it = filtered.erase(it);
		else ++ it;
	}
	if (filtered.size() == 1) return *filtered.begin();
	if (filtered.isEmpty()) return nullptr;



	// Filter non-matching arguments
	int argId = 0;
	for(auto arg: *callExpression->arguments())
	{
		auto actualArgType = arg->type();

		auto it = filtered.begin();
		while(it != filtered.end())
		{
			auto formalArgType = (*it)->arguments()->at(argId)->typeExpression()->type();

			auto typeRelation = actualArgType->relationTo(formalArgType);
			if ( typeRelation.testFlag(TypeSystem::Equal) || typeRelation.testFlag(TypeSystem::IsSubtype)
					|| typeRelation.testFlag(TypeSystem::IsConvertibleTo))
				++it;
			else it = filtered.erase(it);

			SAFE_DELETE(formalArgType);
		}

		SAFE_DELETE(actualArgType);
		++argId;
	}
	if (filtered.size() == 1) return *filtered.begin();
	if (filtered.isEmpty()) return nullptr;


	// Remove methods which are overriden by another candidate
	QSet<Method*> nonOverriden;
	while (!filtered.isEmpty())
	{
		auto m = *filtered.begin();
		filtered.remove(m);

		bool overriden = false;
		auto it = filtered.begin();
		while (it != filtered.end())
		{
			if ((*it)->overrides(m))
			{
				overriden = true;
				break;
			}

			if (m->overrides((*it))) it = filtered.erase(it);
			else ++it;
		}

		if (!overriden) nonOverriden.insert(m);
	}
	filtered = nonOverriden;
	if (filtered.size() == 1) return *filtered.begin();
	if (filtered.isEmpty()) return nullptr;


	// Choose a most specific method
	QSet<Method*> mostSpecific;
	while(!filtered.isEmpty())
	{
		enum Specificity {UNDETERMINED, MORE, LESS, SAME};
		Specificity overallSpecificity = UNDETERMINED;
		auto m = *filtered.begin();
		filtered.remove(m);

		QList<Type*> filteredTypes;
		for (auto te : *m->arguments()) filteredTypes.append(te->typeExpression()->type());

		// Compare this method to all the ones that are already most specifc.
		// Remove any most specific method that is strictly less specific than the current one.
		auto sIt = mostSpecific.begin();
		while(sIt != mostSpecific.end())
		{
			Specificity specificity = UNDETERMINED;
			for(int argId = 0; argId < filteredTypes.size(); ++argId)
			{
				auto spType = (*sIt)->arguments()->at(argId)->typeExpression()->type();
				auto relation = filteredTypes.at(argId)->relationTo(spType);

				if (relation.testFlag(TypeSystem::Equal)) /* Do nothing*/;
				else if (relation.testFlag(TypeSystem::IsSubtype) && specificity == UNDETERMINED) specificity = MORE;
				else if (relation.testFlag(TypeSystem::IsSubtype) && specificity == MORE) /* Do nothing*/;
				else if (relation.testFlag(TypeSystem::IsSubtype) && specificity == LESS) specificity = SAME;
				else if (relation.testFlag(TypeSystem::IsSupertype) && specificity == UNDETERMINED) specificity = LESS;
				else if (relation.testFlag(TypeSystem::IsSupertype) && specificity == LESS) /* Do nothing*/;
				else if (relation.testFlag(TypeSystem::IsSupertype) && specificity == MORE) specificity = SAME;

				SAFE_DELETE(spType);

				if (specificity == SAME) break;
			}

			if (specificity == MORE)
			{
				sIt = mostSpecific.erase(sIt);
				Q_ASSERT(overallSpecificity != LESS);
				if(overallSpecificity == UNDETERMINED) overallSpecificity = MORE;
			}
			else if (specificity == LESS)
			{
				Q_ASSERT(overallSpecificity != MORE);
				overallSpecificity = LESS;
				break;
			}
			else if (specificity == SAME || specificity == UNDETERMINED)
			{
				// UNDETERMINED means an identical or a completely unrelated signature
				++sIt;
				overallSpecificity = SAME;
			}
		}

		for(auto t : filteredTypes) SAFE_DELETE(t);

		Q_ASSERT(mostSpecific.isEmpty() || overallSpecificity != UNDETERMINED);
		if (overallSpecificity != LESS) mostSpecific.insert(m);
	}
	Q_ASSERT(!mostSpecific.isEmpty());
	if (mostSpecific.size() == 1) return *mostSpecific.begin();

	candidates.clear();
	for(auto m : mostSpecific) candidates.insert(m);
	//TODO: check for correct access of public/private

	return nullptr;
}

} /* namespace OOModel */
