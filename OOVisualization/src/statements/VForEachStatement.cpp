/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
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

/***********************************************************************************************************************
 * VForEachStatement.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "statements/VForEachStatement.h"

#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/VList.h"
#include "VisualizationBase/src/items/Static.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VForEachStatement, "item")

VForEachStatement::VForEachStatement(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<PanelBorderLayout>, ForEachStatement>(parent, node, style),
	header(), varContainer(), collectionBackground(), varName(), collection(),	varType(), body()
{
	layout()->setTop(true);
	header = new SequentialLayout(layout()->top(), &style->header());
	layout()->top()->setFirst(header);

	header->append(new Static(header, &style->icon()));
	varContainer = new SequentialLayout(header, &style->varContainer());
	header->append(varContainer);

	varName = new VText(varContainer, node->varNameNode(), &style->varName());
	varContainer->append(varName);

	collectionBackground = new SequentialLayout(header, &style->collection());
	header->append(collectionBackground);
}

VForEachStatement::~VForEachStatement()
{
	// These were automatically deleted by LayoutProvider's destructor
	header = nullptr;
	varContainer = nullptr;
	collectionBackground = nullptr;
	varName = nullptr;
	collection = nullptr;
	varType = nullptr;
	body = nullptr;
}

void VForEachStatement::determineChildren()
{
	layout()->synchronizeContent(body, node()->body(), &style()->body());

	collectionBackground->synchronizeFirst(collection, node()->collection());
	varContainer->synchronizeFirst(varType, node()->varType());
	varContainer->synchronizeLast(varName, node()->varNameNode(), &style()->varName());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle(&style()->layout());
	header->setStyle(&style()->header());
	header->at<Static>(0)->setStyle(&style()->icon());
	varContainer->setStyle( &style()->varContainer() );
	collectionBackground->setStyle( &style()->collection() );
	varName->setStyle( &style()->varName() );
	body->setStyle(&style()->body());
}

}
