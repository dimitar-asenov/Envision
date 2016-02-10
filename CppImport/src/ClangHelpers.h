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

#include "cppimport_api.h"
#include "EnvisionToClangMap.h"

#include "OOModel/src/expressions/ReferenceExpression.h"
#include "Comments/src/nodes/CommentNode.h"

namespace OOModel {
	class Project;
}

namespace CppImport {

class Comment;

/**
 * holds the clang::SourceManager and clang::Preprocessor during macro import.
 * it provides helper methods that only depend on the SourceManager and Preprocessor.
 */
class CPPIMPORT_API ClangHelpers
{
	public:
		ClangHelpers(OOModel::Project* rootProject, QString rootProjectPath);

		const clang::SourceManager* sourceManager() const;
		void setSourceManager(const clang::SourceManager* sourceManager);

		const clang::Preprocessor* preprocessor() const;
		void setPreprocessor(const clang::Preprocessor* preprocessor);

		clang::SourceLocation immediateMacroLocation(clang::SourceLocation location) const;
		void immediateSpellingHistory(clang::SourceLocation loc, QList<clang::SourceLocation>& result) const;

		QList<QString> argumentNames(const clang::MacroDirective* definition) const;

		bool isMacroRange(clang::SourceRange range) const;

		QString unexpandedSpelling(clang::SourceRange range) const;
		QString unexpandedSpelling(clang::SourceLocation start, clang::SourceLocation end) const;

		EnvisionToClangMap& envisionToClangMap();

		void deleteNode(Model::Node* node);

		template<typename NodeType, typename ... ConstructorArgTypes>
		NodeType* createNode(clang::SourceRange sourceRange, ConstructorArgTypes&&... constructorArgs);
		template<typename NodeType, typename ... ConstructorArgTypes>
		NodeType* createNamedNode(clang::NamedDecl* namedDecl, ConstructorArgTypes&&... constructorArgs);
		OOModel::ReferenceExpression* createReference(clang::SourceRange sourceRange);
		QString spelling(clang::SourceRange sourceRange) const;

		OOModel::Project* projectForLocation(clang::SourceLocation location);
		OOModel::Project* projectByName(const QString& name);
		QString projectNameFromPath(QString path);

		OOModel::Project* rootProject();
		QList<Comment*>& comments();


		/**
		 * given a source range calculates the source range corresponding to the code expanded there.
		 *
		 * Example: Macro argument:
		 *
		 *                     start|        |end
		 *                           1+2 == 3
		 *                          |        |
		 *                          |         \
		 *                           Condition
		 *	                out_start|         |out_end
		 *
		 *
		 * Example: Concatenated tokens:
		 *
		 *                    start|          |end    (start != end)
		 *                          Door::open
		 *                         /          \
		 *                        /            \
		 *                        Do##or::##open
		 *                       |out_start     |out_end
		 *
		 *                      start||end            (start == end)
		 *										Door
		 *                           /\___
		 *                          |     \
		 *                           Do##or
		 *	                out_start|     |out_end
		 */
		clang::SourceRange getUnexpandedRange(clang::SourceRange sourceRange) const;

	private:
		EnvisionToClangMap envisionToClangMap_;

		const clang::SourceManager* sourceManager_{};
		const clang::Preprocessor* preprocessor_{};

		QHash<QString, OOModel::Project*> projects_;

		OOModel::Project* rootProject_{};
		QString rootProjectPath_{};

		/*
		 * holds all comments of the current translation unit.
		 */
		QList<Comment*> comments_;
};

inline const clang::SourceManager* ClangHelpers::sourceManager() const { return sourceManager_; }
inline void ClangHelpers::setSourceManager(const clang::SourceManager* sourceManager)
{ sourceManager_ = sourceManager; }
inline const clang::Preprocessor* ClangHelpers::preprocessor() const { return preprocessor_; }
inline void ClangHelpers::setPreprocessor(const clang::Preprocessor* preprocessor)
{ preprocessor_ = preprocessor; }

inline bool ClangHelpers::isMacroRange(clang::SourceRange range) const
{ return range.getBegin().isMacroID() && range.getEnd().isMacroID(); }

inline QString ClangHelpers::unexpandedSpelling(clang::SourceLocation start, clang::SourceLocation end) const
{ return unexpandedSpelling(clang::SourceRange{start, end}); }

inline EnvisionToClangMap& ClangHelpers::envisionToClangMap() { return envisionToClangMap_; }
inline QList<Comment*>& ClangHelpers::comments() { return comments_; }

template<typename NodeType, typename ... ConstructorArgTypes>
NodeType* ClangHelpers::createNode(clang::SourceRange sourceRange, ConstructorArgTypes&&... constructorArgs)
{
	 auto node = new NodeType{std::forward<ConstructorArgTypes>(constructorArgs)...};
	 envisionToClangMap_.mapAst(sourceRange, node);
	 return node;
}

template<typename NodeType, typename ... ConstructorArgTypes>
inline NodeType* ClangHelpers::createNamedNode(clang::NamedDecl* namedDecl, ConstructorArgTypes&&... constructorArgs)
{
	auto name = spelling(namedDecl->getLocation());
	if (name == "~") name += spelling(namedDecl->getLocation().getLocWithOffset(1));
	else if (name == "," || name == ")") name = ""; // if there is no name we sometimes pick up the next token
	auto namedNode = createNode<NodeType>(namedDecl->getSourceRange(), name,
													  std::forward<ConstructorArgTypes>(constructorArgs)...);
	envisionToClangMap_.mapAst(namedDecl->getLocation(), namedNode->nameNode());
	/*
	 * comments processing 2 of 3.
	 * process comments which are associated with declarations.
	 */
	if (auto compositeNode = DCast<Model::CompositeNode>(namedNode))
		if (auto commentForDeclaration = namedDecl->getASTContext().getRawCommentForDeclNoCache(namedDecl))
		{
			auto lines = QString::fromStdString(commentForDeclaration->getRawText(*sourceManager_).str()).split('\n');
			for (auto i = 0; i < lines.length(); i++)
				lines[i] = lines[i].trimmed();
			compositeNode->setComment(new Comments::CommentNode{lines.join('\n')});
		}
	return namedNode;
}

inline OOModel::ReferenceExpression* ClangHelpers::createReference(clang::SourceRange sourceRange)
{ return createNode<OOModel::ReferenceExpression>(sourceRange, spelling(sourceRange.getBegin())); }

inline OOModel::Project* ClangHelpers::projectByName(const QString& name) { return projects_[name]; }

inline OOModel::Project* ClangHelpers::rootProject() { return rootProject_; }

}
