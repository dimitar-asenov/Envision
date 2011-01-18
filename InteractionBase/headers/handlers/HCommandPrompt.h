/***********************************************************************************************************************
 * HCommandPrompt.h
 *
 *  Created on: Jan 18, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef HCOMMANDPROMPT_H_
#define HCOMMANDPROMPT_H_

#include "../interactionbase_api.h"

#include "GenericHandler.h"

namespace Interaction {

class INTERACTIONBASE_API HCommandPrompt: public GenericHandler
{
	protected:
	HCommandPrompt();

	public:
		static HCommandPrompt* instance();

		virtual void keyReleaseEvent(Visualization::Item *target, QKeyEvent *event);
};

}

#endif /* HCOMMANDPROMPT_H_ */
