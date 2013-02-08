/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
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

#ifndef TEST_H_
#define TEST_H_

#include "selftest_api.h"
#include "TestResults.h"

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
