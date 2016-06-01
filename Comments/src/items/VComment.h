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

#include "../comments_api.h"
#include "VCommentStyle.h"
#include "../nodes/CommentNode.h"

#include "VisualizationBase/src/declarative/DeclarativeItem.h"
#include "VisualizationBase/src/items/ItemWithNode.h"
#include "VisualizationBase/src/items/VList.h"

namespace Comments {

class VCommentDiagram;

class COMMENTS_API VComment : public Super<Visualization::ItemWithNode<VComment,
	Visualization::DeclarativeItem<VComment>, CommentNode> >
{
	ITEM_COMMON(VComment)

	public:
		VComment(Visualization::Item* parent, NodeType* node, const StyleType* style = itemStyles().get());

		static void initializeForms();
		virtual int determineForm() override;

		QList<QPair<QString, QString>> parseMarkdownArguments(const QString& argString);

		void toggleEditing();
		bool editing() const;

	protected:
		virtual void determineChildren() override;

	private:
		bool editing_{};
		Visualization::VList* editText_{};
		QList<Visualization::Item*> commentElements_{};

		void parseLines();
		QString replaceMarkdown(QString str);
		Item* createTextualCommentElement(QStringList& contents, bool asHtml = false);
		QSize parseSize(const QString& str);
};

inline bool VComment::editing() const { return editing_; }
}
