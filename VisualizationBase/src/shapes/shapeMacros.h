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
 * Defines standard methods that every shape should implement. These include class name information and style handling.
 *
 * @param ShapeClass
 * 			The name of the class being defined. This class must inherit from from Shape, directly or indirectly.
 *
 * @param StyleTypeName
 * 			The name of the style type for this class
 */
#define SHAPE_COMMON_CUSTOM_STYLENAME( ShapeClass, StyleTypeName)																		\
DECLARE_TYPE_ID																																		\
public:																																					\
	typedef StyleTypeName StyleType;																												\
																																							\
	const StyleType* style() const { return static_cast<const StyleType*> (Shape::style()); }									\
	virtual void setStyle(const Visualization::ShapeStyle* shapeStyle) override;															\
	static StyleSet<ShapeClass>& itemStyles();																								\
																																							\
private:

/**
 * Defines standard methods that every shape should implement. These include class name information and style handling.
 *
 * @param ShapeClass
 * 			The name of the class being defined. This class must inherit from from Shape, directly or indirectly.
 */
#define SHAPE_COMMON( ShapeClass ) SHAPE_COMMON_CUSTOM_STYLENAME(ShapeClass, ShapeClass##Style)

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
DEFINE_TYPE_ID_DERIVED(ShapeClass, #ShapeClass, )																							\
																																							\
void ShapeClass::initType()																														\
{																																							\
	Shape::registerShape<ShapeClass>();																											\
}																																							\
																																							\
void ShapeClass::setStyle(const Visualization::ShapeStyle* shapeStyle)																	\
{																																							\
	if (shapeStyle == style()) return;																												\
	const StyleType* s = dynamic_cast<const StyleType*> (shapeStyle);																		\
	if (!s) throw Visualization::VisualizationException{"Invalid style type when calling " #ShapeClass "::setStyle"};	\
	Shape::setStyle(s);																																\
}																																							\
																																							\
StyleSet<ShapeClass>& ShapeClass::itemStyles()																								\
{																																							\
	static StyleSet<ShapeClass> styles(QString{classType});																				\
	return styles;																																		\
}
