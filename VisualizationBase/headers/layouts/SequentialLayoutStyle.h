/***********************************************************************************************************************
 * SequentialLayoutStyle.h
 *
 *  Created on: Dec 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SEQUENTIALLAYOUTSTYLE_H_
#define SEQUENTIALLAYOUTSTYLE_H_

#include "visualizationbase_api.h"

namespace Visualization {

class VISUALIZATIONBASE_API SequentialLayoutStyle
{
	public:
		enum SequntialLayoutDirection { LeftToRight, RightToLeft, TopToBottom, BottomToTop};

	private:
		SequntialLayoutDirection direction;
		int spaceBetweenElements;


	public:

		static SequentialLayoutStyle& getDefault();
};

}

#endif /* SEQUENTIALLAYOUTSTYLE_H_ */
