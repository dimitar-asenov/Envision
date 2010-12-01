/***********************************************************************************************************************
 * helloworld.cpp
 *
 *  Created on: Nov 1, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "helloworld.h"
#include "HelloWorldPrinter.h"

Q_EXPORT_PLUGIN2( helloworld, Hello::HelloWorld )

namespace Hello {



bool HelloWorld::initialize(Envision::EnvisionManager&)
{
	HelloWorldPrinter::sayHello("the initialization of the hello test Envision plugin");
	return true;
}

void HelloWorld::selfTest(QString testId)
{
	QString greeting = (testId == QString::null ? "" : ( ". The selected test id was: " + testId ) );
	HelloWorldPrinter::sayHello("the self test of hello" + greeting);
}

}
