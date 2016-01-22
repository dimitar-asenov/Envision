/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "Reference.h"
#include "../commands/FieldSet.h"
#include "../model/TreeManager.h"
#include "../ModelException.h"
#include "NameText.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
template class Model::TypedList<Model::Reference>;

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Reference)

QList<Reference*> Reference::allReferences_;
QSet<Reference*> Reference::pendingResolution_;
QList<std::function<void (Node* subTree)>> Reference::unresolutionSteps_;

Reference::Reference(Node *parent) : Super{parent}
{
	allReferences_.append(this);
	if (parent && parent->manager()) pendingResolution_.insert(this);
}

Reference::Reference(Node *parent, PersistentStore &store, bool) : Super{parent}
{
	allReferences_.append(this);
	name_ = store.loadReferenceValue(this);
	if (state_ == ReferenceNeedsToBeResolved && parent && parent->manager())
		pendingResolution_.insert(this);
}

Reference::Reference(const Reference& other) : Super{other}
{
	setName(other.name());
}

Reference* Reference::clone() const { return new Reference{*this}; }

Reference::~Reference()
{
	pendingResolution_.remove(this);
	allReferences_.removeAll(this);
}

void Reference::setName(const QString &name)
{
	if (name != name_)
	{
		execute(new FieldSet<QString> {this, name_, name});
		target_ = nullptr;
		setResolutionNeeded();
	}
}

Node* Reference::computeTarget() const
{
	Q_ASSERT(false && "Should not call Reference::computeTarget()");
	return nullptr;
}

bool Reference::resolveHelper(bool indirect)
{
	// TODO this is not multithread friendly.
	if (state_ != ReferenceNeedsToBeResolved) return isResolved();
	state_ = ReferenceIsBeingResolved;

	Node* newTarget = computeTarget();

	Q_ASSERT(!newTarget || (newTarget->definesSymbol() && newTarget->symbolName() == name_));

	auto oldTarget = target_;
	target_ = newTarget;

	if (target_ || !indirect)
	{
		// This is needed because of the following situation.
		// We are resolving a reference A, and during the process we explore reference B, needed for the resolution of A,
		// and reference C, which is not needed for the resolution of A. All references but A will be explored indirectly.
		// It could happen that to resolve C properly we need to resolve A first. Therefore when failing to resolve a
		// reference that is explored indirectly, we shouldn't assume that resolution will fail.
		pendingResolution_.remove(this);
		state_ = ReferenceEstablished;
	}
	else state_ = ReferenceNeedsToBeResolved;

	if (oldTarget != target_) targetChanged(oldTarget);

	return isResolved();
}

void Reference::targetChanged(Node*){}

void Reference::save(PersistentStore &store) const
{
	store.saveReferenceValue(name_, target_);
}

void Reference::load(PersistentStore &store)
{
	// TODO: Implement reference loading properly.
	throw ModelException("Loading references outside a Reference constructor is not properly implemented yet");

	if (store.currentNodeType() != typeName())
		throw ModelException("Trying to load a Reference node from an incompatible node type " + store.currentNodeType());

	setName( store.loadReferenceValue(this) );
}

Node* Reference::target()
{
	if (state_ == ReferenceNeedsToBeResolved) resolveHelper(true);
	return state_ == ReferenceEstablished ? target_ : nullptr;
}

void Reference::setResolutionNeeded()
{
	state_ = ReferenceNeedsToBeResolved;
	pendingResolution_.insert(this);
}

void Reference::unresolveReferencesHelper(Node* subTree, bool all,
	const QSet<QString>& names)
{
	if (!subTree || (!all && names.isEmpty())) return;

	auto targetManager = subTree->manager();
	bool isRoot = !subTree->parent();

	std::function<void (Reference*)> f;
	if (all)
		f = [](Reference* ref) { ref->setResolutionNeeded();};
	else if (names.size() == 2)
	{
		//This case occurs when changing a name. This happens when the user is typing and therefore it is very
		//performance sensitive. Thus we optimize it specially.

		QString first = *names.begin();
		QString second = *++names.begin();

		// Removing this if, and just leaving the statement at the of this method to handle the updates using the
		// defined f function is slow. In one test an additional 4-5ms were needed on top of the 10-13ms processing
		// time on a 3.4GHz CPU. Therefore there is a significant cost associated with calling the f function, and
		// it's betterto just process this case here directly.
		// If needed this optimization could also be used in the other cases.
		if (isRoot)
		{
			for (auto ref : allReferences_)
				if (targetManager == ref->manager())
				{
					auto& refName = ref->name();
					if (refName == first || refName == second)
						ref->setResolutionNeeded();
				}
			return;
		}

		f = [=](Reference* ref)
			{ auto& refName = ref->name(); if (refName == first || refName == second) ref->setResolutionNeeded();};
	}
	else if (names.size() < 20)
	{
		// Optimize in the case of just a few names, by avoiding hasing.

		// TODO: Find out what's a good value for the magick number 20 above.
		QMap<QString, int> mapNames;
		for (auto n : names)
		{
			// TODO: isn't there a way to put void here?
			mapNames.insert(n, 0);
		}

		f = [=](Reference* ref) { if (mapNames.contains(ref->name())) ref->setResolutionNeeded();};
	}
	else
		f = [=](Reference* ref) { if (names.contains(ref->name())) ref->setResolutionNeeded();};

	if (isRoot)
	{
		for (auto ref : allReferences_) if (targetManager == ref->manager()) f(ref);
	}
	else
	{
		// If this is not the root, then explore the references only in the specified subTree, which is faster
		forAll<Reference>(subTree, f);
	}
}

void Reference::unresolveAll(Node* subTree)
{
	unresolveReferencesHelper(subTree, true, {});
}

void Reference::unresolveNames(Node* subTree, const QSet<QString>& names)
{
	unresolveReferencesHelper(subTree, false, names);
}

void Reference::unresolveIfNameIntroduced(Node* subTreeToUnresolve,
		Node* subTreeToLookForNewNames)
{
	QSet<QString> names;
	forAll<NameText>(subTreeToLookForNewNames, [&names](NameText* name){ names.insert(name->get());});
	unresolveReferencesHelper(subTreeToUnresolve, false, names);
}

void Reference::unresolveAfterNewSubTree(Node* subTree)
{
	Reference::unresolveAll(subTree);
	Reference::unresolveIfNameIntroduced(subTree->root(), subTree);
	for (auto step : unresolutionSteps_) step(subTree);
}

void Reference::addUnresolutionSteps(std::function<void (Node* subTree)> step)
{
	unresolutionSteps_.append(step);
}

void Reference::resolvePending()
{
	log.debug("Resolving references");

	int i = 0;
	int round = 0;
	int resolved = 0;
	auto pending = pendingResolution_;

	while (!pending.isEmpty())
	{
		log.debug("resolution round " + QString::number(round) + ", "
					 + QString::number(pendingResolution_.size()) + " references pending.");
		++round;

		for (auto r : pending)
		{
			if ( r->resolve() ) ++resolved;
			if (++i % 10000 == 0)	log.debug("Processed " + QString::number(i) + " references.");
		}

		// As a result of resolution of some references, some other references could have become invalid.
		pending = pendingResolution_;
	}

	log.debug(QString::number(resolved) + " resolution" + (resolved == 1 ? "" : "s") + " in "
				 + QString::number(round) + (round > 1 ? " rounds." : " round."));

	int totalResolvedReferences = 0;
	for (auto r : allReferences_) if (r->isResolved()) ++totalResolvedReferences;
	log.debug("Total number of resolved references: " + QString::number(totalResolvedReferences));
}

}
