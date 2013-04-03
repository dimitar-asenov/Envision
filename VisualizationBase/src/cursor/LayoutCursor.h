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

#include "../visualizationbase_api.h"
#include "../layouts/Layout.h"
#include "Cursor.h"

namespace Visualization {

class Element;

class VISUALIZATIONBASE_API LayoutCursor : public Cursor {
	public:
		LayoutCursor(Item* owner, CursorType type);

		void setVisualizationSize(const QSize& size);

		void set2DIndex(int x, int y);
		void setIndex(int index);
		void setVisualizationPosition(const QPoint& pos);
		void setOwnerElement(Element* ownerElement);

		int x();
		int y();
		int index();
		Element* ownerElement();

		void setIsAtBoundary(bool isAtBoundary);

		virtual bool isSame(Cursor* c);
		virtual bool isAtBoundary() const;

	private:
		int x_;
		int y_;
		int index_;
		bool isAtBoundary_;
		Element* ownerElement_;
};

inline void LayoutCursor::set2DIndex(int x, int y) { x_ = x; y_ = y; }
inline void LayoutCursor::setIndex(int index) { index_ = index; }
inline void LayoutCursor::setOwnerElement(Element* ownerElement) {ownerElement_ = ownerElement;}

inline int LayoutCursor::x() { return x_; }
inline int LayoutCursor::y() { return y_; }
inline int LayoutCursor::index() { return index_; }
inline Element* LayoutCursor::ownerElement() { return ownerElement_; }

inline void LayoutCursor::setIsAtBoundary(bool isAtBoundary) { isAtBoundary_ = isAtBoundary; }

} /* namespace Visualization */
