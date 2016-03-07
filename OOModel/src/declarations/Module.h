/***********************************************************************************************************************
**
** Copyright (c) 2011, 2016 ETH Zurich
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

#include "../oomodel_api.h"

#include "Class.h"
#include "ModelBase/src/nodes/UsedLibrary.h"

namespace OOModel {
	class Module;
}
extern template class OOMODEL_API Model::TypedList<OOModel::Module>;

namespace OOModel {

class OOMODEL_API Module : public Super<Declaration>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(Module)

	ATTRIBUTE(Model::TypedList<Module>, modules, setModules)
	ATTRIBUTE(Model::TypedList<Class>, classes, setClasses)
	ATTRIBUTE(Model::TypedList<Method>, methods, setMethods)
	ATTRIBUTE(Model::TypedList<Field>, fields, setFields)
	ATTRIBUTE(Model::TypedList<Model::UsedLibrary>, libraries, setLibraries)

	PRIVATE_ATTRIBUTE_VALUE(Model::Integer, modKind, setModKind, int)

	public:
		enum class ModuleKind : int {Standard /**< Used for namespaces or packages. */,
											  Folder /**< Used for grouping declarations in sub-directories. */};

		Module(const QString& name, ModuleKind kind = ModuleKind::Standard);

		virtual SymbolTypes symbolType() const override;
		virtual QList<const Model::UsedLibrary*> usedLibraries() const override;

		virtual bool isNewPersistenceUnit() const override;
		virtual bool isTransparentForNameResolution() const override;

		ModuleKind kind() const;
		void setKind(ModuleKind kind);
};

inline Module::ModuleKind Module::kind() const { return static_cast<ModuleKind> (modKind()); }
inline void Module::setKind(ModuleKind kind) { setModKind(static_cast<int> (kind)); }

}
