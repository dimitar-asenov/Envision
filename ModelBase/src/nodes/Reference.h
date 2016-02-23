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

#pragma once

#include "TypedList.h"
#include "nodeMacros.h"

namespace Model { class Reference; }
extern template class MODELBASE_API Model::TypedList<Model::Reference>;

namespace Model {

class PersistentStore;

class MODELBASE_API Reference: public Super<Node>
{
	friend class PersistentStore;

	NODE_DECLARE_STANDARD_METHODS( Reference )

	public:
		Reference(const Reference& other);
		virtual ~Reference() override;

		const QString& name() const;
		void setName(const QString &name);

		Node* target();

		virtual void save(PersistentStore &store) const override;
		virtual void load(PersistentStore &store) override;

		bool resolve();
		bool isResolved() const;

		virtual Node* computeTarget() const;

		/**
		 * Invalidates the target of this reference and enters a state where resolution is needed.
		 */
		void setResolutionNeeded();

		/**
		 * Marks all the references in the provided \a subTree as unresolved.
		 */
		static void unresolveAll(Node* subTree);

		/**
		 * Marks references whose name is in \a names in the provided \a subTree as unresolved.
		 */
		static void unresolveNames(Node* subTree, const QSet<QString>& names);

		/**
		 * Performs all necessary operations to unresolve refrences after a new tree has been introduced.
		 */
		static void unresolveAfterNewSubTree(Node* subTree);

		/**
		 * Adds the \a step function to be called when references are being unresolved after a sub tree is added.
		 */
		static void addUnresolutionSteps(std::function<void (Node* subTree)> step);

		/**
		 * Marks as unresolved references in \a subTreeToUnresolve, whose name is identical to any NameText node in
		 * \a subTreeToLookForNewNames.
		 */
		static void unresolveIfNameIntroduced(Node* subTreeToUnresolve, Node* subTreeToLookForNewNames);

		static void resolvePending();

	private:
		Node* target_{};
		QString name_;

		/**
		 * A set of all unresolved references which are pending resolution. Note that there can be unresolved references
		 * which are not pending resolution, e.g. references in deleted branches of a tree, which are now owned by the
		 * undo stack.
		 */
		static QSet<Reference*> pendingResolution_;

		/**
		 * A set of all existing reference objects.
		 */
		static QList<Reference*> allReferences_;

		static QList<std::function<void (Node* subTree)>> unresolutionSteps_;

		enum State {ReferenceEstablished, ReferenceNeedsToBeResolved, ReferenceIsBeingResolved};
		State state_{ReferenceNeedsToBeResolved};

		bool resolveHelper(bool indirect);

		static void unresolveReferencesHelper(Node* subTree, bool all, const QSet<QString>& names);

		template<typename NodeType>
		static void forAll(Node* subTree, std::function<void (NodeType* node)> function);

		virtual void targetChanged(Node* oldTarget);
};

inline const QString& Reference::name() const { return name_; }
inline bool Reference::isResolved() const { return (state_ == ReferenceEstablished) && target_; }
inline bool Reference::resolve() { return resolveHelper(false); }

template <typename NodeType>
void Reference::forAll(Node* subTree, std::function<void(NodeType* node)> function)
{
	if (subTree)
	{
		QList<Node*> stack;
		stack << subTree;
		while (!stack.isEmpty())
		{
			auto top = stack.takeLast();

			if (auto node = DCast<NodeType>(top) ) function(node);
			else stack.append(top->children());
		}
	}
}

}
