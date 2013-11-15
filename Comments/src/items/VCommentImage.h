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

#include "VComment.h"
#include "VisualizationBase/src/items/Item.h"

namespace Comments {

class COMMENTS_API VCommentImage : public Super<Visualization::Item>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VCommentImage, Visualization::ItemStyle)

	public:
		VCommentImage(Visualization::Item* parent, const QString& path, const StyleType* style = itemStyles().get());
		VCommentImage(Visualization::Item* parent, const QString& path, QSize size,
				const StyleType* style = itemStyles().get());

		QSize imageSize() const;
		bool updateSize(QSize size); // Returns true if the size was successfully updated

		int lineNumber() const;
		void setLineNumber(int lineNumber);

	protected:
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

	private:

		QString path_;
		QImage image_;
		QSize size_;
		int lineNumber_{-1};

		static const QSize errorSize_;
};

inline void VCommentImage::setLineNumber(int lineNumber) { Q_ASSERT(lineNumber>=0); lineNumber_ = lineNumber; }
inline QSize VCommentImage::imageSize() const { return size_; }
inline int VCommentImage::lineNumber() const {return lineNumber_;}

} /* namespace Comments */
