/***********************************************************************************************************************
 * LogTester.h
 *
 *  Created on: Nov 3, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef LOGTESTER_H_
#define LOGTESTER_H_

#include <QObject>
#include "Log.h"

namespace Logger {

class LogTester: public QObject
{
	Q_OBJECT

	private:
		int correctLogEntries;
		Log* log;

	public:
		LogTester();
		void runTests();
		bool allTestsOK();


	public slots:
		void newLogEntryAdded();
};

}

#endif /* LOGTESTER_H_ */
