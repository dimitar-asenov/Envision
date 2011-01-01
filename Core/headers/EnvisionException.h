/***********************************************************************************************************************
 * EnvisionException.h
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef ENVISIONEXCEPTION_H_
#define ENVISIONEXCEPTION_H_

#include <QtCore/QString>
#include <QtCore/QTextStream>

namespace Envision {

/**
 * The EnvisionException class is the base class of all exception classes in Envision.
 *
 * It provides simple facilities to store and print an error message.
 */
class EnvisionException
{
	private:
		QString msg;

	public:
		EnvisionException()
		{
		}

		EnvisionException(const QString& message) :
			msg(message)
		{
		}

		virtual ~EnvisionException()
		{
		}

		virtual const QString& name() const
		{
			static QString ename("EnvisionException");
			return ename;
		}

		virtual const QString& message() const
		{
			return msg;
		}

		virtual void printError() const
		{
			QTextStream err(stderr);
			err << "Exception " << qPrintable( name() ) << ": " << qPrintable( message() ) << endl;
		}
};

}

#endif /* ENVISIONEXCEPTION_H_ */
