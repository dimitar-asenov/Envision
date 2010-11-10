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
#include <QList>

namespace SelfTest {

class SELFTEST_API TestResults
{
	private:
		QList<TestResult> results;

		int numPassed;
		int numFailed;
		int numInconclusive;

	public:
		TestResults();
		virtual ~TestResults();

		void addPassed(const QString& message = "OK");
		void addFailed(const QString& message);
		void addInconclusive(const QString& message);

		int getNumExecuted() const;
		int getNumPassed() const;
		int getNumFailed() const;
		int getNumInconclusive() const;

		const QList<TestResult>& getResults() const;

		void printResultStatistics() const;
};

}

#endif /* TESTRESULTS_H_ */
