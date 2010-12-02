/***********************************************************************************************************************
 * HelloWorldPrinter
 *
 *  Created on: Nov 1, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "HelloWorldPrinter.h"
#include <QtCore/QTextStream>

namespace Hello {

void HelloWorldPrinter::sayHello(QString fromWho)
{
	QTextStream out(stdout);
	out<< "Hello World from " << fromWho << '.' << endl;
}

}
