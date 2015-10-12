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

#include "cppimport_api.h"

#include <ModelBase/src/nodes/Node.h>

namespace CppImport {

class CPPIMPORT_API NodeMapping
{
	public:
		void add(Model::Node* original, Model::Node* clone)
		{
			clones_[clone] = original;
			originals_[original] = clone;
		}

		Model::Node* original(Model::Node* clone)
		{
			return clones_[clone];
		}

		QVector<Model::Node*> original(QVector<Model::Node*> clones)
		{
			QVector<Model::Node*> result;
			for (auto clone : clones)
				result.append(original(clone));
			return result;
		}

		Model::Node* clone(Model::Node* original)
		{
			return originals_[original];
		}

		QVector<Model::Node*> clone(QVector<Model::Node*> originals)
		{
			QVector<Model::Node*> result;
			for (auto original : originals)
				result.append(clone(original));
			return result;
		}

		void replaceClone(Model::Node* clone, Model::Node* replacement)
		{
			auto original = clones_[clone];
			clones_.remove(clone);
			add(original, replacement);
		}

	private:
		QHash<Model::Node*, Model::Node*> clones_;
		QHash<Model::Node*, Model::Node*> originals_;
};

}
