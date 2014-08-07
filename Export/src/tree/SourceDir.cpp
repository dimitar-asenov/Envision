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
#include "SourceDir.h"

namespace Export {

SourceDir::SourceDir(SourceDir* parent, const QString& name) : parent_{parent}, name_{name} {}

template<class T> T* SourceDir::find(const QString& name, QList<T>& container, bool createIfNotFound)
{
	auto found = std::find_if(container.begin(), container.end(),
			[&name](const T& element){return element.name() == name;});

	if (found != container.end())
		return &(*found);

	if (createIfNotFound)
	{
		container.append(T(this, name));
		return &container.last();
	}

	return nullptr;
}

SourceDir* SourceDir::findDirectories(const QString& name, bool createIfNotFound)
{
	int skipFirst = name.startsWith('/') ? 1 : 0;
	int mid = name.indexOf('/');
	QString first = name.mid(skipFirst, mid - skipFirst);
	QString remaining = name.mid(mid+1);

	auto found = find(first, directories_, createIfNotFound);
	if (!found || remaining.isEmpty()) return found;
	return found->findDirectories(remaining, createIfNotFound);
}

bool SourceDir::hasSubDir(const QString& subDirName) const
{
	return const_cast<SourceDir*>(this)->findDirectories(subDirName, false);
}

SourceDir& SourceDir::subDir(const QString& subDirName)
{
	return *findDirectories(subDirName, true);
}

bool SourceDir::hasFile(const QString& fileName) const
{
	int splitIndex = fileName.lastIndexOf('/');
	if (splitIndex < 0)
		return const_cast<SourceDir*>(this)->find(fileName, const_cast<QList<SourceFile>&>(files_), false);

	auto foundDir = const_cast<SourceDir*>(this)->findDirectories(fileName.left(splitIndex), false);
	if (foundDir) return const_cast<SourceDir*>(foundDir)->find(fileName.mid(splitIndex+1), foundDir->files_, false);
	return false;
}

SourceFile& SourceDir::file(const QString& fileName)
{
	int splitIndex = fileName.lastIndexOf('/');
	if (splitIndex < 0) return *find(fileName, files_, true);

	auto foundDir = findDirectories(fileName.left(splitIndex), true);
	return * (foundDir->find(fileName.mid(splitIndex+1), foundDir->files_, true));
}

} /* namespace Export */
