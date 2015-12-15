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
// Note that this file is assumed to be loaded after all the system headers in Core/src/precompiled.h have been already
// included


template <typename T> inline void SAFE_DELETE( T* & object)
{
	if (object)
	{
		delete object;
		object = nullptr;
	}
}

template <typename T> inline void SAFE_DELETE( T* && object)
{
	if (object) delete object;
}

class OnScopeExit
{
	public:
		OnScopeExit(const OnScopeExit& other) = delete;
		OnScopeExit(OnScopeExit&& other) = delete;
		OnScopeExit& operator= (const OnScopeExit other) = delete;

		template<typename T>
		explicit OnScopeExit(T&& functionToCall) : functionToCall_{std::forward<T>(functionToCall)}{}

		~OnScopeExit() {functionToCall_();}

	private:
		std::function<void ()> functionToCall_;
};

class SystemCommandResult {
	public:
		int exitCode() const;
		QStringList stdout() const;
		QString stdoutOneLine() const;
		QStringList stderr() const;

		operator QString() const;
		operator QStringList() const;

	private:
		friend SystemCommandResult runSystemCommand(const QString& command, const QStringList& arguments,
																  const QString& workingDirectory);

		int exitCode_{};
		QStringList stdout_;
		QStringList stderr_;
};

inline int SystemCommandResult::exitCode() const { return exitCode_; }
inline QStringList SystemCommandResult::stdout() const { return stdout_; }
inline QStringList SystemCommandResult::stderr() const { return stderr_; }

SystemCommandResult runSystemCommand(const QString& program, const QStringList& arguments = {},
									  const QString& workingDirectory = QString{});
