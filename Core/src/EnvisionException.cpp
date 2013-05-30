/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/
#include "EnvisionException.h"

namespace Core {

bool& EnvisionException::assertOnThrow()
{
#if defined(DEBUG)
	// This helps with debugging duing development
	static bool aot = true;
#else
	static bool aot = false;
#endif
	return aot;
}

EnvisionException::EnvisionException()
{
	if (assertOnThrow()) Q_ASSERT_X(false, "Exception thrown", "");
}

EnvisionException::EnvisionException(const QString& message) : msg_{message}
{
	if (assertOnThrow()) Q_ASSERT_X(false, "Exception thrown", message.toAscii());
}

EnvisionException::~EnvisionException() {}

const QString& EnvisionException::name() const
{
	static QString ename("EnvisionException");
	return ename;
}

const QString& EnvisionException::message() const
{
	return msg_;
}

void EnvisionException::printError() const
{
	QTextStream err(stderr);
	err << "Exception " << qPrintable( name() ) << ": " << qPrintable( message() ) << endl;
}

}
