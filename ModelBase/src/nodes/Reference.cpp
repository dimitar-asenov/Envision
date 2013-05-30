/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(Model::Reference)

namespace Model {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Reference)

Reference::Reference(Node *parent) : Super(parent), target_()
{
	manageUnresolvedReferencesListInModel();
}

Reference::Reference(Node *parent, PersistentStore &store, bool) : Super(parent), target_()
{
	name_ = store.loadReferenceValue(this);
	manageUnresolvedReferencesListInModel();
}

void Reference::setName(const QString &name, bool tryResolvingImmediately)
{
	execute(new FieldSet<QString> (this, name_, name));
	execute(new FieldSet<Node*> (this, target_, nullptr));

	if (tryResolvingImmediately) resolve();
	manageUnresolvedReferencesListInModel();
}

void Reference::setTarget(Node* target)
{
	if (target) execute(new FieldSet<QString> (this, name_, QString()));
	else execute(new FieldSet<QString> (this, name_, name()));

	execute(new FieldSet<Node*> (this, target_, target));
	manageUnresolvedReferencesListInModel();
}

bool Reference::resolve()
{
	return false;
}

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

void Reference::manageUnresolvedReferencesListInModel()
{
	if (model())
	{
		if (isResolved()) model()->removeUnresolvedReference(this);
		else model()->addUnresolvedReference(this);
	}
}

}
