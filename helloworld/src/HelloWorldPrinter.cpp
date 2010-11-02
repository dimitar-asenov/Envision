/***********************************************************************************************************************
 * HelloWorldPrinter
 *
 *  Created on: Nov 1, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "HelloWorldPrinter.h"
#include <iostream>

namespace Hello {

void HelloWorldPrinter::sayHello(QString fromWho)
{
	std::cout<< "Hello World from " << qPrintable(fromWho) << '.' << std::endl;
}

}
