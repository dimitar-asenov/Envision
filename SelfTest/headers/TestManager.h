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
 * TestManager.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TESTMANAGER_H_
#define TESTMANAGER_H_

#include "selftest_api.h"
#include "Test.h"
#include "TestResults.h"

#include "Core/headers/global.h"

#include <QtCore/QMap>

namespace SelfTest {

/**
 * The TestManager class manages test registrations and execution for each plug-in.
 *
 * @param T
 * 				The plug-in class for this plug-in. This is the class that implements the EnvisionPlugin interface.
 * 				It is necessary that each plug-in has its own instantiation of the TestManager, since this manager
 * 				contains a list of all test cases that the plug-in has.
 */
template<class T>
class TestManager
{
	private:
		/**
		 * This is a static class. Object of this class can not be created.
		 */
		TestManager()
		{
		}

		/**
		 * A list of all test constructors. Test classes are not created unless requested.
		 */
		static QMap<QString, Test::TestConstructor>* testConstructors;

	public:

		/**
		 * Adds a new test to the list of tests.
		 *
		 * @param test
		 * 				The constructor of the test class that implements this test.
		 *
		 * @param name
		 * 				The name of this test.
		 */
		static void add(Test::TestConstructor test, const QString &name)
		{
			static QMap<QString, Test::TestConstructor> constructors;
			testConstructors = &constructors;
			testConstructors->insert(name, test);
		}

		/**
		 * Runs all tests registered for this plug-in.
		 */
		static TestResults runAllTests()
		{
			TestResults testRes;

			if (testConstructors)
			{
				for (QMap<QString, Test::TestConstructor>::iterator testConstructor = testConstructors->begin(); testConstructor != testConstructors->end(); testConstructor++)
				{
					Test* test = (*testConstructor)();
					test->run(testRes);
					SAFE_DELETE(test);
				}
			}

			return testRes;
		}

		/**
		 * Runs the specified test for this plug-in.
		 *
		 * @param name
		 * 				The string id of the test to run.
		 */
		static TestResults runTest(const QString &name)
		{
			TestResults testRes;
			if (testConstructors->contains(name))
			{
				Test* test = testConstructors->value(name)();
				test->run(testRes);
				SAFE_DELETE(test);
			}

			return testRes;
		}
};
template<class T> QMap<QString, Test::TestConstructor>* TestManager<T>::testConstructors = nullptr;

}

#endif /* TESTMANAGER_H_ */
