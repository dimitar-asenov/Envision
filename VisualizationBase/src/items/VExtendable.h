/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#include "../visualizationbase_api.h"

#include "ItemWithNode.h"
#include "VExtendableStyle.h"
#include "../layouts/SequentialLayout.h"
#include "../layouts/PanelBorderLayout.h"
#include "ModelBase/src/nodes/Extendable/ExtendableNode.h"

namespace Visualization {

class VISUALIZATIONBASE_API VExtendable : public ItemWithNode<VExtendable, Item, Model::ExtendableNode>
{
	ITEM_COMMON(VExtendable)

	public:
		VExtendable(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());
		virtual ~VExtendable();

		void setExpanded(bool expanded = true);
		bool expanded() const;

	protected:
		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);

	private:
		typedef ItemWithNode<VExtendable, Item, Model::ExtendableNode> BaseItemType;

		SequentialLayout* header;
		PanelBorderLayout* layout; //only used when expanded
		SequentialLayout* attributes; //only used when expanded

		bool expanded_;
		bool expandedSwtiched() const;

};

inline bool VExtendable::expanded() const { return expanded_; }

}
