/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * ItemMacros.h
 *
 *  Created on: Dec 21, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ITEMMACROS_H_
#define ITEMMACROS_H_

#include "VisualizationBase/src/styles/StyleSet.h"

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
	static const QString& className();																											\
																																							\
	const StyleType* style() const { return static_cast<const StyleType*> (Item::style()); }									\
	virtual void setStyle(const Visualization::ItemStyle* style);																		\
	static Visualization::StyleSet<ItemClass>& itemStyles();																				\
																																							\
	virtual Visualization::InteractionHandler* handler() const { return handler_; };												\
	static void setInteractionHandler(Visualization::InteractionHandler* handler) {handler_ = handler;}					\
																																							\
private:																																					\
	static Visualization::InteractionHandler* handler_;
/**********************************************************************************************************************/

/**
 * Defines standard methods that every item should implement. These include class name information and style handling.
 *
 * @param ItemClass
 * 			The name of the class being defined. This class must inherit from from Item, directly or indirectly.
 */
#define ITEM_COMMON( ItemClass ) ITEM_COMMON_CUSTOM_STYLENAME(ItemClass, ItemClass##Style)
/**********************************************************************************************************************/

/**
 * Defines standard methods and members that every item should implement.
 *
 * @param ItemClass
 * 			The name of the class being defined.
 * @param classType
 * 			The class type of this item. This determines the folder inside the root 'styles' folder that will be used to
 * 			look for this item's style directory. Typical values are "item", "shape", "layout" and "icon".
 */
#define ITEM_COMMON_DEFINITIONS( ItemClass, classType )																					\
Visualization::InteractionHandler* ItemClass::handler_ = Visualization::InteractionHandler::instance();					\
																																							\
void ItemClass::setStyle(const Visualization::ItemStyle* style_)																		\
{																																							\
	if (style_ == style()) return;																												\
	const StyleType* s = dynamic_cast<const StyleType*> (style_);																		\
	if (!s) throw Visualization::VisualizationException("Invalid style type when calling " #ItemClass "::setStyle");	\
	Item::setStyle(s);																																\
}																																							\
																																							\
const QString& ItemClass::className()																											\
{																																							\
	static QString name(#ItemClass);																												\
	return name;																																		\
}																																							\
																																							\
Visualization::StyleSet<ItemClass>& ItemClass::itemStyles()																				\
{																																							\
	static Visualization::StyleSet<ItemClass> styles(QString(classType));															\
	return styles;																																		\
}
/**********************************************************************************************************************/

#endif /* ITEMMACROS_H_ */
