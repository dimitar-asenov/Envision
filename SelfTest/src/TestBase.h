/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#pragma once

#include "selftest_api.h"

namespace SelfTest {

class TestResults;

/**
 * The Test class is the base class for a single test for the Envision testing framework.
 */
class SELFTEST_API TestBase
{
	public:
		using TestConstructor = std::function<TestBase* ()>;

		TestBase(const QString &name);
		virtual ~TestBase();

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
		 * 				Test failures are added here.
		 * @param passed
		 * 				must be set to false if the test failed.
		 */
		virtual void runCustom(TestResults& testResults, bool &passed) = 0;

	private:
		QString name;
};

}
