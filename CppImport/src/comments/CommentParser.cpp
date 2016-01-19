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

#include "CommentParser.h"

namespace CppImport {

Comments::CommentNode* CommentParser::parseComment(clang::comments::Comment* comment)
{
	// empty old text
	collectedText_.clear();
	// handle the current comment
	processComment(comment);
	return new Comments::CommentNode{collectedText_};
}

void CommentParser::processComment(clang::comments::Comment* comment)
{
	if (!comment) return;

	if (auto tc = llvm::dyn_cast<clang::comments::TextComment>(comment))
		processTextComment(tc);
	else if (auto fc = llvm::dyn_cast<clang::comments::FullComment>(comment))
		processFullComment(fc);
	else if (auto pc = llvm::dyn_cast<clang::comments::ParagraphComment>(comment))
		processParagraphComment(pc);
	else
		qDebug() << "Unsupported comment type: " << comment->getCommentKindName();
}

void CommentParser::processTextComment(clang::comments::TextComment* textComment)
{
	if (!textComment->isWhitespace())
		collectedText_.append(QString::fromStdString(textComment->getText().str()));
}

void CommentParser::processFullComment(clang::comments::FullComment* fullComment)
{
	for (auto it = fullComment->child_begin(); it != fullComment->child_end(); ++it) processComment(*it);
}

void CommentParser::processParagraphComment(clang::comments::ParagraphComment* paragraphComment)
{
	for (auto it = paragraphComment->child_begin(); it != paragraphComment->child_end(); ++it) processComment(*it);
}

}
