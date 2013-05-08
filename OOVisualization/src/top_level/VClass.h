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

#include "../oovisualization_api.h"
#include "VClassStyle.h"

#include "OOModel/src/top_level/Class.h"

#include "VisualizationBase/src/items/ItemWithNode.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.h"

namespace Visualization {
	class VText;
	class VList;
	class Static;
	class PositionLayout;
	class NodeWrapper;
}

namespace Model {
	class Node;
}

namespace OOVisualization {
class VStatementItemList;

class OOVISUALIZATION_API VClass
: public Super<Visualization::ItemWithNode<VClass, Visualization::DeclarativeItem<VClass>, OOModel::Class>>
{
	ITEM_COMMON(VClass)

	public:
		VClass(Item* parent, NodeType* node, const StyleType* style = itemStyles().get());

		static void initializeForms();
		int determineForm() override;
		void determineChildren() override;

	private:
		Visualization::Static* icon_{};
		Visualization::VText* name_{};
		Visualization::VList* typeArguments_{};
		Visualization::VList* baseClasses_{};
		VStatementItemList* annotations_{};
		Visualization::VList* enumerators_{};
		Visualization::PositionLayout* body_{};
		Visualization::NodeWrapper* fieldBackground_{};
		QList<Model::Node*> publicFields_{};
		QList<Model::Node*> privateFields_{};
		QList<Model::Node*> protectedFields_{};
		QList<Model::Node*> defaultFields_{};
};

}
