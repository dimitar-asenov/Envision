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

#pragma once

#include "../oodebug_api.h"

#include "CompilerFeedback.h"

namespace OODebug {

/**
 * A wrapper class for command line compilers.
 */
class OODEBUG_API CommandLineCompiler
{
	public:
		/**
		 * Creates a new \a CommandLineCompiler which will use the command \a compilerCommand
		 * and \a parseFunction for parsing the output.
		 */
		CommandLineCompiler(const QString& compilerCommand,
								  std::function<CompilerFeedback(const QString&)> parseFunction)
			: command_{compilerCommand}, parseFunction_{parseFunction} { Q_ASSERT(parseFunction); }

		/**
		 * Starts the compile command in the directory \a workingDirectory and
		 * compiles the file with name \a fileName using the arguments as in \a args.
		 *
		 * If there are problems (like e.g. missing command) this method throws an OODebugException.
		 *
		 * Note: This call is blocking, it blocks until the command is finished.
		 */
		CompilerFeedback compileFile(const QString& workingDirectory, const QString& fileName,
											  const QStringList& args = {});

	private:
		QString command_;
		std::function<CompilerFeedback(const QString&)> parseFunction_;
};

}
