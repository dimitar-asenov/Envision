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

#include "../filepersistence_api.h"
#include "GenericPersistentUnit.h"

namespace FilePersistence {

class GenericNode;

class FILEPERSISTENCE_API GenericTree {
	public:
		GenericTree(QString name = QString(), QString commitName = QString());
		~GenericTree();

		const QString& name() const;
		const QString& commitName() const;

		void remove(const QString& persistentUnitName);
		void remove(const GenericPersistentUnit& unit);

		GenericPersistentUnit& newPersistentUnit(QString name, char* data = nullptr, int dataSize = 0);
		GenericPersistentUnit* persistentUnit(const QString& name);

		GenericNode* find(Model::NodeIdType id) const;

	private:
		friend class GenericPersistentUnit;

		QString name_;
		QString commitName_;

		QHash<QString, GenericPersistentUnit> persistentUnits_;

		constexpr static int ALLOCATION_CHUNK_SIZE = 1000; // num elements to allocate at once
		QList<GenericNode*> emptyChunks_;

		GenericNode* emptyChunk();
		void releaseChunk(GenericNode* unusedChunk);
};

inline const QString& GenericTree::name() const { return name_; }
inline const QString& GenericTree::commitName() const { return commitName_; }

inline void GenericTree::remove(const QString& persistentUnitName) { persistentUnits_.remove(persistentUnitName); }
inline void GenericTree::remove(const GenericPersistentUnit& unit) { remove(unit.name()); }

} /* namespace FilePersistence */
