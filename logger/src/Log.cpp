/***********************************************************************************************************************
 * Log.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Log.h"
#include "DefaultLog.h"

namespace Logger {

Log* Log::getLogger(QString requestingPluginId)
{
	return new DefaultLog(requestingPluginId);
}

}
