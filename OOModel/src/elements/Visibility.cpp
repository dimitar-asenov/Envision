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

#include "elements/Visibility.h"
#include "ModelBase/src/commands/FieldSet.h"
#include "OOModelException.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::Visibility)

namespace OOModel {

NODE_DEFINE_TYPE_REGISTRATION_METHODS(Visibility)

Visibility::Visibility(Model::Node *parent)
: Super(parent), vis(DEFAULT)
{
}

Visibility::Visibility(Model::Node *parent, Model::PersistentStore &store, bool)
: Super(parent)
{
	vis = fromInt( store.loadIntValue() );
}

Visibility::Visibility(const VisibilityType val)
: Super(nullptr), vis(val)
{
}

void Visibility::set(const VisibilityType& value)
{
	execute(new Model::FieldSet<VisibilityType> (this, vis, value));
}

void Visibility::save(Model::PersistentStore &store) const
{
	store.saveIntValue(vis);
}

void Visibility::load(Model::PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw OOModelException("Trying to load a Visibility node from an incompatible node type "
				+ store.currentNodeType());

	set(fromInt(store.loadIntValue()));
}

Visibility::VisibilityType Visibility::fromInt(int val)
{
	switch (val)
	{
		case DEFAULT:
			return DEFAULT;
			break;
		case PUBLIC:
			return PUBLIC;
			break;
		case PRIVATE:
			return PRIVATE;
			break;
		case PROTECTED:
			return PROTECTED;
			break;
		default:
			throw OOModelException("Trying to a load a visibility node from an invalid store location");
	}
}

}
