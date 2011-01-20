/***********************************************************************************************************************
 * SelectedItemStyle.h
 *
 *  Created on: Jan 20, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef SELECTEDITEMSTYLE_H_
#define SELECTEDITEMSTYLE_H_

#include "../visualizationbase_api.h"

#include "ItemStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API SelectedItemStyle : public ItemStyle
{
	public:
		virtual void load();

		static SelectedItemStyle* getDefault();
};

}

#endif /* SELECTEDITEMSTYLE_H_ */
