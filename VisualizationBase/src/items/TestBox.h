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

#include "TextRenderer.h"
#include "../nodes/TestBoxNode.h"

namespace Visualization {

class VISUALIZATIONBASE_API TestBox : public Super<TextRenderer> {

	ITEM_COMMON_CUSTOM_STYLENAME(TestBox, TextStyle)

	public:
		using NodeType = TestBoxNode;

		TestBox(Item* parent, NodeType* node);
		TestBox(NodeType* node);
		TestBox(const QString& label, QColor color, bool sizeDependsOnParent, bool isHtml);
		TestBox(const QString& label, bool sizeDependsOnParent, bool isHtml);

		virtual bool sizeDependsOnParent() const override;

		virtual bool hasNode() const override;
		virtual NodeType* node() const override;
		virtual int revision() const override;
		virtual void setRevision(int newRevision) override;

		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

		virtual bool setText(const QString& newText) override;
		virtual bool moveCursor(CursorMoveDirection dir = MoveDefault, QPoint reference = {},
										CursorMoveOptions options = None) override;

	protected:
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;
		virtual QString currentText() override;

	private:
		NodeType* node_{};
		int nodeRevision_{-1};

		QColor color_{Qt::yellow};
		bool sizeDependsOnParent_{};
		bool isHtml_{};
};

}
