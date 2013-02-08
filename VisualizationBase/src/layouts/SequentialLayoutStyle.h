/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#ifndef SEQUENTIALLAYOUTSTYLE_H_
#define SEQUENTIALLAYOUTSTYLE_H_

#include "../visualizationbase_api.h"

#include "LayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API SequentialLayoutStyle : public LayoutStyle
{
	private:
		Direction direction_;
		Alignment alignment_;
		int spaceBetweenElements_;
		bool notLocationEquivalentCursors_;
		bool noInnerCursors_;
		bool noBoundaryCursorsInsideShape_;
		bool extraCursorsOutsideShape_;
		bool hasCursorWhenEmpty_;
		int minWidth_;
		int minHeight_;

	public:
		SequentialLayoutStyle();
		virtual void load(StyleLoader& sl);

		Direction direction() const;
		Alignment alignment() const;
		int spaceBetweenElements() const;
		bool notLocationEquivalentCursors() const;
		bool noInnerCursors() const;
		bool noBoundaryCursorsInsideShape() const;
		bool extraCursorsOutsideShape() const;
		bool hasCursorWhenEmpty() const;
		int minWidth() const;
		int minHeight() const;

		void setDirection(Direction direction);
		void setAlignment(Alignment alignment);
		void setSpaceBetweenElements(int space);

		bool isHorizontal() const;
		bool isForward() const;
};

inline LayoutStyle::Direction SequentialLayoutStyle::direction() const { return direction_; }
inline LayoutStyle::Alignment SequentialLayoutStyle::alignment() const {	return alignment_; }
inline int SequentialLayoutStyle::spaceBetweenElements() const {	return spaceBetweenElements_; }
inline bool SequentialLayoutStyle::notLocationEquivalentCursors() const {return notLocationEquivalentCursors_;}
inline bool SequentialLayoutStyle::noInnerCursors() const {return noInnerCursors_;}
inline bool SequentialLayoutStyle::noBoundaryCursorsInsideShape() const {return noBoundaryCursorsInsideShape_;}
inline bool SequentialLayoutStyle::extraCursorsOutsideShape() const {return extraCursorsOutsideShape_;}
inline bool SequentialLayoutStyle::hasCursorWhenEmpty() const { return hasCursorWhenEmpty_; }
inline int SequentialLayoutStyle::minWidth() const {return minWidth_;}
inline int SequentialLayoutStyle::minHeight() const {return minHeight_;}

inline bool SequentialLayoutStyle::isHorizontal() const
	{ return direction_ == Direction::LeftToRight || direction_ == Direction::RightToLeft;}
inline bool SequentialLayoutStyle::isForward() const
	{ return direction_ == Direction::LeftToRight || direction_ == Direction::TopToBottom;}

}

#endif /* SEQUENTIALLAYOUTSTYLE_H_ */
