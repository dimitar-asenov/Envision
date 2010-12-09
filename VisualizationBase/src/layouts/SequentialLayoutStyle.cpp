/***********************************************************************************************************************
 * SequentialLayoutStyle.cpp
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "layouts/SequentialLayoutStyle.h"

namespace Visualization {

SequentialLayoutStyle& SequentialLayoutStyle::getDefault()
{
	static SequentialLayoutStyle s;
	return s;
}

}
