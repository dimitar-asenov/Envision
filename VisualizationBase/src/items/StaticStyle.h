/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "../visualizationbase_api.h"

#include "ItemStyle.h"

namespace Visualization {

class Static;

class VISUALIZATIONBASE_API StaticStyle : public Super<ItemStyle>
{
	public:

		// Returns true if the even has been handled and should not be propagated any further.
		using ClickHandler = std::function<bool (Static* clickedItem)>;
		StaticStyle();
		virtual ~StaticStyle();

		void load(StyleLoader& sl);

		const QString& itemClass() const;
		const ItemStyle& itemStyle() const;
		int zValue() const;
		bool alwaysStretchable() const;

		bool isEmpty() const;

		ClickHandler clickHandler() const;
		void setClickHandler(ClickHandler clickHandler) const;

	private:
		QString itemClass_;
		int zValue_{};
		bool alwaysStretchable_{};
		QSharedPointer<ItemStyle> itemStyle_;

		mutable ClickHandler clickHandler_{};
};

class VISUALIZATIONBASE_API StaticSequenceStyle : public Super<Visualization::ItemStyle>
{
	private:
		QVector<StaticStyle> seq_;

	public:
		void load(Visualization::StyleLoader& sl);

		const StaticStyle& stat(int i) const;
};

inline const StaticStyle& StaticSequenceStyle::stat(int i) const { return seq_.at(i); }

inline const QString& StaticStyle::itemClass() const { return itemClass_; }
inline const ItemStyle& StaticStyle::itemStyle() const { return *itemStyle_.data(); }
inline int StaticStyle::zValue() const { return zValue_; }
inline bool StaticStyle::alwaysStretchable() const { return alwaysStretchable_; }

inline bool StaticStyle::isEmpty() const { return itemClass_.isEmpty(); }

inline StaticStyle::ClickHandler StaticStyle::clickHandler() const {return clickHandler_;}
inline void StaticStyle::setClickHandler(ClickHandler clickHandler) const {clickHandler_ = clickHandler;}

}
