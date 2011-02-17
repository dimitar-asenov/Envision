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
 * 			The name of the class being defined. This class must inherit from from Item, directly or indirectly.
 *
 * @param StyleTypeName
 * 			The name of the style type for this class
 */
#define ITEM_COMMON_CUSTOM_STYLENAME( ItemClass, StyleTypeName)																		\
public:																																					\
	typedef StyleTypeName StyleType;																												\
	static const QString& className() { static QString name(#ItemClass); return name; }											\
																																							\
	const StyleType* style() const { return static_cast<const StyleType*> (Item::style()); }									\
	virtual void setStyle(const Visualization::ItemStyle* style);																		\
	virtual Visualization::InteractionHandler* handler() const { return handler_; };												\
	static void setInteractionHandler(Visualization::InteractionHandler* handler) {handler_ = handler;}					\
																																							\
private:																																					\
	static Visualization::InteractionHandler* handler_;																					\


/**
 * Defines standard methods that every item should implement. These include class name information and style handling.
 *
 * @param ItemClass
 * 			The name of the class being defined. This class must inherit from from Item, directly or indirectly.
 */
#define ITEM_COMMON( ItemClass ) ITEM_COMMON_CUSTOM_STYLENAME(ItemClass, ItemClass##Style)

/**
 * Defines standard methods and members that every item should implement.
 *
 * @param ItemClass
 * 			The name of the class being defined.
 */
#define ITEM_COMMON_DEFINITIONS( ItemClass )																									\
Visualization::InteractionHandler* ItemClass::handler_ = Visualization::InteractionHandler::instance();					\
																																							\
void ItemClass::setStyle(const Visualization::ItemStyle* style)																		\
{																																							\
	const StyleType* s = dynamic_cast<const StyleType*> (style);																		\
	if (!s) throw Visualization::VisualizationException("Invalid style type when calling " #ItemClass "::setStyle");	\
	Item::setStyle(s);																																\
}																																							\


#endif /* ITEMMACROS_H_ */
