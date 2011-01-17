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
	SAFE_DELETE_ITEM(visualization_);
}

}
