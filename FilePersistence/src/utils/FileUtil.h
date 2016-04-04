/***********************************************************************************************************************
**
** Copyright (c) 2011, 2016 ETH Zurich
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

#include "../filepersistence_api.h"

#include "Core/src/EnvisionException.h"

#include "../FilePersistencePlugin.h"

#include "Logger/src/Log.h"

namespace FilePersistence {

class FILEPERSISTENCE_API FileUtil
{
	public:

		/**
		 * Deletes all files from \a oldFiles which are not contained in \a newFiles.
		 * The entries in \a oldFiles will be deleted during the process.
		 */
		template<typename Container>
		static void deleteUnnecessaryFiles(Container& oldFiles, Container& newFiles)
		{

			// Remove from the list all previous exports that are also current exports
			for (const auto& current : newFiles)
				oldFiles.erase(current);

			auto it = oldFiles.begin();
			while (it != oldFiles.end())
			{
				QFileInfo file{*it};
				if (file.isFile())
				{
					FilePersistencePlugin::log().info("Removing unnecessary file: " + file.absoluteFilePath());
					if (!QFile{*it}.remove())
						throw Core::EnvisionException{"Could not remove previously generated file: " + file.absoluteFilePath()};

					it = oldFiles.erase(it);
				}
				else ++it;
			}

			// Now, delete empty directories
			it = oldFiles.begin();
			while (it != oldFiles.end())
			{
				QDir dir{*it};
				if (dir.entryInfoList(QDir::NoDot | QDir::NoDotDot | QDir::AllEntries).isEmpty())
				{
					FilePersistencePlugin::log().info("Removing unnecessary directory: " + dir.absolutePath());
					if (!dir.removeRecursively())
						throw Core::EnvisionException{"Could not remove directory : " + dir.absolutePath()};

					it = oldFiles.erase(it);
				}
				else ++it;
			}
		}
};

}
