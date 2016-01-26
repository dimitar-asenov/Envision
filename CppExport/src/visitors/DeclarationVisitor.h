/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "../cppexport_api.h"

#include "Export/src/Visitor.h"

namespace Model {
	class CompositeNode;
}

namespace OOModel {
	class Project;
	class Module;
	class Class;
	class Declaration;
	class Method;
	class NameImport;
	class VariableDeclaration;
	class ExplicitTemplateInstantiation;
	class TypeAlias;
	class MetaDefinition;
	class Expression;
	class Field;
}

namespace Export {
	class SourceDir;
	class SourceFile;
	class SourceFragment;
}

namespace CppExport {

class ExpressionVisitor;
class StatementVisitor;
class ElementVisitor;

const int HEADER_VISITOR = 0;
const int SOURCE_VISITOR = 1;
const int MACRO_VISITOR = 2;

class CPPEXPORT_API DeclarationVisitor
:public Export::Visitor<DeclarationVisitor, ExpressionVisitor, StatementVisitor, ElementVisitor>
{
	public:
		using Visitor::Visitor;

		Export::SourceFragment* visit(OOModel::Declaration* declaration);

		Export::SourceFragment* visit(OOModel::Class* classs);
		Export::SourceFragment* visit(OOModel::Method* method);
		Export::SourceFragment* visit(OOModel::MetaDefinition* metaDefinition);

		Export::SourceFragment* visit(OOModel::NameImport* nameImport);
		Export::SourceFragment* visit(OOModel::VariableDeclaration* vd);

		Export::SourceFragment* visit(OOModel::ExplicitTemplateInstantiation* eti);
		Export::SourceFragment* visit(OOModel::TypeAlias* typeAlias);

		Export::SourceFragment* printAnnotationsAndModifiers(OOModel::Declaration* declaration);

		Export::SourceFragment* visitTopLevelClass(OOModel::Class* classs);

		static Export::SourceFragment* compositeNodeComments(Model::CompositeNode* compositeNode, const QString& style);

	private:
		template<typename Predicate>
		bool addMemberDeclarations(OOModel::Class* classs, Export::CompositeFragment* section, Predicate filter);

		bool isHeaderVisitor();
		bool isSourceVisitor();

		static bool metaCallFilter(OOModel::Expression* expression, bool equal);

		Export::SourceFragment* visitHeaderPart(OOModel::Class* classs);
		Export::SourceFragment* visitSourcePart(OOModel::Class* classs);
		Export::SourceFragment* visitHeaderPart(OOModel::VariableDeclaration* variableDeclaration);
		Export::SourceFragment* visitSourcePart(OOModel::VariableDeclaration* variableDeclaration);
		Export::SourceFragment* visitSourcePart(OOModel::Field* field);
};

inline bool DeclarationVisitor::isHeaderVisitor() { return data().get()->modeStack_.last() == HEADER_VISITOR; }
inline bool DeclarationVisitor::isSourceVisitor() { return data().get()->modeStack_.last() == SOURCE_VISITOR; }

}
