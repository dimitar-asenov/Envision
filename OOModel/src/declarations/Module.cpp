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

#include "Module.h"

#include "ModelBase/src/nodes/TypedList.hpp"
template class Model::TypedList<OOModel::Module>;

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(Module)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(Module)

REGISTER_ATTRIBUTE(Module, modules, TypedListOfModule, false, false, true)
REGISTER_ATTRIBUTE(Module, classes, TypedListOfClass, false, false, true)
REGISTER_ATTRIBUTE(Module, methods, TypedListOfMethod, false, false, true)
REGISTER_ATTRIBUTE(Module, fields, TypedListOfField, false, false, true)
REGISTER_ATTRIBUTE(Module, libraries, TypedListOfUsedLibrary, false, false, true)
REGISTER_ATTRIBUTE(Module, modKind, Integer, false, false, true)

Module::Module(const QString& name, ModuleKind kind) : Super(nullptr, Module::getMetaData())
{
	setName(name);
	setKind(kind);
}

Module::SymbolTypes Module::symbolType() const
{
	return CONTAINER;
}

QList<const Model::UsedLibrary*> Module::usedLibraries() const
{
	QList<const Model::UsedLibrary*> all;
	for (auto libs : *libraries()) all.append(libs);
	for (auto m : *modules()) all << m->usedLibraries();
	return all;
}

bool Module::isNewPersistenceUnit() const
{
	return true;
}

bool Module::isTransparentForNameResolution() const
{
	return kind() == ModuleKind::Folder;
}

}
