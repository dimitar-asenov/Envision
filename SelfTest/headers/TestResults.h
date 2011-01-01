/***********************************************************************************************************************
 * TestResults.h
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TESTRESULTS_H_
#define TESTRESULTS_H_

#include "selftest_api.h"
#include "TestResult.h"
#include <QtCore/QList>

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
class SELFTEST_API TestResults
{
	private:
		QList<TestResult> testResults;
		QList<TestResult> checkResults;

		int numPassedTests;
		int numFailedTests;

		int numPassedChecks;
		int numFailedChecks;

	public:
		TestResults();
		virtual ~TestResults();

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
};

}

#endif /* TESTRESULTS_H_ */
