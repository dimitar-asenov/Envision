/***********************************************************************************************************************
 * BoxTest.h
 *
 *  Created on: Dec 13, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef BOXTEST_H_
#define BOXTEST_H_

#include "items/Item.h"
#include "layouts/SequentialLayoutStyle.h"
#include "layouts/SequentialLayout.h"
#include "shapes/BoxStyle.h"

namespace Visualization {

class BoxTest : public Item
{
	private:
		SequentialLayout items;

	public:
		BoxTest(Item* parent, int sub);

		void determineChildren();
		void updateGeometry(int availableWidth, int availableHeight);
};

}

#endif /* BOXTEST_H_ */
