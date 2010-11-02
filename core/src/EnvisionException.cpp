/***********************************************************************************************************************
 * EnvisionException.cpp
 *
 *  Created on: Oct 29, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "EnvisionException.h"
#include <iostream>

namespace Envision {

EnvisionException::EnvisionException() :
		causedBy(NULL)
{
}

EnvisionException::EnvisionException(QString reason_) :
	reason(reason_), causedBy(NULL)
{
}

EnvisionException::EnvisionException(QString reason_, EnvisionException& causedBy_) :
	reason(reason_), causedBy(new EnvisionException(causedBy_) )
{
}

EnvisionException::~EnvisionException()
{
	if (causedBy)
	{
		delete causedBy;
		causedBy = NULL;
	}
}

QString EnvisionException::getExceptionName()
{
	return "EnvisionException";
}

QString EnvisionException::getExceptionReason()
{
	return reason;
}

EnvisionException* EnvisionException::getCausingException()
{
	return causedBy;
}

void EnvisionException::printError()
{
	std::cerr << "Exception " << qPrintable(getExceptionName()) << ": " << qPrintable(reason) << std::endl;
	if ( causedBy )
	{
		std::cerr << "This error was caused by: " << std::endl;
		causedBy->printError();
	}
}

}
