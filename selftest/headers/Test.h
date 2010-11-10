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
#include <QString>

namespace SelfTest {

template<class T> class TestManager;

class SELFTEST_API Test
{
	private:
		QString name;

	public:
		typedef Test* (*TestConstructor)();

		Test(const QString &name);
		virtual ~Test();

		const QString& getName() const;
		void run(TestResults& testResults);
		virtual void runCustom(TestResults& testResults) = 0;
};

}

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
		void runCustom(SelfTest::TestResults& testResults);																				\
};																																							\
int pluginClass##Test##testName::initTrigger = pluginClass##Test##testName::init();												\
void pluginClass##Test##testName::runCustom(SelfTest::TestResults& testResults)
/**********************************************************************************************************************/

#endif /* TEST_H_ */
