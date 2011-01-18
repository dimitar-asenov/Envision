/***********************************************************************************************************************
 * CommandError.cpp
 *
 *  Created on: Jan 17, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "commands/CommandError.h"

namespace Interaction {

CommandError::CommandError(const QString& message, Visualization::Item* visualization) :
	message_(message), visualization_(visualization)
{
}

CommandError::~CommandError()
{
	// Do not delete this, this is the responsibility of the object handling this error.
	visualization_ = NULL;
}

}
