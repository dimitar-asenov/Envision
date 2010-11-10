/***********************************************************************************************************************
 * TestResult.h
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TESTRESULT_H_
#define TESTRESULT_H_

#include "selftest_api.h"
#include <QString>

namespace SelfTest {

class SELFTEST_API TestResult
{
	public:
		typedef enum{TestPassed, TestFailed, TestInconclusive} TestOutcome;

	private:
		TestOutcome outcome;
		QString message;

	public:
		TestResult(TestOutcome outcome, const QString& message);
		virtual ~TestResult();

		const QString& getMessage() const;

		bool isPassed() const;
		bool isFailed() const;
		bool isInconclusive() const;

		TestOutcome getOutcome() const;
};

}

#endif /* TESTRESULT_H_ */
