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

#include "VArrayInitializer.h"

#include "VisualizationBase/src/layouts/GridLayout.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/declarative/DeclarativeItem.hpp"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

DEFINE_ITEM_COMMON(VArrayInitializer, "item")

VArrayInitializer::VArrayInitializer(Item* parent, NodeType* node, const StyleType* style) : Super{parent, node, style}
{}

void VArrayInitializer::initializeForms()
{
	addForm(item<VList>(&I::list_, [](I* v){return v->node()->values();}, &StyleType::list));
	addForm(item(&I::grid_, &StyleType::grid));
}

int VArrayInitializer::determineForm()
{
	return (node()->values()->size()  > 0 && DCast<ArrayInitializer>(node()->values()->at(0))) ? 1 : 0;
}

void VArrayInitializer::determineChildren()
{
	Super::determineChildren();

	if (list_) list_->setSuppressDefaultRemovalHandler(true);

	if (grid_)
	{
		QList< QList< Model::Node*> > nodes;
		for (auto genericRow : *node()->values())
		{
			nodes.append( QList< Model::Node*>() );

			ArrayInitializer* column = dynamic_cast< ArrayInitializer* >(genericRow);
			if (column)
			{
				for (auto genericColumn : *column->values())
					nodes.last().append(genericColumn);
			}
			else nodes.last().append( genericRow );
		}

		grid_->synchronizeWithNodes(nodes);
	}
}

}
