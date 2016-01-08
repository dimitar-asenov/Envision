/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "cppimport_api.h"

namespace Model {
	class Node;
}

namespace OOModel {
	class StatementItemList;
	class CommentStatementItem;
}

namespace CppImport {

class CPPIMPORT_API Comment
{
	public:
		Comment(clang::RawComment* rawComment, const clang::SourceManager& sourceManager);

		/**
		 * the node this comment is associated with.
		 */
		Model::Node* node();

		/**
		 * associates the comment with a node.
		 */
		void setNode(Model::Node* node);

		clang::RawComment* rawComment();

		/**
		 * the raw text of this comment.
		 */
		QString text();

		/**
		 * the presumed file name where this comment is located.
		 */
		QString fileName();

		/**
		 * the presumed line this comment starts on.
		 */
		unsigned lineStart();

		/**
		 * the presumed line this comment ends on.
		 */
		unsigned lineEnd();

		/**
		 * associates the comment with the provided item list and creates a comment statement as a child of the list at
		 * the provided index (by default at the end of the list).
		 * the association created by this method can later be rolled back by a call to 'removeFromItemList'.
		 */
		void insertIntoItemList(OOModel::StatementItemList* itemList, int index = -1);

		/**
		 * rolls back the association made by a previous call to 'insertIntoItemList'.
		 */
		void removeFromItemList();

	private:
		clang::RawComment* rawComment_{};
		Model::Node* node_{};
		QString text_;
		QString fileName_;
		unsigned lineStart_{};
		unsigned lineEnd_{};

		/**
		 * this field is used to roll back associations made using 'insertIntoItemList'.
		 */
		OOModel::CommentStatementItem* commentStatementItem_{};
};

inline Model::Node* Comment::node() { return node_; }
inline clang::RawComment* Comment::rawComment() { return rawComment_; }
inline QString Comment::fileName() { return fileName_; }
inline unsigned Comment::lineStart() { return lineStart_; }
inline unsigned Comment::lineEnd() { return lineEnd_; }

}
