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

#include "TestResults.h"

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
	QTextStream err(stderr);
	out << "--------------------------" << endl;
	out << "Total executed tests : " << getNumExecutedTests() << "\t(" << getNumExecutedChecks() << ")" << endl;
	out << "Passed               : " << numPassedTests << "\t(" << getNumPassedChecks() << ")" << endl;
	out << "Failed               : " << numFailedTests << "\t(" << getNumFailedChecks() << ")" << endl;
	out << "--------------------------" << endl;
	out.flush();

	if ( numFailedChecks > 0 || numFailedTests > 0)
	{
		err << endl << "Error messages" << endl;
		for (QList<TestResult>::const_iterator r = testResults.begin(); r != testResults.end(); r++)
			if ( r->isFailed() ) err << "Test FAILED: " << r->getMessage() << endl;

		for (QList<TestResult>::const_iterator r = checkResults.begin(); r != checkResults.end(); r++)
					if ( r->isFailed() ) err << "Failure:" << r->getMessage() << endl;
	}
}

}
