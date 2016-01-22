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

#include "../oomodel_api.h"

#include "ModelBase/src/nodes/Node.h"
#include "ModelBase/src/nodes/nodeMacros.h"
#include "ModelBase/src/persistence/PersistentStore.h"
#include "ModelBase/src/nodes/TypedList.h"

namespace OOModel { class Modifier; }
extern template class OOMODEL_API Model::TypedList<OOModel::Modifier>;

namespace OOModel {

class OOMODEL_API Modifier :  public Super<Model::Node>
{
	NODE_DECLARE_STANDARD_METHODS(Modifier)

	public:
		enum ModifierFlag {
			None  = 0x00000000,

			Public = 0x00000001,
			Private = 0x00000002,
			Protected = 0x00000004,

			Static = 0x00000008,

			Final = 0x00000010,
			Abstract = 0x00000020,
			Virtual = 0x00000040,
			Override = 0x00000080,
			Inline = 0x00000100,
			Const = 0x00000200,
			ConstExpr = 0x0000400,

			Default = 0x00000800,
			Deleted = 0x00001000,
			Explicit = 0x00002000
		};
		Q_DECLARE_FLAGS(Modifiers, ModifierFlag)

		Modifier(Modifiers modifiers);

		Modifiers get() const;
		bool isSet(ModifierFlag flag) const;
		void set(Modifiers modifiers, bool enable = true);
		void clear();

		virtual void save(Model::PersistentStore &store) const override;
		virtual void load(Model::PersistentStore &store) override;

	private:
		Modifiers modifiers_{0};

		Modifiers fromInt(int val);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Modifier::Modifiers)

inline Modifier::Modifiers Modifier::get() const { return modifiers_; }
inline bool Modifier::isSet(ModifierFlag flag) const { return modifiers_.testFlag(flag);}

}
