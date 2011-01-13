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
template<class T> QMap<QString, Test::TestConstructor>* TestManager<T>::testConstructors = NULL;

}

#endif /* TESTMANAGER_H_ */
