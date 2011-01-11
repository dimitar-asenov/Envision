/***********************************************************************************************************************
 * PLUGINNAME_LOWERCASE.cpp
 *
 *  Created on: DATE
 *      Author: AUTHOR
 **********************************************************************************************************************/

#include "PLUGINNAME_LOWERCASE.h"

Q_EXPORT_PLUGIN2( PLUGINNAME_LOWERCASE, NAMESPACE::PLUGINNAME )

namespace NAMESPACE {

bool PLUGINNAME::initialize(Envision::EnvisionManager&)
{
	return true;
}

void PLUGINNAME::selfTest(QString testId)
{
}

}
