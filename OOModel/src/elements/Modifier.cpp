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

#include "Modifier.h"

#include "../OOModelException.h"
#include "ModelBase/src/commands/FieldSet.h"

#include "ModelBase/src/nodes/TypedList.hpp"
template class Model::TypedList<OOModel::Modifier>;

namespace OOModel {

DEFINE_NODE_TYPE_REGISTRATION_METHODS(Modifier)

Modifier::Modifier(Model::Node *parent)
: Super{parent}
{
}

Modifier::Modifier(Model::Node *parent, Model::PersistentStore &store, bool)
: Super{parent}
{
	modifiers_ = fromInt( store.loadIntValue() );
}

Modifier::Modifier(Modifiers modifiers)
: Super{nullptr}, modifiers_{modifiers}
{}

Modifier* Modifier::clone() const { return new Modifier{*this}; }

void Modifier::set(Modifiers modifiers, bool enable)
{
	execute(new Model::FieldSet<Modifiers>{this, modifiers_,
			enable ? modifiers_ | modifiers : modifiers_ & (~modifiers)});
}

void Modifier::clear()
{
	execute(new Model::FieldSet<Modifiers>{this, modifiers_, None});
}

void Modifier::save(Model::PersistentStore &store) const
{
	store.saveIntValue(modifiers_);
}

void Modifier::load(Model::PersistentStore &store)
{
	if (store.currentNodeType() != typeName())
		throw OOModelException{"Trying to load a Modifier node from an incompatible node type "
				+ store.currentNodeType()};

	set(fromInt(store.loadIntValue()));
}

Modifier::Modifiers Modifier::fromInt(int val)
{
	//TODO: Do some error checking
	return static_cast<Modifiers>(val);
}

}
