/***********************************************************************************************************************
 * TestRunner.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "TestRunner.h"
#include <QApplication>
#include "EnvisionException.h"
#include "TestEvent.h"

namespace Envision {

TestRunner::TestRunner() :
	pm(NULL)
{
}

void TestRunner::enqueueSelfTests(PluginManager& pm)
{
	this->pm = &pm;

	QStringList arguments = qApp->arguments();

	bool tests = false;
	for (QStringList::iterator arg = arguments.begin(); arg != arguments.end(); arg++)
	{
		if ( *arg == "--test" )
		{
			tests = true;
			continue;
		}

		if ( arg->startsWith("--") ) tests = false;

		if ( tests )
		{
			QStringList parts = arg->split(":");

			if ( parts.length() > 2 ) throw EnvisionException("Incorrectly specified test target and/or test id.");

			QString target = parts[0];
			QString id = QString::null;
			if ( parts.length() > 1 ) id = parts[1];

			if (pm.isPluginLoaded(target) )
			{
				qApp->postEvent(this, new TestEvent(target, id));
			}
			else throw EnvisionException("A test was requested for a plugin which is not loaded: " + target);
		}
	}
}

void TestRunner::customEvent( QEvent * event )
{
	TestEvent* ev = dynamic_cast<TestEvent *>(event);
	if (ev)
	{
		pm->getLoadedPluginInterface(ev->getTarget())->selfTest(ev->getId());
	}
}

}
