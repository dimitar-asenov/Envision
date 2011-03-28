/***********************************************************************************************************************
 * shapeMacros.h
 *
 *  Created on: Dec 21, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SHAPEMACROS_H_
#define SHAPEMACROS_H_

#include "VisualizationBase/headers/styles/StyleSet.h"

/**
 * Defines standard methods that every shape should implement. These include class name information and style handling.
 *
 * @param ShapeClass
 * 			The name of the class being defined. This class must inherit from from Shape, directly or indirectly.
 *
 * @param StyleTypeName
 * 			The name of the style type for this class
 */
#define SHAPE_COMMON_CUSTOM_STYLENAME( ShapeClass, StyleTypeName)																		\
public:																																					\
	typedef StyleTypeName StyleType;																												\
	static const QString& className();																											\
																																							\
	const StyleType* style() const { return static_cast<const StyleType*> (Shape::style()); }									\
	virtual void setStyle(const Visualization::ShapeStyle* style);																		\
	static StyleSet<ShapeClass>& itemStyles();																								\
																																							\
private:
/**********************************************************************************************************************/

/**
 * Defines standard methods that every shape should implement. These include class name information and style handling.
 *
 * @param ShapeClass
 * 			The name of the class being defined. This class must inherit from from Shape, directly or indirectly.
 */
#define SHAPE_COMMON( ShapeClass ) SHAPE_COMMON_CUSTOM_STYLENAME(ShapeClass, ShapeClass##Style)
/**********************************************************************************************************************/

/**
 * Defines standard methods and members that every shape should implement.
 *
 * @param ShapeClass
 * 			The name of the class being defined.
 * @param classType
 * 			The class type of this shape. This determines the folder inside the root 'styles' folder that will be used
 * 			to look for this shape's style directory. Typical values are "item", "shape", "layout" and "icon".
 */
#define SHAPE_COMMON_DEFINITIONS( ShapeClass, classType )																				\
void ShapeClass::setStyle(const Visualization::ShapeStyle* style_)																	\
{																																							\
	if (style_ == style()) return;																												\
	const StyleType* s = dynamic_cast<const StyleType*> (style_);																		\
	if (!s) throw Visualization::VisualizationException("Invalid style type when calling " #ShapeClass "::setStyle");	\
	Shape::setStyle(s);																																\
}																																							\
																																							\
const QString& ShapeClass::className()																											\
{																																							\
	static QString name(#ShapeClass);																											\
	return name;																																		\
}																																							\
																																							\
StyleSet<ShapeClass>& ShapeClass::itemStyles()																								\
{																																							\
	static StyleSet<ShapeClass> styles(QString(classType));																				\
	return styles;																																		\
}
/**********************************************************************************************************************/

#endif /* SHAPEMACROS_H_ */
