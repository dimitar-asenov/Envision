/***********************************************************************************************************************
 * Static.h
 *
 *  Created on: Mar 30, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef STATIC_H_
#define STATIC_H_

#include "../visualizationbase_api.h"

#include "Item.h"
#include "StaticStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API Static : public Item
{
	ITEM_COMMON(Static)

	friend class StaticStyle;

	public:
		Static(Item* parent, const StyleType *style = itemStyles().get());
		virtual ~Static();

		virtual bool focusChild(FocusTarget location);
		virtual bool isEmpty() const;
		virtual bool sizeDependsOnParent() const;

		template<class T> static void registerStaticItem();

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Item* item_;

		typedef Item* (*staticItemConstructorType)(Item* parent, const ItemStyle* style);
		static QMap<QString, staticItemConstructorType> itemClasses_;

		typedef ItemStyle* (*staticItemStyleConstructorType)();
		static QMap<QString, staticItemStyleConstructorType> itemStyles_;

		template<class T> static Item* staticItemConstructor(Item* parent, const ItemStyle* style);
		template<class T> static ItemStyle* staticItemStyleConstructor();

		static ItemStyle* constructStyle(const QString& itemClass);
};

template<class T> void Static::registerStaticItem()
{
	itemClasses_.insert(T::className(), staticItemConstructor<T>);
	itemStyles_.insert(T::className(), staticItemStyleConstructor<T>);
}

template<class T> Item* Static::staticItemConstructor(Item* parent, const ItemStyle* style)
{
	return new T(parent, static_cast<const typename T::StyleType*> (style) );
}

template<class T> ItemStyle* Static::staticItemStyleConstructor()
{
	return new typename T::StyleType();
}

}

#endif /* STATIC_H_ */
