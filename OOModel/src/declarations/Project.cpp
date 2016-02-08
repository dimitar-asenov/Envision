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

#include "Project.h"

#include "ModelBase/src/nodes/TypedList.hpp"
template class Model::TypedList<OOModel::Project>;

namespace OOModel {

DEFINE_COMPOSITE_EMPTY_CONSTRUCTORS(Project)
DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS(Project)

DEFINE_ATTRIBUTE(Project, projects, TypedListOfProject, false, false, true)
DEFINE_ATTRIBUTE(Project, modules, TypedListOfModule, false, false, true)
DEFINE_ATTRIBUTE(Project, classes, TypedListOfClass, false, false, true)
DEFINE_ATTRIBUTE(Project, methods, TypedListOfMethod, false, false, true)
DEFINE_ATTRIBUTE(Project, fields, TypedListOfField, false, false, true)
DEFINE_ATTRIBUTE(Project, libraries, TypedListOfUsedLibrary, false, false, true)
DEFINE_ATTRIBUTE(Project, implicitBaseType, Expression, false, true, true)
DEFINE_ATTRIBUTE(Project, implicitEnumType, Expression, false, true, true)
DEFINE_ATTRIBUTE(Project, implicitArrayType, Expression, false, true, true)

Project::Project(const QString& name) : Super{nullptr, Project::getMetaData()}
{
	setName(name);
}

bool Project::isTransparentForNameResolution() const
{
	return true;
}

Project::SymbolTypes Project::symbolType() const
{
	return CONTAINER;
}


QList<const Model::UsedLibrary*> Project::usedLibraries() const
{
	QList<const Model::UsedLibrary*> all;
	for (auto libs : *libraries()) all.append(libs);
	for (auto p : *projects()) all << p->usedLibraries();
	for (auto m : *modules()) all << m->usedLibraries();
	return all;
}

bool Project::isNewPersistenceUnit() const
{
	return true;
}


}
