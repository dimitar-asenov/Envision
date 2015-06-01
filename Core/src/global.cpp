/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "precompiled.h"
#include "core.h"

QString SystemCommandResult::stdoutOneLine() const {
	Q_ASSERT(stdout_.size() == 1);
	return stdout_.first();
}

SystemCommandResult::operator QString() const
{
	Q_ASSERT(exitCode() == 0);
	Q_ASSERT(stderr_.size() == 0);
	return stdoutOneLine();
}

SystemCommandResult::operator QStringList() const
{
	Q_ASSERT(exitCode() == 0);
	Q_ASSERT(stderr_.size() == 0);
	return stdout_;
}

SystemCommandResult runSystemCommand(const QString& program, const QStringList& arguments,
												 const QString& workingDirectory)
{
	QProcess process;
	process.setProgram(program);
	if (!arguments.isEmpty()) process.setArguments(arguments);
	if (!workingDirectory.isNull()) process.setWorkingDirectory(workingDirectory);

	process.start();
	process.waitForFinished();

	SystemCommandResult result;
	result.exitCode_ = process.exitCode();

	auto EOLRegex = QRegularExpression("(\\r\\n|\\r|\\n)");
	result.stdout_ = QString(process.readAllStandardOutput()).split(EOLRegex);
	if (!result.stdout_.isEmpty() && result.stdout_.last().isEmpty())  result.stdout_.removeLast();
	result.stderr_ = QString(process.readAllStandardError()).split(EOLRegex);
	if (!result.stderr_.isEmpty() && result.stderr_.last().isEmpty())  result.stderr_.removeLast();

	return result;
}
