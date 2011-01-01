/***********************************************************************************************************************
 * Test.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TEST_H_
#define TEST_H_

#include "selftest_api.h"
#include "TestResults.h"
#include <QtCore/QString>

namespace SelfTest {

template<class T> class TestManager;

/**
 * The Test class is the base class for a single test for the Envision testing framework.
 */
class SELFTEST_API Test
{
	private:
		QString name;

	public:
		typedef Test* (*TestConstructor)();

		Test(const QString &name);
		virtual ~Test();

		const QString& getName() const;

		/**
		 * Runs the test and reports any failures to the specified test results.
		 *
		 * @param testResults
		 * 				The outcome of the test is saved in this object.
		 */
		void run(TestResults& testResults);

		/**
		 * Implements custom test behavior.
		 *
		 * This function must be implemented for each test.
		 *
		 * @param testResults
		 *					Test failures are added here.
		 *	@param passed
		 *					must be set to false if the test failed.
		 */
		virtual void runCustom(TestResults& testResults, bool &passed) = 0;
};

}

/**
 * The TEST macro is used to define a new test.
 *
 * @param pluginClass
 * 				the name of the class of the current plugin. This is the class that implements the EnvisionPlugin
 * 				interface.
 *	@param testName
 *					the name of this test. This name can be used at the command line to request that this specific test be
 *					started.
 */
/**********************************************************************************************************************/
#define TEST(pluginClass, testName)																												\
class pluginClass##Test##testName : public SelfTest::Test																				\
{																																							\
		static int initTrigger;																														\
		pluginClass##Test##testName (const QString &name) : Test (name) {}															\
																																							\
	public:																																				\
		static Test* create##pluginClass##Test##testName ()																				\
		{																																					\
			return new pluginClass##Test##testName (#testName);																			\
		}																																					\
																																							\
		static int init ()																															\
		{																																					\
			SelfTest::TestManager<pluginClass>::add(create##pluginClass##Test##testName, #testName);							\
			return 0;																																	\
		}																																					\
																																							\
		void runCustom(SelfTest::TestResults& testResults, bool &passed);																\
};																																							\
int pluginClass##Test##testName::initTrigger = pluginClass##Test##testName::init();												\
void pluginClass##Test##testName::runCustom(SelfTest::TestResults& testResults, bool &allChecksPassedFlag)
/**********************************************************************************************************************/

#endif /* TEST_H_ */
