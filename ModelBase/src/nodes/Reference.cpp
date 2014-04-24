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

#include "nodes/Reference.h"
#include "commands/FieldSet.h"
#include "model/Model.h"
#include "ModelException.h"
#include "NameText.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(Model::Reference)

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Reference)

QSet<Reference*> Reference::pendingResolution_;

Reference::Reference(Node *parent) : Super(parent)
{
	if (parent && parent->model()) pendingResolution_.insert(this);
}

Reference::Reference(Node *parent, PersistentStore &store, bool) : Super(parent)
{
	name_ = store.loadReferenceValue(this);
	if (state_ == ReferenceNeedsToBeResolved && parent && parent->model())
		pendingResolution_.insert(this);
}

Reference::~Reference()
{
	pendingResolution_.remove(this);
}

void Reference::setName(const QString &name)
{
	if (name != name_)
	{
		execute(new FieldSet<QString> (this, name_, name));
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

	auto newTarget = computeTarget();

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

template<class NodeType>
void Reference::forAll(Node* subTree, Node* avoid, std::function<void (NodeType*)> function)
{
	if (subTree)
	{
		QList<Node*> stack;
		stack << subTree;
		while (!stack.isEmpty())
		{
			auto top = stack.takeLast();
			if (top == avoid) continue;

			if (auto node = DCast<NodeType>(top) ) function(node);
			else stack.append(top->children());
		}
	}
}

void Reference::unresolveAll(Node* subTree, bool markForResolution)
{
	if (markForResolution)
		forAll<Reference>(subTree, nullptr, [](Reference* ref) { ref->setResolutionNeeded();});
	else
		forAll<Reference>(subTree, nullptr, [](Reference* ref) { ref->state_ = ReferenceNeedsToBeResolved; });
}

void Reference::unresolveNames(Node* subTree, bool markForResolution, const QSet<QString>& names)
{
	if (markForResolution)
		forAll<Reference>(subTree, nullptr, [=](Reference* ref)
				{ if (names.contains(ref->name())) ref->setResolutionNeeded();});
	else
		forAll<Reference>(subTree, nullptr, [=](Reference* ref)
				{ if (names.contains(ref->name())) ref->state_ = ReferenceNeedsToBeResolved; });
}

void Reference::unresolveIfNameIntroduced(Node* subTreeToUnresolve, bool markForResolution,
		Node* subTreeToLookForNewNames)
{
	QSet<QString> names;
	forAll<NameText>(subTreeToLookForNewNames, nullptr, [&names](NameText* name){ names.insert(name->get());});
	if (names.isEmpty()) return;

	if (markForResolution)
		forAll<Reference>(subTreeToUnresolve, subTreeToLookForNewNames, [=](Reference* ref)
				{ if (names.contains(ref->name())) ref->setResolutionNeeded();});
	else
		forAll<Reference>(subTreeToUnresolve, subTreeToLookForNewNames, [=](Reference* ref)
				{ if (names.contains(ref->name())) ref->state_ = ReferenceNeedsToBeResolved; });
}

void Reference::resolvePending()
{
	log.debug("Resolving references");

	int i = 0;
	int round = 0;
	int resolved = 0;
	auto pending = pendingResolution_;

	while(!pending.isEmpty())
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

	log.debug("Resolved a total of " + QString::number(resolved) + " references in "
				 + QString::number(round) + (round > 1 ? " rounds." : " round."));
}

}
