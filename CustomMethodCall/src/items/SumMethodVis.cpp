/***********************************************************************************************************************
 * SumMethodVis.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "items/SumMethodVis.h"

#include "VisualizationBase/headers/items/Symbol.h"

using namespace Visualization;
using namespace OOModel;

namespace CustomMethodCall {

ITEM_COMMON_DEFINITIONS(SumMethodVis, "item")

SumMethodVis::SumMethodVis(Item* parent, NodeType* node, const StyleType* style) :
	ItemWithNode<LayoutProvider<>, MethodCallExpression>(parent, node, style),
	name_(new Symbol(NULL, &style->name()) ),
	prefix_(NULL),
	from_(NULL),
	to_(NULL),
	arguments_(new SequentialLayout(NULL, &style->arguments()))
{
	arguments_->append(name_);
	layout()->append(arguments_);
}

SumMethodVis::~SumMethodVis()
{
	// These were automatically deleted by LayoutProvider's destructor
	name_ = NULL;
	prefix_ = NULL;
	arguments_ = NULL;
}

void SumMethodVis::determineChildren()
{
	Model::Node* fromNode = NULL;
	if ( node()->arguments()->size() > 0 ) fromNode = node()->arguments()->at(0);

	arguments_->synchronizeLast(from_, fromNode);

	Model::Node* toNode = NULL;
	if ( node()->arguments()->size() > 1 ) toNode = node()->arguments()->at(1);
	arguments_->synchronizeFirst(to_, toNode);

	layout()->synchronizeLast(prefix_, node()->prefix());

	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle( &style()->layout());
	name_->setStyle( &style()->name());
	arguments_->setStyle( &style()->arguments() );
}

}
