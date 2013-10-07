/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "../comments_api.h"

#include "VisualizationBase/src/declarative/DeclarativeItem.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/items/ItemWithNode.h"

#include "VCommentDiagram.h"
#include "../nodes/CommentNode.h"

namespace Comments {

class COMMENTS_API VComment : public Super<Visualization::ItemWithNode<VComment,
	Visualization::DeclarativeItem<VComment>, CommentNode> >
{
	ITEM_COMMON_CUSTOM_STYLENAME(VComment, Visualization::DeclarativeItemBaseStyle)

	public:
		VComment(Visualization::Item* parent, NodeType* node);
		static void initializeForms();
		virtual int determineForm() override;
		void clearChildren();
		QMap<QString, CommentDiagram*> diagrams() const;

		void toggleEditing();
		bool editing() const;

	private:
		void parseLines();
		void synchroniseDiagrams(QSet<QString> itemDiagramNames);
		QString replaceMarkdown(QString str);
		void pushTextLine(QString text);
		void popLineBuffer();
		void addChildItem(Visualization::Item* item);
		QVector<QPair<QString,QString>>* parseMarkdownArguments(const QString& argString);
		QSize parseSize(const QString& str);

		QStringList lineBuffer_{};
		bool editing_{};
		Visualization::Item* editLabel_{};
		QMap<QString, CommentDiagram*> diagrams_{};
		QList<Visualization::Item*> children_{};
};

inline void VComment::clearChildren() { children_.clear(); }

} /* namespace Comments */
