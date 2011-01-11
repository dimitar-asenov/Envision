/***********************************************************************************************************************
 * interactionbase.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "interactionbase.h"

Q_EXPORT_PLUGIN2( interactionbase, Interaction::InteractionBase )

namespace Interaction {

bool InteractionBase::initialize(Envision::EnvisionManager&)
{
	return true;
}

void InteractionBase::selfTest(QString)
{
}

}
