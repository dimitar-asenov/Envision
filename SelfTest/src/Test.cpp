/***********************************************************************************************************************
 * Test.cpp
 *
 *  Created on: Nov 9, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "Test.h"
#include "TestManager.h"
#include "EnvisionException.h"

namespace SelfTest {

Test::Test(const QString &name_) :
	name(name_)
{
}

Test::~Test()
{
}

const QString& Test::getName() const
{
	return name;
}

void Test::run(TestResults& testResults)
{
	bool passed = true;

	try
	{
		runCustom(testResults, passed);
	}
	catch (const Envision::EnvisionException& e)
	{
		passed = false;
		testResults.addFailedCheck(getName() +": " + e.name() + " " + e.message());
	}
	catch (...)
	{
		passed = false;
		testResults.addFailedCheck(getName() +": Uncaught exception of unknown type");
	}

	if (passed) testResults.addPassedTest(getName());
	else testResults.addFailedTest(getName());
}

}
