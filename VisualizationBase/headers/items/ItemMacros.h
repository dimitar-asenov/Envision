/***********************************************************************************************************************
 * ItemMacros.h
 *
 *  Created on: Dec 21, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ITEMMACROS_H_
#define ITEMMACROS_H_

/**
 * Defines standard methods that every item should implement. These include class name information and style handling.
 *
 * @param ItemClass
 * 			The name of the class being defined. This class must inherit directly from BaseClass.
 *
 * @param BaseClass
 * 			The base class of ItemClass. This class must inherit from from Item, directly or indirectly.
 *
 * @param StyleTypeName
 * 			The name of the style type for this class
 */
#define ITEM_COMMON_CUSTOM_STYLENAME( ItemClass, BaseClass, StyleTypeName)															\
public:																																					\
	typedef StyleTypeName StyleType;																												\
	static const QString& className() { static QString name(#ItemClass); return name; }											\
	const StyleType* style() const { return static_cast<const StyleType*> (BaseClass::style()); }							\
	virtual void setStyle(const ItemStyle* style)																							\
	{																																						\
		const StyleType* s = dynamic_cast<const StyleType*> (style);																	\
		if (!s) throw VisualizationException("Invalid style type when calling " #ItemClass "::setStyle");					\
		Item::setStyle(s);																															\
	}																																						\
private:

/**
 * Defines standard methods that every item should implement. These include class name information and style handling.
 *
 * @param ItemClass
 * 			The name of the class being defined. This class must inherit directly from BaseClass.
 *
 * @param BaseClass
 * 			The base class of ItemClass. This class must inherit from from Item, directly or indirectly.
 */
#define ITEM_COMMON( ItemClass, BaseClass) ITEM_COMMON_CUSTOM_STYLENAME(ItemClass, BaseClass, ItemClass##Style)

#endif /* ITEMMACROS_H_ */
