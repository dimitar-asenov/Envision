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

#include "TestResults.h"

namespace SelfTest {

void TestResults::addPassedTest(const QString& testName)
{
	testResults_.append(TestResult{TestResult::TestPassed, testName});
	numPassedTests_++;
}

void TestResults::addFailedTest(const QString& testName)
{
	testResults_.append(TestResult{TestResult::TestFailed, testName});
	numFailedTests_++;
}

void TestResults::addPassedCheck(const QString& message)
{
	checkResults_.append(TestResult{TestResult::TestPassed, message});
	numPassedChecks_++;
}

void TestResults::addFailedCheck(const QString& message)
{
	checkResults_.append(TestResult{TestResult::TestFailed, message});
	numFailedChecks_++;
}

void TestResults::merge(const TestResults& other)
{
	testResults_ << other.testResults_;
	checkResults_ << other.checkResults_;
	numPassedTests_ += other.numPassedTests_;
	numFailedTests_ += other.numFailedTests_;
	numPassedChecks_ += other.numPassedChecks_;
	numFailedChecks_ += other.numFailedChecks_;
}

void TestResults::printResultStatistics() const
{
	QTextStream out{stdout};
	QTextStream err{stderr};
	out << "--------------------------" << Qt::endl;
	out << "Total executed tests : " << getNumExecutedTests() << "\t(" << getNumExecutedChecks() << ")" << Qt::endl;
	out << "Passed               : " << numPassedTests_ << "\t(" << getNumPassedChecks() << ")" << Qt::endl;
	out << "Failed               : " << numFailedTests_ << "\t(" << getNumFailedChecks() << ")" << Qt::endl;
	out << "--------------------------" << Qt::endl;
	out.flush();

	if ( numFailedChecks_ > 0 || numFailedTests_ > 0)
	{
		err << Qt::endl << "Error messages" << Qt::endl;
		for (QList<TestResult>::const_iterator r = testResults_.begin(); r != testResults_.end(); r++)
			if ( r->isFailed() ) err << "Test FAILED: " << r->getMessage() << Qt::endl;

		for (QList<TestResult>::const_iterator r = checkResults_.begin(); r != checkResults_.end(); r++)
					if ( r->isFailed() ) err << "Failure:" << r->getMessage() << Qt::endl;
	}
}

}
