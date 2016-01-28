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

template <typename Base, typename Derived>
inline void dynamicCastAndAssign(Base& base, Derived& derived)
{
	static_assert(std::is_base_of<typename Base::element_type, typename Derived::element_type>::value,
					  "dynamicCastAndAssign requires two classes that have an inheritance relationship");
	if (auto cast = dynamic_cast<typename Derived::pointer>(base.get()))
	{
		base.release();
		derived.reset(cast);
	}
}

class OnScopeExit
{
	public:
		OnScopeExit(const OnScopeExit& other) = delete;
		OnScopeExit(OnScopeExit&& other) = delete;
		OnScopeExit& operator= (const OnScopeExit other) = delete;

		template<typename T>
		explicit OnScopeExit(T&& functionToCall);

		~OnScopeExit();

	private:
		std::function<void ()> functionToCall_;
};

template <typename T>
inline OnScopeExit::OnScopeExit(T&& functionToCall) : functionToCall_{std::forward<T>(functionToCall)}{}
inline OnScopeExit::~OnScopeExit() { functionToCall_(); }

class SystemCommandResult {
	public:
		int exitCode() const;
		QStringList standardout() const;
		QString standardoutOneLine() const;
		QStringList standarderr() const;

		operator QString() const;
		operator QStringList() const;

	private:
		friend SystemCommandResult runSystemCommand(const QString& command, const QStringList& arguments,
																  const QString& workingDirectory);

		int exitCode_{};
		QStringList standardout_;
		QStringList standarderr_;
};

inline int SystemCommandResult::exitCode() const { return exitCode_; }
inline QStringList SystemCommandResult::standardout() const { return standardout_; }
inline QStringList SystemCommandResult::standarderr() const { return standarderr_; }

SystemCommandResult runSystemCommand(const QString& program, const QStringList& arguments = {},
												 const QString& workingDirectory = {});
