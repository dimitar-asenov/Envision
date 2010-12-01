/***********************************************************************************************************************
 * apideptest.cpp
 *
 *  Created on: Nov 1, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "apideptest.h"
#include "HelloWorld/headers/HelloWorldPrinter.h"
#include <QString>

Q_EXPORT_PLUGIN2( apideptest, APIDepTest::APIDep )

namespace APIDepTest {

bool APIDep::initialize(Envision::EnvisionManager& em)
{
	QString size = QString::number(em.getAllLoadedPluginsInfo().length());
	Hello::HelloWorldPrinter::sayHello("the initialization method of the APIDepTest plugin. The number of loaded plug-ins so far is " + size);

	return true;
}

void APIDep::selfTest(QString)
{
}

}
