/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
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
 **********************************************************************************************************************/

#include "StandardExpressionVisualizations.h"

#include "../OOVisualizationException.h"

#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/NodeWrapper.h"

#define BEGIN_STANDARD_EXPRESSION_VISUALIZATION_BASE(apiSpecification, className, nodeType)									\
ITEM_COMMON_DEFINITIONS(className, "item")																									\
																																							\
className::className(::Visualization::Item* parent, NodeType* node, const StyleType* style)									\
	: Super{parent, node, style}{}																												\
																																							\
void className::determineChildren()																												\
{																																							\
	int index = 0;																																		\

//********************************************************************************************************************

#define BEGIN_STANDARD_EXPRESSION_VISUALIZATION(apiSpecification, className, nodeType)											\
BEGIN_STANDARD_EXPRESSION_VISUALIZATION_BASE(apiSpecification, className, nodeType)												\
	const ::OOVisualization::OperatorStyle* opStyle = style();																			\
	layout()->setStyle( &opStyle->layout());																									\

//********************************************************************************************************************

#define BEGIN_STANDARD_ENUMERATION_EXPRESSION_VISUALIZATION(apiSpecification, className, nodeType, enumeration)		\
BEGIN_STANDARD_EXPRESSION_VISUALIZATION_BASE(apiSpecification, className, nodeType)												\
																																							\
	const ::OOVisualization::OperatorStyle* opStyle = &style()->op( (int) node()->enumeration() );							\
	layout()->setStyle( &opStyle->layout());																									\

//********************************************************************************************************************

#define BEGIN_STANDARD_FLAG_EXPRESSION_VISUALIZATION(apiSpecification, className, nodeType, flag)							\
BEGIN_STANDARD_EXPRESSION_VISUALIZATION_BASE(apiSpecification, className, nodeType)												\
																																							\
	int f = node()->flag();																															\
	if (!f) throw new ::OOVisualization::OOVisualizationException{"Visualization does not have a flag set."};			\
	int i = 0;																																			\
	while ( !(f&1) ) {++i; f >>= 1;}																												\
	if (f!=1) throw new ::OOVisualization::OOVisualizationException{"Visualization has more than 1 flag set."};			\
	const ::OOVisualization::OperatorStyle* opStyle = &style()->op( i );																\
	layout()->setStyle( &opStyle->layout());
//********************************************************************************************************************

#define OPERAND(name)																																\
layout()->synchronizeMid(name##_, node()->name(), index);																				\
if (name##_) ++index;																																\

//********************************************************************************************************************

#define WRAPPED_OPERAND(name, wrapId)																											\
layout()->synchronizeMid<::Visualization::Item, ::Visualization::NodeWrapper>														\
									(name##_, node()->name(), &opStyle->operand##wrapId##Wrapper(), index);							\
if (name##_) {																																			\
	++index;																																				\
	name##_->setStyle( &opStyle->operand##wrapId##Wrapper() );				 															\
}																																							\

//********************************************************************************************************************

#define PREINPOSTFIX(name, condition, styleAttribute)																						\
layout()->synchronizeMid(name##_, condition, &opStyle->styleAttribute(), index);													\
if (name##_)																																			\
{																																							\
	++index;																																				\
	name##_->setStyle( &opStyle->styleAttribute());																							\
}																																							\

//********************************************************************************************************************

#define END_STANDARD_EXPRESSION_VISUALIZATION }
#define PREFIX(condition) PREINPOSTFIX(prefix, condition, preSymbol)
#define INFIX(condition) PREINPOSTFIX(infix, condition, inSymbol)
#define INFIX2(condition) PREINPOSTFIX(infix2, condition, in2Symbol)
#define POSTFIX(condition) PREINPOSTFIX(postfix, condition, postSymbol)

//********************************************************************************************************************

namespace OOVisualization {
#include "StandardExpressionDefinitions.h"
}

#undef BEGIN_STANDARD_EXPRESSION_VISUALIZATION_BASE
#undef BEGIN_STANDARD_EXPRESSION_VISUALIZATION
#undef BEGIN_STANDARD_ENUMERATION_EXPRESSION_VISUALIZATION
#undef BEGIN_STANDARD_FLAG_EXPRESSION_VISUALIZATION
#undef END_STANDARD_EXPRESSION_VISUALIZATION
#undef PREINPOSTFIX
#undef PREFIX
#undef INFIX
#undef INFIX2
#undef POSTFIX
#undef OPERAND
#undef WRAPPED_OPERAND
