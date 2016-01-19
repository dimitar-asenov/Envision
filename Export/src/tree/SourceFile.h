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

#pragma once

#include "../export_api.h"
#include "SourceFragment.h"
#include "TextFragment.h"

namespace Export {

class SourceDir;

class EXPORT_API SourceFile {
	public:
		SourceFile(SourceDir* parent, const QString& name);
		~SourceFile();

		const QString& name() const;
		/**
		 * Returns the relative path of this file including the name at the end.
		 */
		QString path() const;
		QList<SourceFragment*> fragments();

		template <typename T> T* append(T* fragment);
		TextFragment* append(Model::Node* node, const QString& text);

	private:
		SourceDir* parent_{};
		QString name_;

		QList<SourceFragment*> fragments_;
};

inline const QString& SourceFile::name() const { return name_; }
inline QList<SourceFragment*> SourceFile::fragments() { return fragments_; }

template <typename T>
inline T* SourceFile::append(T* fragment) { Q_ASSERT(fragment); fragments_.append(fragment); return fragment;}
inline TextFragment* SourceFile::append(Model::Node* node, const QString& text)
{ return append(new TextFragment{node, text}); }

}
