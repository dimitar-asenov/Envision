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

#pragma once

#include "FileDependencies.h"

namespace Model {
	class Node;
}

namespace CppExport {

/**
 * container storing trees to be put in the same physical file when exporting.
 * serves as dependency unit for dependency analysis.
 */
class File
{
	public:
		File(QString name, Model::Node* node, File* source = nullptr);

		bool isHeaderFile();
		void addNode(Model::Node* node);

		const QString& name() const;
		File* source();
		QSet<Model::Node*> nodes();
		FileDependencies& dependencies();

		static QString printFileSet(QString description, QSet<File*> files);

	private:
		QString name_;
		File* source_{};
		QSet<Model::Node*> nodes_;
		FileDependencies dependencies_;
};

inline bool File::isHeaderFile() { return source_; }
inline const QString& File::name() const { return name_; }
inline File* File::source() { return source_; }
inline QSet<Model::Node*> File::nodes() { return nodes_; }
inline FileDependencies& File::dependencies() { return dependencies_; }

}
