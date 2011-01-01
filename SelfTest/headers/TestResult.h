/***********************************************************************************************************************
 * TestResult.h
 *
 *  Created on: Nov 10, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef TESTRESULT_H_
#define TESTRESULT_H_

#include "selftest_api.h"
#include <QtCore/QString>

namespace SelfTest {

/**
 * The TestResult class describes the outcome of a single test or check.
 *
 * An outcome consists of two fields:
 * 	- Outcome code (passed, failed)
 * 	- Outcome message (optional)
 */
class SELFTEST_API TestResult
{
	public:
		typedef enum{TestPassed, TestFailed} TestOutcome;

	private:
		TestOutcome outcome;
		QString message;

	public:
		TestResult(TestOutcome outcome, const QString& message);
		virtual ~TestResult();

		const QString& getMessage() const;

		bool isPassed() const;
		bool isFailed() const;

		TestOutcome getOutcome() const;
};

}

#endif /* TESTRESULT_H_ */
