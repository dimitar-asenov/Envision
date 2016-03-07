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

#include "TestResult.h"

namespace SelfTest {

/**
 * The TestResults class describes the outcome of series of tests run consecutively.
 *
 * Two things are logged here:
 * 	- Fine-grained: checks (assertions).  A single test may consist of many such checks.
 * 	- Coarse-grained: tests.
 *
 * Typically a check is only logged if it fails, providing failure details.
 * A test should be logged in all cases (failure or success).
 */
class SELFTEST_API TestResults final
{
	public:
		void addPassedTest(const QString& testName);
		void addFailedTest(const QString& testName);

		void addPassedCheck(const QString& message = "OK");
		void addFailedCheck(const QString& message);

		int getNumExecutedTests() const;
		int getNumPassedTests() const;
		int getNumFailedTests() const;

		int getNumExecutedChecks() const;
		int getNumPassedChecks() const;
		int getNumFailedChecks() const;

		const QList<TestResult>& getTestResults() const;
		const QList<TestResult>& getCheckResults() const;

		/**
		 * Prints a summary of the test results to the standard output.
		 */
		void printResultStatistics() const;

		void merge(const TestResults& other);

	private:
		QList<TestResult> testResults_;
		QList<TestResult> checkResults_;

		int numPassedTests_{};
		int numFailedTests_{};

		int numPassedChecks_{};
		int numFailedChecks_{};
};

inline int TestResults::getNumExecutedTests() const { return numPassedTests_ + numFailedTests_; }
inline int TestResults::getNumPassedTests() const { return numPassedTests_; }
inline int TestResults::getNumFailedTests() const { return numFailedTests_; }
inline int TestResults::getNumExecutedChecks() const { return numPassedChecks_ + numFailedChecks_; }
inline int TestResults::getNumPassedChecks() const { return numPassedChecks_; }
inline int TestResults::getNumFailedChecks() const { return numFailedChecks_; }
inline const QList<TestResult>& TestResults::getTestResults() const { return testResults_; }
inline const QList<TestResult>& TestResults::getCheckResults() const { return checkResults_; }

}
