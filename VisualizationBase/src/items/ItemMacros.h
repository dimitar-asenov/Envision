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
DECLARE_TYPE_ID																																		\
public:																																					\
	typedef StyleTypeName StyleType;																												\
																																							\
	const StyleType* style() const { return static_cast<const StyleType*> (Item::style()); }									\
	virtual void setStyle(const Visualization::ItemStyle* itemStyle) override;															\
	static Visualization::StyleSet<ItemClass>& itemStyles();																				\
																																							\
	virtual Visualization::InteractionHandler* handler() const override;																\
	static void setDefaultClassHandler(Visualization::InteractionHandler* handler) {defaultClassHandler_ = handler;}	\
	static Visualization::InteractionHandler* defaultClassHandler() {return defaultClassHandler_;}							\
																																							\
	virtual QList<Visualization::VisualizationAddOn*> addOns() override;																\
	static void addAddOn(Visualization::VisualizationAddOn* addOn);																	\
	static bool removeAddOn(Visualization::VisualizationAddOn* addOn);																\
private:																																					\
	static Visualization::InteractionHandler* defaultClassHandler_;																	\
	static QList<Visualization::VisualizationAddOn*>& staticAddOns();

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
 * @param classType
 * 			The class type of this item. This determines the folder inside the root 'styles' folder that will be used to
 * 			look for this item's style directory. Typical values are "item", "shape", "layout" and "icon".
 */
#define ITEM_COMMON_DEFINITIONS( ItemClass, classType )																					\
DEFINE_TYPE_ID_DERIVED(ItemClass, #ItemClass, )																								\
																																							\
void ItemClass::initType() { Super::initType(); }																							\
																																							\
Visualization::InteractionHandler* ItemClass::defaultClassHandler_ =	 nullptr;													\
																																							\
void ItemClass::setStyle(const Visualization::ItemStyle* itemStyle)																		\
{																																							\
	Q_ASSERT(itemStyle);																																	\
	if (itemStyle == style()) return;																												\
	const StyleType* s = dynamic_cast<const StyleType*> (itemStyle);																		\
	if (!s) throw Visualization::VisualizationException{"Invalid style (" + QString{typeid(*itemStyle).name()}				\
		+ ") type when calling " #ItemClass "::setStyle"};																					\
	Item::setStyle(s);																																\
}																																							\
																																							\
Visualization::InteractionHandler* ItemClass::handler() const																			\
{																																							\
	if (defaultClassHandler()) return defaultClassHandler();																				\
	return Super::handler();																														\
}																																							\
																																							\
Visualization::StyleSet<ItemClass>& ItemClass::itemStyles()																				\
{																																							\
	static Visualization::StyleSet<ItemClass> styles(QString{classType});															\
	return styles;																																		\
}																																							\
																																							\
void ItemClass::addAddOn(Visualization::VisualizationAddOn* addOn)																	\
{																																							\
	if (!staticAddOns().contains(addOn))																										\
		staticAddOns().append(addOn);																												\
}																																							\
																																							\
bool ItemClass::removeAddOn(Visualization::VisualizationAddOn* addOn)																\
{																																							\
	return staticAddOns().removeAll(addOn) != 0;																								\
}																																							\
																																							\
QList<Visualization::VisualizationAddOn*>& ItemClass::staticAddOns()																	\
{																																							\
	static QList<Visualization::VisualizationAddOn*> addons;																				\
	return addons;																																		\
}																																							\
																																							\
QList<Visualization::VisualizationAddOn*> ItemClass::addOns()																			\
{																																							\
	QList<Visualization::VisualizationAddOn*> list(Super::addOns());																	\
	list.append(staticAddOns());																													\
	return list;																																		\
}																																							\
