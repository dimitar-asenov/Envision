/***********************************************************************************************************************
 * VArrayInitializer.cpp
 *
 *  Created on: Mar 21, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/VArrayInitializer.h"

#include "VisualizationBase/headers/layouts/GridLayout.h"
#include "VisualizationBase/headers/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VArrayInitializer, "item")

VArrayInitializer::VArrayInitializer(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode< LayoutProvider<GridLayout>, ArrayInitializer>(parent, node, style),
	values_( NULL ),
	matrixForm_(false)
{
}

VArrayInitializer::~VArrayInitializer()
{
	// These were automatically deleted by LayoutProvider's destructor
	values_ = NULL;
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

		if (matrixForm_) values_ = NULL; // this was deleted by the clear() operation above
		else
		{
			layout()->setGridSize(1,1, false);
			values_ = new VList(NULL, node()->values(), &style()->values());
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
	}
}

void VArrayInitializer::showInMatrixForm(bool matrixForm)
{
	if ( matrixForm != matrixForm_ )
	{
		matrixForm_ = matrixForm;
		setUpdateNeeded();
	}
}

bool VArrayInitializer::viewFormSwitched() const
{
	return matrixForm_ == (values_ != NULL) || layout()->gridSize().width() == 0;
}

}
