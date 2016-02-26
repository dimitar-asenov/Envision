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
#include "Profiler.h"

namespace Core {

ProfileJob* Profiler::currentJob_{nullptr};
QMap<QString, ProfileJob*> Profiler::jobMap_;

static constexpr bool DISABLE_ALL = true;
static constexpr bool DISABLE_CALL_PROFILING = false || DISABLE_ALL;
static constexpr bool DISABLE_TIME_MEASUREMENT = false || DISABLE_ALL;

void Profiler::start(bool condition, const QString& jobName, const QString& fileName)
{
	if (DISABLE_ALL) return;

	Q_ASSERT(!currentJob_);
	if (condition)
	{
		currentJob_ = findJob(jobName, fileName);
		startProfiling();
	}
}

void Profiler::startOnce(bool condition, const QString& jobName, const QString& fileName)
{
	if (DISABLE_ALL) return;

	Q_ASSERT(!currentJob_);
	if (condition)
	{
		auto job = findJob(jobName, fileName);
		Q_ASSERT(job);
		if (job->startedAtLeastOnce == false)
		{
			currentJob_ = job;
			startProfiling();
		}
	}
}

void Profiler::startProfiling()
{
	Q_ASSERT(currentJob_);
	currentJob_->startedAtLeastOnce = true;
	qDebug() << "Profiling" << currentJob_->jobName;
	if (!DISABLE_TIME_MEASUREMENT) currentJob_->timer.start();
	if (!DISABLE_CALL_PROFILING) ProfilerStart(currentJob_->fileName.toStdString().c_str());
}

void Profiler::stop(const QString& jobName)
{
	if (DISABLE_ALL) return;

	if (currentJob_ && currentJob_->jobName == jobName)
	{
		if (!DISABLE_CALL_PROFILING) ProfilerStop();

		if (!DISABLE_TIME_MEASUREMENT)
		{
			qDebug() << "Profiling of" << currentJob_->jobName << "finished in"
						<< currentJob_->timer.nsecsElapsed() / 1000000000.0 << "seconds.";
			currentJob_->timer.restart();
		}
		currentJob_ = nullptr;
	}
}

ProfileJob* Profiler::findJob(const QString& jobName, const QString& fileName)
{
	auto jobEntry = jobMap_.find(jobName);
	if (jobEntry != jobMap_.end()) return *jobEntry;

	auto job = new ProfileJob{};
	job->jobName = jobName;
	job->fileName = fileName;
	jobMap_.insert(jobName, job);
	return job;
}

}
