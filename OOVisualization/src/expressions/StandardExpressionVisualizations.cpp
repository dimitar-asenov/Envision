/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
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

#include "VisualizationBase/src/items/Static.h"

#define BEGIN_STANDARD_EXPRESSION_VISUALIZATION_ALL(apiSpecification, className, nodeType)									\
ITEM_COMMON_DEFINITIONS(className, "item")																									\
																																							\
className::className(::Visualization::Item* parent, NodeType* node, const StyleType* style)									\
	: Super(parent, node, style){}																												\
																																							\
void className::determineChildren()																												\
{																																							\
	int index = 0;																																		\

//********************************************************************************************************************

#define BEGIN_STANDARD_EXPRESSION_VISUALIZATION(apiSpecification, className, nodeType)											\
BEGIN_STANDARD_EXPRESSION_VISUALIZATION_ALL(apiSpecification, className, nodeType)												\
	const ::OOVisualization::OperatorStyle* opStyle = style();																			\
	layout()->setStyle( &opStyle->layout());																									\

//********************************************************************************************************************

#define BEGIN_STANDARD_ENUMERATION_EXPRESSION_VISUALIZATION(apiSpecification, className, nodeType, enumeration)		\
BEGIN_STANDARD_EXPRESSION_VISUALIZATION_ALL(apiSpecification, className, nodeType)												\
																																							\
	const ::OOVisualization::OperatorStyle* opStyle = &style()->op( node()->enumeration() );									\
	layout()->setStyle( &opStyle->layout());																									\

//********************************************************************************************************************

#define BEGIN_STANDARD_FLAG_EXPRESSION_VISUALIZATION(apiSpecification, className, nodeType, flag)							\
BEGIN_STANDARD_EXPRESSION_VISUALIZATION_ALL(apiSpecification, className, nodeType)												\
																																							\
	int f = node()->flag();																															\
	Q_ASSERT(f);																																		\
	int i = 0;																																			\
	while ( !(f&1) ) {++i; f >>= 1;}																												\
	Q_ASSERT(f==1);																																	\
	const ::OOVisualization::OperatorStyle* opStyle = &style()->op( i );																\
	layout()->setStyle( &opStyle->layout());
//********************************************************************************************************************

#define OPERAND(name)																																\
layout()->synchronizeMid(name##_, node()->name(), index);																				\
if (name##_) ++index;																																\

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

namespace OOVisualization {
#include "StandardExpressionDefinitions.h"
}

#undef BEGIN_STANDARD_EXPRESSION_VISUALIZATION_ALL
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
