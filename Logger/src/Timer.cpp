/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
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

#include "Timer.h"

namespace Logger {

QMap<QString, Timer*>& Timer::timers()
{
	static QMap<QString, Timer*> t;
	return t;
}


Timer* Timer::timer(const QString& timer)
{
	if (timers().contains(timer))
		return timers().value(timer);

	auto t = new Timer{};
	timers().insert(timer, t);
	return t;
}

void Timer::removeAllTimers()
{
	for (auto t : timers().values()) SAFE_DELETE(t);
	timers().clear();
}

Timer::Timer(){}

void Timer::setNumValuesLimit(int limit)
{
	Q_ASSERT(limit >= 0);
	numValuesLimit_ = limit;
	while (values_.size() > limit) values_.removeFirst();
}

void Timer::start()
{
	qtTimer_.start();
}

qint64 Timer::tick()
{
	auto val = qtTimer_.restart();
	if (val == 0 && ignoreZeroValues_) return 0;

	values_.append(val);
	if (values_.size() > numValuesLimit_) values_.removeFirst();
	totalSum_ += val;
	++totalCount_;

	return val;
}

double Timer::latestValuesAverage() const
{
	double sum = 0;
	for (auto v : values_) sum += v;
	return sum / values_.size();
}

}
