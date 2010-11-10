/***********************************************************************************************************************
 * TestResult.cpp
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "TestResult.h"

namespace SelfTest {

TestResult::TestResult(TestOutcome outcome_, const QString& message_) :
		outcome(outcome_), message(message_)
{
}

TestResult::~TestResult()
{
}

const QString& TestResult::getMessage() const
{
	return message;
}

bool TestResult::isPassed() const
{
	return outcome == TestPassed;
}

bool TestResult::isFailed() const
{
	return outcome == TestFailed;
}

bool TestResult::isInconclusive() const
{
	return outcome == TestInconclusive;
}

TestResult::TestOutcome TestResult::getOutcome() const
{
	return outcome;
}

}
