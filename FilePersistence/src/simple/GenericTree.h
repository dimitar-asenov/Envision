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
class PiecewiseLoader;
struct NodeData;

class FILEPERSISTENCE_API GenericTree {
	public:

		GenericTree(QString name = QString());
		~GenericTree();

		const QString& name() const;

		void remove(const QString& persistentUnitName);
		void remove(const GenericPersistentUnit& unit);

		GenericPersistentUnit& newPersistentUnit(QString name, char* data = nullptr, int dataSize = 0);
		GenericPersistentUnit* persistentUnit(const QString& name) const;
		QList<std::shared_ptr<GenericPersistentUnit>> persistentUnits() const;

		GenericNode* find(Model::NodeIdType id);

		void setPiecewiseLoader(std::shared_ptr<PiecewiseLoader> loader);
		void loadNode(Model::NodeIdType id);

		GenericNode* root() const;

		void buildLookupHash();

	private:
		friend class GenericPersistentUnit;
		friend class GenericNode;
		friend class PiecewiseLoader;

		QString name_;

		bool hasQuickLookupHash_{};
		QHash<Model::NodeIdType, GenericNode*> quickLookupHash_;
		QMultiHash<Model::NodeIdType, GenericNode*> nodesWithoutParents_;
		std::shared_ptr<PiecewiseLoader> piecewiseLoader_;

		QHash<QString, std::shared_ptr<GenericPersistentUnit>> persistentUnits_;

		constexpr static int ALLOCATION_CHUNK_SIZE = 1000; // num elements to allocate at once
		QList<GenericNode*> emptyChunks_;

		GenericNode* emptyChunk();
		void releaseChunk(GenericNode* unusedChunk);

		const std::shared_ptr<PiecewiseLoader>& piecewiseLoader() const;
		QMultiHash<Model::NodeIdType, GenericNode*>& nodesWithoutParents();
};

inline const QString& GenericTree::name() const { return name_; }

inline void GenericTree::remove(const QString& persistentUnitName) { persistentUnits_.remove(persistentUnitName); }
inline void GenericTree::remove(const GenericPersistentUnit& unit) { remove(unit.name()); }

inline const std::shared_ptr<PiecewiseLoader>& GenericTree::piecewiseLoader() const { return piecewiseLoader_;}
inline QMultiHash<Model::NodeIdType, GenericNode*>& GenericTree::nodesWithoutParents() { return nodesWithoutParents_;}
inline QList<std::shared_ptr<GenericPersistentUnit>> GenericTree::persistentUnits() const
	{return persistentUnits_.values();}
} /* namespace FilePersistence */
