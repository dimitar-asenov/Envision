/***********************************************************************************************************************
 * HList.h
 *
 *  Created on: Jan 26, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef HLIST_H_
#define HLIST_H_

#include "../interactionbase_api.h"

#include "GenericHandler.h"

namespace Interaction {

class INTERACTIONBASE_API HList : public GenericHandler
{
	public:
		virtual void keyPressEvent(Visualization::Item *target, QKeyEvent *event);
		virtual void focusInEvent(Visualization::Item *target, QFocusEvent *event);

		static HList* instance();

	protected:
		HList();
};

}

#endif /* HLIST_H_ */
