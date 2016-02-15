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

#include "../modelbase_api.h"

#include "composite/CompositeNode.h"
#include "Text.h"
#include "nodeMacros.h"
#include "../model/TreeManager.h"

namespace Model { class UsedLibrary; }
extern template class MODELBASE_API Model::TypedList<Model::UsedLibrary>;

namespace Model {

class Node;
class PersistentStore;

class MODELBASE_API UsedLibrary : public Super<CompositeNode>
{
	COMPOSITENODE_DECLARE_STANDARD_METHODS(UsedLibrary)
	ATTRIBUTE_VALUE_CUSTOM_RETURN(Text, name, setName, QString, const QString&)

	public:
		UsedLibrary(const QString& name);
		virtual QList<const UsedLibrary*> usedLibraries() const override;

		Node* libraryRoot() const;
		TreeManager* libraryManager() const;
		void loadLibrary(PersistentStore* store) const;
};

inline Node* UsedLibrary::libraryRoot() const { if (auto m = libraryManager()) return m->root(); else return nullptr;}

}
