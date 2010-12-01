/***********************************************************************************************************************
 * TestResults.cpp
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "TestResults.h"
#include <QTextStream>

namespace SelfTest {

TestResults::TestResults() :
	numPassedTests(0), numFailedTests(0), numPassedChecks(0), numFailedChecks(0)
{
}

TestResults::~TestResults()
{
}

void TestResults::addPassedTest(const QString& testName)
{
	testResults.append(TestResult(TestResult::TestPassed, testName));
	numPassedTests++;
}

void TestResults::addFailedTest(const QString& testName)
{
	testResults.append(TestResult(TestResult::TestFailed, testName));
	numFailedTests++;
}

void TestResults::addPassedCheck(const QString& message)
{
	checkResults.append(TestResult(TestResult::TestPassed, message));
	numPassedChecks++;
}

void TestResults::addFailedCheck(const QString& message)
{
	checkResults.append(TestResult(TestResult::TestFailed, message));
	numFailedChecks++;
}

int TestResults::getNumExecutedTests() const
{
	return numPassedTests + numFailedTests;
}

int TestResults::getNumPassedTests() const
{
	return numPassedTests;
}

int TestResults::getNumFailedTests() const
{
	return numFailedTests;
}

int TestResults::getNumExecutedChecks() const
{
	return numPassedChecks + numFailedChecks;
}

int TestResults::getNumPassedChecks() const
{
	return numPassedChecks;
}

int TestResults::getNumFailedChecks() const
{
	return numFailedChecks;
}

const QList<TestResult>& TestResults::getTestResults() const
{
	return testResults;
}

const QList<TestResult>& TestResults::getCheckResults() const
{
	return checkResults;
}

void TestResults::printResultStatistics() const
{
	QTextStream out(stdout);
	out << "--------------------------" << endl;
	out << "Total executed tests : " << getNumExecutedTests() << "\t(" << getNumExecutedChecks() << ")" << endl;
	out << "Passed               : " << numPassedTests << "\t(" << getNumPassedChecks() << ")" << endl;
	out << "Failed               : " << numFailedTests << "\t(" << getNumFailedChecks() << ")" << endl;
	out << "--------------------------" << endl;

	if ( numFailedChecks > 0 || numFailedTests > 0)
	{
		out << endl << "Error messages" << endl;
		for (QList<TestResult>::const_iterator r = testResults.begin(); r != testResults.end(); r++)
			if ( r->isFailed() ) out << "Test FAILED: " << r->getMessage() << endl;

		for (QList<TestResult>::const_iterator r = checkResults.begin(); r != checkResults.end(); r++)
					if ( r->isFailed() ) out << "Failure:" << r->getMessage() << endl;
	}
}

}
