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

#pragma once

#include "../oovisualization_api.h"
#include "VisualizationBase/src/items/ItemWithNode.h"

namespace Visualization {
	class InteractionHandler;
}

namespace OOVisualization {

class OOVISUALIZATION_API VStatementItemStaticData
{
	public:
		static void setDefaultClassHandler(Visualization::InteractionHandler* handler);
		static Visualization::InteractionHandler* defaultClassHandler();

	private:
		static Visualization::InteractionHandler* defaultClassHandler_;
};
inline void VStatementItemStaticData::setDefaultClassHandler(Visualization::InteractionHandler* handler)
	{defaultClassHandler_ = handler;}
inline Visualization::InteractionHandler* VStatementItemStaticData::defaultClassHandler()
	{return defaultClassHandler_;}


template <class Derived, class Super, class ContainedNode, bool defaultInitialization = true>
class VStatementItem  : public Visualization::ItemWithNode<Derived, Super, ContainedNode, defaultInitialization> {
	public:
		VStatementItem(Visualization::Item* parent, ContainedNode* node, const typename Super::StyleType* style=nullptr);

		virtual Visualization::InteractionHandler* handler() const override;
};

template <class Derived, class Super, class ContainedNode, bool defaultInitialization>
VStatementItem<Derived,Super,ContainedNode,defaultInitialization>::
	VStatementItem(Visualization::Item* parent, ContainedNode* node, const typename Super::StyleType* style)
	:Visualization::ItemWithNode<Derived, Super, ContainedNode, defaultInitialization>(parent, node, style)
{}

template <class Derived, class Super, class ContainedNode, bool defaultInitialization>
Visualization::InteractionHandler*  VStatementItem<Derived,Super,ContainedNode,defaultInitialization>::handler() const
{
	if (VStatementItemStaticData::defaultClassHandler()) return VStatementItemStaticData::defaultClassHandler();
	return Visualization::ItemWithNode<Derived, Super, ContainedNode, defaultInitialization>::handler();
}

} /* namespace OOVisualization */

