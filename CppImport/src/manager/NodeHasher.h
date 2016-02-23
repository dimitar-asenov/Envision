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

#include "../cppimport_api.h"
#include "../ClangHelpers.h"

namespace CppImport {

/**
 * This is a helper class which aims to create unique identifiers for clang nodes.
 * The methods should return the same value for full defined clang nodes and declarations.
 * The user of this class has to make sure that the source manager is set and valid.
 */
class CPPIMPORT_API NodeHasher
{
	public:
		NodeHasher(ClangHelpers& clang);

		const QString hashFunction(const clang::FunctionDecl* functionDecl);
		const QString hashMethod(const clang::CXXMethodDecl* methodDecl);
		const QString hashNameSpace(const clang::NamespaceDecl* namespaceDecl);
		const QString hashRecord(const clang::RecordDecl* recordDecl);
		const QString hashClassTemplate(const clang::ClassTemplateDecl* classTemplate);
		const QString hashClassTemplateSpec(const clang::ClassTemplateSpecializationDecl* classTemplateSpec);
		const QString hashEnum(const clang::EnumDecl* enumDecl);

		/**
		 * Hashes a static field.
		 * Note that clang treats static fields as VarDecl's.
		 */
		const QString hashStaticField(const clang::VarDecl* varDecl);
		/**
		 * This function is a dispatcher for a \a context which represents a class to return the correct hash.
		 * It is used to hash the context (=parent) of static fields.
		 */
		const QString hashParentOfStaticField(const clang::DeclContext* context);

		const QString hashUsingParent(const clang::DeclContext* context);
		const QString hashUsingDirective(const clang::UsingDirectiveDecl* usingDirective);
		const QString hashUsingDecl(const clang::UsingDecl* usingDecl);
		const QString hashUnresolvedUsingDecl(const clang::UnresolvedUsingValueDecl* unresolvedUsing);
		const QString hashNameSpaceAlias(const clang::NamespaceAliasDecl* namespaceAlias);
		const QString hashTypeAlias(const clang::TypedefNameDecl* typeAlias);
		const QString hashTypeAliasTemplate(const clang::TypeAliasTemplateDecl* typeAliasTemplate);
		const QString hashNestedNameSpecifier(const clang::NestedNameSpecifier* nestedName);

		const QString hashType(const clang::QualType& type);
		const QString hashTypeSourceInfo(const clang::TypeSourceInfo* info);
		const QString hashTemplateTypeParm(const clang::TemplateTypeParmDecl* templTypeParam);
		const QString hashTemplateTypeParm(const clang::NonTypeTemplateParmDecl* nonTypeTemplParam);
		const QString hashTemplateArg(const clang::TemplateArgument& templateArg);

		const QString hashNamespaceField(const clang::VarDecl* varDecl);
	private:
		ClangHelpers& clang_;
};

}
