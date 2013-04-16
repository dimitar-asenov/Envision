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

#include "expressions/VArrayInitializer.h"

#include "VisualizationBase/src/layouts/GridLayout.h"
#include "VisualizationBase/src/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VArrayInitializer, "item")

VArrayInitializer::VArrayInitializer(Item* parent, NodeType* node, const StyleType* style) :
	BaseItemType(parent, node, style),
	values_(nullptr),
	matrixForm_(false)
{
}

VArrayInitializer::~VArrayInitializer()
{
	// These were automatically deleted by LayoutProvider's destructor
	values_ = nullptr;
}

void VArrayInitializer::determineChildren()
{
	// Automatically switch to matrix form if there are nested ArrayInitializers
	if (node()->values()->size()  > 0 && dynamic_cast<ArrayInitializer*>(node()->values()->at(0)))
		matrixForm_ = true;
	else
		matrixForm_ = false;



	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());

	if (viewFormSwitched())
	{
		layout()->clear();

		if (matrixForm_) values_ = nullptr; // this was deleted by the clear() operation above
		else
		{
			layout()->setGridSize(1,1, false);
			values_ = new VList(layout(), node()->values(), &style()->values());
			layout()->set(values_,0,0, true);
		}
	}

	if (matrixForm_)
	{
		useShape();

		QList< QList< Model::Node*> > nodes;
		for(int y = 0; y<node()->values()->size(); ++y)
		{
			nodes.append( QList< Model::Node*>() );

			ArrayInitializer* column = dynamic_cast< ArrayInitializer* >(node()->values()->at(y));
			if (column)
			{
				for(int x = 0; x < column->values()->size(); ++x)
					nodes.last().append(column->values()->at(x));
			}
			else nodes.last().append( node()->values()->at(y) );
		}

		layout()->synchronizeWithNodes(nodes, renderer());
	}
	else
	{
		removeShape();

		layout()->synchronize(values_, node()->values(), &style()->values(), 0,0);
		values_->setStyle( &style()->values() );
		values_->setSuppressDefaultRemovalHandler(true);
	}
}

void VArrayInitializer::showInMatrixForm(bool matrixForm)
{
	if ( matrixForm != matrixForm_ )
	{
		matrixForm_ = matrixForm;
		setUpdateNeeded(FullUpdate);
	}
}

bool VArrayInitializer::viewFormSwitched() const
{
	return matrixForm_ == (values_ != nullptr) || layout()->gridSize().width() == 0;
}

}
