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

#include "Project.h"

#include "ModelBase/src/nodes/TypedListDefinition.h"
DEFINE_TYPED_LIST(OOModel::Project)

namespace OOModel {

COMPOSITENODE_DEFINE_EMPTY_CONSTRUCTORS(Project)
COMPOSITENODE_DEFINE_TYPE_REGISTRATION_METHODS(Project)

REGISTER_ATTRIBUTE(Project, projects, TypedListOfProject, false, false, true)
REGISTER_ATTRIBUTE(Project, modules, TypedListOfModule, false, false, true)
REGISTER_ATTRIBUTE(Project, classes, TypedListOfClass, false, false, true)
REGISTER_ATTRIBUTE(Project, methods, TypedListOfMethod, false, false, true)
REGISTER_ATTRIBUTE(Project, fields, TypedListOfField, false, false, true)

Project::Project(const QString& name) : Super(nullptr, Project::getMetaData())
{
	setName(name);
}

QList<Model::Node*> Project::findSymbols(const QRegExp& symbolExp,Model::Node* source, FindSymbolMode mode,
		bool exhaustAllScopes)
{
	QList<Model::Node*> symbols;

	symbols << projects()->findAllSymbolDefinitions(symbolExp);
	symbols << modules()->findAllSymbolDefinitions(symbolExp);
	symbols << classes()->findAllSymbolDefinitions(symbolExp);
	symbols << methods()->findAllSymbolDefinitions(symbolExp);
	symbols << fields()->findAllSymbolDefinitions(symbolExp);
	symbols << subDeclarations()->findAllSymbolDefinitions(symbolExp);

	if (exhaustAllScopes || symbols.isEmpty())
		symbols << Node::findSymbols(symbolExp, source, mode, exhaustAllScopes);

	return symbols;
}

}
