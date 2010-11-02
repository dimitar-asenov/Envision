/***********************************************************************************************************************
 * EnvisionException.h
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ENVISIONEXCEPTION_H_
#define ENVISIONEXCEPTION_H_

#include <QString>

namespace Envision {

class EnvisionException
{
	private:
		QString reason;
		EnvisionException* causedBy;

	public:
		EnvisionException();
		EnvisionException(QString reason);
		EnvisionException(QString reason, EnvisionException& causedBy);

		virtual ~EnvisionException();

		virtual QString getExceptionName();
		virtual QString getExceptionReason();
		virtual EnvisionException* getCausingException();

		void printError();
};

}

#endif /* ENVISIONEXCEPTION_H_ */
