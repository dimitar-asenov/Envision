/***********************************************************************************************************************
 * HelloWorldPrinter.h
 *
 *  Created on: Nov 1, 2010
 *      Author: Dimitar Asenov
 *
 * This illustrates how to export classes from a plug-in.
 **********************************************************************************************************************/

#ifndef HELLOWORLDPRINTER_H_
#define HELLOWORLDPRINTER_H_

#include "helloworld_api.h"
#include <QtCore/QString>

namespace Hello {

/**
 * Class that contains a static "Hello World" method.
 *
 * This class is exported by the Hello World plug-in. To use it from other plug-ins, simply declare this plug-in as a
 * dependency and include this header file. The libhelloworl.so (helloworld.dll) must be linked in your plug-in when
 * compiling.
 */
class HELLOWORLD_API HelloWorldPrinter
{
	public:
		/**
		 * Prints a "Hello World" message on the standard output, including the fromWho argument.
		 */
		static void sayHello(QString fromWho);
};

}
#endif /* HELLOWORLDPRINTER_H_ */
