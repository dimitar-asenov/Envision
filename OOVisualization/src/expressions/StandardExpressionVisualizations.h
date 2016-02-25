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

#pragma once

#include "OperatorStyle.h"
#include "VExpression.h"

#include "VisualizationBase/src/items/LayoutProvider.h"
#include "OOModel/src/allOOModelNodes.h"

namespace Visualization {
	class Static;
}

#define BEGIN_STANDARD_EXPRESSION_VISUALIZATION_BASE(apiSpecification, className, nodeType, styleTypeName)				\
class apiSpecification className																													\
	: public ::Super<::OOVisualization::VExpression<className, ::Visualization::LayoutProvider<>, nodeType>> {			\
	ITEM_COMMON_CUSTOM_STYLENAME(className, styleTypeName)																				\
																																							\
	public:																																				\
		className(::Visualization::Item* parent, NodeType* node, const StyleType* style = itemStyles().get());			\
	protected:																																			\
		virtual void determineChildren() override;																							\

//********************************************************************************************************************

#define BEGIN_STANDARD_EXPRESSION_VISUALIZATION(apiSpecification, className, nodeType)											\
BEGIN_STANDARD_EXPRESSION_VISUALIZATION_BASE(apiSpecification, className, nodeType, ::OOVisualization::OperatorStyle)
//********************************************************************************************************************

#define BEGIN_STANDARD_ENUMERATION_EXPRESSION_VISUALIZATION(apiSpecification, className, nodeType, enumeration)		\
BEGIN_STANDARD_EXPRESSION_VISUALIZATION_BASE(apiSpecification, className, nodeType,											\
::OOVisualization::OperatorSequenceStyle)
//********************************************************************************************************************

#define BEGIN_STANDARD_FLAG_EXPRESSION_VISUALIZATION(apiSpecification, className, nodeType, flag)							\
BEGIN_STANDARD_EXPRESSION_VISUALIZATION_BASE(apiSpecification, className, nodeType,											\
::OOVisualization::OperatorSequenceStyle)
//********************************************************************************************************************

#define EXPRESSION_PART(type, name)																												\
	public:																																				\
		type* name() const { return name##_; }																									\
	private:																																				\
		type* name##_{};																																\

//********************************************************************************************************************

#define END_STANDARD_EXPRESSION_VISUALIZATION };
#define PREFIX(condition) EXPRESSION_PART(::Visualization::Static, prefix)
#define INFIX(condition) EXPRESSION_PART(::Visualization::Static, infix)
#define INFIX2(condition) EXPRESSION_PART(::Visualization::Static, infix2)
#define POSTFIX(condition) EXPRESSION_PART(::Visualization::Static, postfix)
#define OPERAND(name) EXPRESSION_PART(::Visualization::Item, name)
#define WRAPPED_OPERAND(name, wrapId) OPERAND(name)

//********************************************************************************************************************

namespace OOVisualization {
#include "StandardExpressionDefinitions.h"
}

#undef BEGIN_STANDARD_EXPRESSION_VISUALIZATION_BASE
#undef BEGIN_STANDARD_EXPRESSION_VISUALIZATION
#undef BEGIN_STANDARD_ENUMERATION_EXPRESSION_VISUALIZATION
#undef BEGIN_STANDARD_FLAG_EXPRESSION_VISUALIZATION
#undef END_STANDARD_EXPRESSION_VISUALIZATION
#undef EXPRESSION_PART
#undef PREFIX
#undef INFIX
#undef INFIX2
#undef POSTFIX
#undef OPERAND
#undef WRAPPED_OPERAND
