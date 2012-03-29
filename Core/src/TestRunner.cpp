/***********************************************************************************************************************
**
** Copyright (c) 2011, ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
**    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
**      following disclaimer in the documentation and/or other materials provided with the distribution.
**    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
**      derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

/***********************************************************************************************************************
 * TestRunner.cpp
 *
 *  Created on: Nov 2, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "TestRunner.h"
#include "TestEvent.h"
#include "EnvisionException.h"

namespace Envision {

TestRunner::TestRunner() :
	pm(nullptr)
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
		QTextStream out(stdout);
		if ( ev->getId().isEmpty())
			out << "Running all tests for plug-in " + ev->getTarget() << endl;
		else
			out << "Running test '" << ev->getId() << "' for plug-in " << ev->getTarget() << endl;
		pm->getLoadedPluginInterface(ev->getTarget())->selfTest(ev->getId());
	}
}

}
