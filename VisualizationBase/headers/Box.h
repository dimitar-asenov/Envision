/***********************************************************************************************************************
 * Box.h
 *
 *  Created on: Dec 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BOX_H_
#define BOX_H_

#include "Item.h"
#include "layouts/SequentialLayout.h"
#include "shapes/ShapeExperiment.h"

namespace Visualization {

class Box: public ShapeExperiment<Item>
{
	private:
		SequentialLayoutStyle style;
		SequentialLayout items;

	public:
		Box(Item* parent, int sub);

		void determineChildren();
		void updateState();
};

}

#endif /* BOX_H_ */
