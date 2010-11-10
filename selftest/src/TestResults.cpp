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
	numPassed(0), numFailed(0), numInconclusive(0)
{
}

TestResults::~TestResults()
{
}

void TestResults::addPassed(const QString& message)
{
	results.append( TestResult(TestResult::TestPassed, message) );
	numPassed++;
}

void TestResults::addFailed(const QString& message)
{
	results.append( TestResult(TestResult::TestFailed, message) );
	numFailed++;
}

void TestResults::addInconclusive(const QString& message)
{
	results.append( TestResult(TestResult::TestInconclusive, message) );
	numInconclusive++;
}

int TestResults::getNumExecuted() const
{
	return numPassed + numFailed + numInconclusive;
}

int TestResults::getNumPassed() const
{
	return numPassed;
}

int TestResults::getNumFailed() const
{
	return numFailed;
}

int TestResults::getNumInconclusive() const
{
	return numInconclusive;
}

const QList<TestResult>& TestResults::getResults() const
{
	return results;
}

void TestResults::printResultStatistics() const
{
	QTextStream out(stdout);
	out << "--------------------------" << endl;
	out << "Total executed tests : " << getNumExecuted() << endl;
	out << "Passed               : " << numPassed << endl;
	out << "Failed               : " << numFailed << endl;
	out << "Inconclusive         : " << numInconclusive << endl;
	out << "--------------------------" << endl;

	if (numFailed > 0 || numInconclusive > 0)
	{
		out << endl << "Error messages" << endl;
		for (QList<TestResult>::const_iterator r = results.begin(); r != results.end(); r++)
		{
			if (r->isFailed()) out << "FAILED: " << r->getMessage() << endl;
			if (r->isInconclusive())  out << "INCONCLUSIVE: " << r->getMessage() << endl;
		}
	}
}

}
