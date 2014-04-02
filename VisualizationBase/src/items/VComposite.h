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

#include "../visualizationbase_api.h"

#include "ItemWithNode.h"
#include "VCompositeStyle.h"
#include "../declarative/DeclarativeItem.h"
#include "ModelBase/src/nodes/composite/CompositeNode.h"

namespace Visualization {

class EmptyItem;
class Text;

class VISUALIZATIONBASE_API VComposite
: public Super<ItemWithNode<VComposite, DeclarativeItem<VComposite>, Model::CompositeNode>>
{
	ITEM_COMMON(VComposite)

	public:
		VComposite(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());

		void setExpanded(bool expanded = true);
		bool expanded() const;

		static void initializeForms();
		int determineForm() override;

	private:
		Item* name_{};
		Text* typeName_{};
		EmptyItem* headerBackground_{};

		bool expanded_{};

		Model::Node* nameNode();

		QList<Item*> attributes();
		QList<Item*> attributes_;
};

inline bool VComposite::expanded() const { return expanded_; }

}
