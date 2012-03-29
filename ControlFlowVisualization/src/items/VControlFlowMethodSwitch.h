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
 * VControlFlowMethodSwitch.h
 *
 *  Created on: Mar 1, 2012
 *      Author: Dimitar Asenov
 */

#ifndef ControlFlowVisualization_VCONTROLFLOWMETHODSWITCH_H_
#define ControlFlowVisualization_VCONTROLFLOWMETHODSWITCH_H_

#include "../controlflowvisualization_api.h"

#include "OOModel/src/top_level/Method.h"

#include "VisualizationBase/src/items/ItemWithNode.h"
#include "VisualizationBase/src/items/ItemStyle.h"
#include "VisualizationBase/src/items/Item.h"

namespace OOVisualization {
	class VMethod;
}

namespace ControlFlowVisualization {

class VMethodCF;

class CONTROLFLOWVISUALIZATION_API VControlFlowMethodSwitch
	: public Visualization::ItemWithNode<Visualization::Item, OOModel::Method>
{
	ITEM_COMMON_CUSTOM_STYLENAME(VControlFlowMethodSwitch, Visualization::ItemStyle)

	public:
		VControlFlowMethodSwitch(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());

		virtual bool sizeDependsOnParent() const;

		bool isShownAsControlFlow() const;
		void setShowAsControlFlow(bool showAsControlFlow);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		bool showAsControlFlow_;
		VMethodCF* metCF_;
		OOVisualization::VMethod* met_;

		bool isRenderingChanged() const;
};

inline bool VControlFlowMethodSwitch::isShownAsControlFlow() const { return showAsControlFlow_; }
inline void VControlFlowMethodSwitch::setShowAsControlFlow(bool showAsControlFlow)
	{ showAsControlFlow_ = showAsControlFlow; setUpdateNeeded(); }

} /* namespace ControlFlowVisualization */
#endif /* ControlFlowVisualization_VCONTROLFLOWMETHODSWITCH_H_ */
