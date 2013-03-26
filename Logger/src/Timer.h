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

#pragma once

#include "logger_api.h"

namespace Logger {

class LOGGER_API Timer
{
	public:
		static Timer* start(const QString& timer);
		static qint64 tick(const QString& timer);

		static QStringList timerNames();
		static Timer* timer(const QString& timer);
		static void removeAllTimers();

		void start();
		qint64 tick();
		const QString& name() const;
		const QList<qint64> values() const;
		void setNumValuesLimit(int limit);
		double totalAverage() const;
		double latestValuesAverage() const;

	private:
		Timer();
		static QMap<QString, Timer*>& timers();

		QString name_;
		QList<qint64> values_;
		QElapsedTimer qtTimer_;
		qint64 totalSum_{};
		qint64 totalCount_{};
		int numValuesLimit_{10};
};

inline Timer* Timer::start(const QString& timer) { auto t = Timer::timer(timer); t->start(); return t; }
inline qint64 Timer::tick(const QString& timer) { return Timer::timer(timer)->tick(); }
inline QStringList Timer::timerNames() { return timers().keys(); }

inline void Timer::start() { qtTimer_.start(); }
inline const QString& Timer::name() const { return name_; }
inline const QList<qint64> Timer::values() const { return values_; }
inline double Timer::totalAverage() const { return (double) totalSum_/totalCount_; }

} /* namespace Logger */
