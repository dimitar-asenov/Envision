/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2015 ETH Zurich
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

#include "FileDependencies.h"

#include "File.h"

namespace CppExport {

FileDependencies::FileDependencies() {}

FileDependencies::FileDependencies(QSet<File*> softDependencies, QSet<File*> hardDependencies) :
	softDependencies_(softDependencies), hardDependencies_(hardDependencies) {}

void FileDependencies::addDependency(File* file, bool softDependency)
{
	if (softDependency)
	{
		if (!hardDependencies_.contains(file))
			softDependencies_.insert(file);
	}
	else
	{
		softDependencies_.remove(file);
		hardDependencies_.insert(file);
	}
}

QSet<File*> FileDependencies::hardDependencyTransitiveReduction()
{
	QSet<File*> reachableHardDependencies;
	for (auto hardDependency : hardDependencies_)
		calculateReachableHardDependencies(hardDependency, reachableHardDependencies);

	return hardDependencies().subtract(reachableHardDependencies);
}

void FileDependencies::calculateReachableHardDependencies(File* file, QSet<File*>& reachableHardDependencies)
{
	for (auto hardDependency : file->dependencies().hardDependencies())
		if (!reachableHardDependencies.contains(hardDependency))
		{
			reachableHardDependencies.insert(hardDependency);
			calculateReachableHardDependencies(hardDependency, reachableHardDependencies);
		}
}

FileDependencies FileDependencies::transitiveReduction()
{
	QSet<File*> reducedHardDependencies = hardDependencyTransitiveReduction();

	QSet<File*> reachableSoftDependencies;
	for (auto hardDependency : reducedHardDependencies)
	{
		reachableSoftDependencies.insert(hardDependency);

		for (auto softDependency : hardDependency->dependencies().softDependencies())
			reachableSoftDependencies.insert(softDependency);
	}
	QSet<File*> reducedSoftDependencies = softDependencies().subtract(reachableSoftDependencies);

	return FileDependencies(reducedSoftDependencies, reducedHardDependencies);
}

}
