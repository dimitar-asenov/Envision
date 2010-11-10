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
#include <QMap>

namespace SelfTest {

template<class T>
class TestManager
{
	private:
		TestManager()
		{
		}
		static QMap<QString, Test::TestConstructor>* testConstructors;

	public:

		static void add(Test::TestConstructor test, const QString &name)
		{
			static QMap<QString, Test::TestConstructor> constructors;
			testConstructors = &constructors;
			testConstructors->insert(name, test);
		}

		static TestResults runAllTests()
		{
			TestResults testRes;
			for (QMap<QString, Test::TestConstructor>::iterator testConstructor = testConstructors->begin(); testConstructor != testConstructors->end(); testConstructor++)
			{
				Test* test = (*testConstructor)();
				test->run(testRes);
				delete test;
			}

			return testRes;
		}

		static TestResults runTest(const QString &name)
		{
			TestResults testRes;
			if (testConstructors->contains(name))
			{
				Test* test = testConstructors->value(name)();
				test->run(testRes);
				delete test;
			}

			return testRes;
		}
};
template<class T> QMap<QString, Test::TestConstructor>* TestManager<T>::testConstructors = NULL;

}

#endif /* TESTMANAGER_H_ */
