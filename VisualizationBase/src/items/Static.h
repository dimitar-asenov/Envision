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

#include "Item.h"
#include "StaticStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API Static : public Super<Item>
{
	ITEM_COMMON(Static)

	friend class StaticStyle;

	public:
		Static(Item* parent, const StyleType *style = itemStyles().get());
		virtual ~Static();

		virtual bool isEmpty() const override;
		virtual bool sizeDependsOnParent() const override;

		template<typename T> static void registerStaticItem();

		Item* item();

	protected:
		virtual void determineChildren() override;
		virtual void updateGeometry(int availableWidth, int availableHeight) override;

	private:
		Item* item_;

		using staticItemConstructorType = Item* (*)(Item* parent, const ItemStyle* style);
		static QMap<QString, staticItemConstructorType> itemClasses_;

		using staticItemStyleConstructorType = ItemStyle* (*)();
		static QMap<QString, staticItemStyleConstructorType> itemStyles_;

		template<typename T> static Item* staticItemConstructor(Item* parent, const ItemStyle* style);
		template<typename T> static ItemStyle* staticItemStyleConstructor();

		static ItemStyle* constructStyle(const QString& itemClass);
};

inline Item* Static::item() { return item_; }

template<typename T> void Static::registerStaticItem()
{
	itemClasses_.insert(T::typeNameStatic(), staticItemConstructor<T>);
	itemStyles_.insert(T::typeNameStatic(), staticItemStyleConstructor<T>);
}

template<typename T> Item* Static::staticItemConstructor(Item* parent, const ItemStyle* style)
{
	return new T{parent, static_cast<const typename T::StyleType*> (style) };
}

template<typename T> ItemStyle* Static::staticItemStyleConstructor()
{
	return new typename T::StyleType{};
}

}
