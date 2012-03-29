/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2012 ETH Zurich
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

/*
 * VConditionalExpression.h
 *
 *  Created on: Mar 27, 2012
 *      Author: Dimitar Asenov
 */

#ifndef OOVisualization_VCONDITIONALEXPRESSION_H_
#define OOVisualization_VCONDITIONALEXPRESSION_H_

#include "../oovisualization_api.h"
#include "OperatorStyle.h"

#include "OOModel/src/expressions/ConditionalExpression.h"

#include "VisualizationBase/src/items/ItemWithNode.h"
#include "VisualizationBase/src/items/LayoutProvider.h"

namespace Visualization {
	class Static;
}

namespace OOVisualization {

class OOVISUALIZATION_API VConditionalExpression : public Visualization::ItemWithNode<Visualization::LayoutProvider<>,
	OOModel::ConditionalExpression>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VConditionalExpression, OperatorStyle)

	public:
		VConditionalExpression(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VConditionalExpression();

	protected:
		void determineChildren();

	private:
		Visualization::Static* pre_;
		Visualization::Static* in_;
		Visualization::Static* in2_;
		Visualization::Static* post_;
		Visualization::Item* condition_;
		Visualization::Item* true_;
		Visualization::Item* false_;
};

} /* namespace OOVisualization */
#endif /* OOVisualization_VCONDITIONALEXPRESSION_H_ */
