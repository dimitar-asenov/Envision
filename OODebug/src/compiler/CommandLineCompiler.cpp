/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#include "CommandLineCompiler.h"

#include "../OODebugException.h"

namespace OODebug {

CompilerFeedback CommandLineCompiler::compileFile(const QString& workingDirectory, const QString& fileName,
																  const QStringList& args)
{
	QProcess compilerProcess;
	compilerProcess.setWorkingDirectory(workingDirectory);
	compilerProcess.setProcessChannelMode(QProcess::MergedChannels);
	compilerProcess.start(command_, QStringList() << args << fileName);
	// block until finished
	compilerProcess.waitForFinished(-1);
	// check if everything went fine
	auto error = compilerProcess.error();
	// If the process failed to start the exitStatus is still QProcess::NormalExit.
	// Thus we have to check here if the process failed to start.
	// Uknown error is the default return value of error() -> if the process exited correctly unknown error is returned.
	if (error != QProcess::UnknownError)
	{
		if (error == QProcess::FailedToStart)
			throw new OODebugException{QString("It seems like %1 is not installed on your system!").arg(command_)};
		else
			throw new OODebugException{QString("Error while executing %1: %2").arg(command_, error)};
	}
	if (compilerProcess.exitStatus() != QProcess::NormalExit)
		throw new OODebugException{QString("Compiler crash, command: %1").arg(command_)};
	return parseFunction_(QString(compilerProcess.readAllStandardOutput()));
}

}
